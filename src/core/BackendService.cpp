#include "BackendService.hpp"
#include <spdlog/spdlog.h>
#include <sqlite3.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <map>

namespace VRMusicStudio {

struct BackendService::Impl {
    sqlite3* db = nullptr;
    SSL_CTX* sslContext = nullptr;
    std::string dbPath;
    std::string sslCertPath;
    std::string sslKeyPath;
    std::queue<std::function<void()>> changeQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::thread changeThread;
    bool running = false;
    std::function<void(const std::string&)> changeCallback;
    std::map<int, std::string> activeSessions; // projectId -> sessionId
};

BackendService::BackendService() : pImpl(std::make_unique<Impl>()) {
    pImpl->dbPath = "vrmusicstudio.db";
    pImpl->sslCertPath = "cert.pem";
    pImpl->sslKeyPath = "key.pem";
}

BackendService::~BackendService() {
    shutdown();
}

bool BackendService::initialize() {
    if (!initializeDatabase()) {
        spdlog::error("Datenbank konnte nicht initialisiert werden");
        return false;
    }
    if (!initializeSSL()) {
        spdlog::error("SSL konnte nicht initialisiert werden");
        return false;
    }
    pImpl->running = true;
    pImpl->changeThread = std::thread(&BackendService::processChangeQueue, this);
    return true;
}

void BackendService::shutdown() {
    if (pImpl->running) {
        pImpl->running = false;
        pImpl->queueCondition.notify_all();
        if (pImpl->changeThread.joinable()) {
            pImpl->changeThread.join();
        }
    }
    if (pImpl->db) {
        sqlite3_close(pImpl->db);
        pImpl->db = nullptr;
    }
    if (pImpl->sslContext) {
        SSL_CTX_free(pImpl->sslContext);
        pImpl->sslContext = nullptr;
    }
}

bool BackendService::initializeDatabase() {
    int rc = sqlite3_open(pImpl->dbPath.c_str(), &pImpl->db);
    if (rc != SQLITE_OK) {
        spdlog::error("Datenbank konnte nicht geöffnet werden: {}", sqlite3_errmsg(pImpl->db));
        return false;
    }
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL,
            email TEXT UNIQUE NOT NULL
        );
        CREATE TABLE IF NOT EXISTS projects (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            owner_id INTEGER NOT NULL,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (owner_id) REFERENCES users(id)
        );
        CREATE TABLE IF NOT EXISTS collaborations (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            project_id INTEGER NOT NULL,
            user_id INTEGER NOT NULL,
            role TEXT NOT NULL,
            FOREIGN KEY (project_id) REFERENCES projects(id),
            FOREIGN KEY (user_id) REFERENCES users(id)
        );
        CREATE TABLE IF NOT EXISTS files (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            project_id INTEGER NOT NULL,
            name TEXT NOT NULL,
            path TEXT NOT NULL,
            type TEXT NOT NULL,
            FOREIGN KEY (project_id) REFERENCES projects(id)
        );
        CREATE TABLE IF NOT EXISTS plugins (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            version TEXT NOT NULL,
            path TEXT NOT NULL
        );
        CREATE TABLE IF NOT EXISTS backups (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            project_id INTEGER NOT NULL,
            timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            path TEXT NOT NULL,
            FOREIGN KEY (project_id) REFERENCES projects(id)
        );
        CREATE TABLE IF NOT EXISTS versions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            project_id INTEGER NOT NULL,
            version_number INTEGER NOT NULL,
            timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            path TEXT NOT NULL,
            FOREIGN KEY (project_id) REFERENCES projects(id)
        );
    )";
    char* errMsg = nullptr;
    rc = sqlite3_exec(pImpl->db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        spdlog::error("SQL-Fehler: {}", errMsg);
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool BackendService::initializeSSL() {
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    pImpl->sslContext = SSL_CTX_new(TLS_server_method());
    if (!pImpl->sslContext) {
        spdlog::error("SSL-Kontext konnte nicht erstellt werden");
        return false;
    }
    if (SSL_CTX_use_certificate_file(pImpl->sslContext, pImpl->sslCertPath.c_str(), SSL_FILETYPE_PEM) <= 0) {
        spdlog::error("SSL-Zertifikat konnte nicht geladen werden");
        return false;
    }
    if (SSL_CTX_use_PrivateKey_file(pImpl->sslContext, pImpl->sslKeyPath.c_str(), SSL_FILETYPE_PEM) <= 0) {
        spdlog::error("SSL-Private-Key konnte nicht geladen werden");
        return false;
    }
    return true;
}

// User-Verwaltung
bool BackendService::registerUser(const std::string& username, const std::string& password, const std::string& email) {
    const char* sql = "INSERT INTO users (username, password, email) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(pImpl->db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("SQL-Fehler: {}", sqlite3_errmsg(pImpl->db));
        return false;
    }
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, email.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool BackendService::authenticateUser(const std::string& username, const std::string& password) {
    const char* sql = "SELECT password FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(pImpl->db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("SQL-Fehler: {}", sqlite3_errmsg(pImpl->db));
        return false;
    }
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        std::string storedPassword = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        sqlite3_finalize(stmt);
        return storedPassword == password;
    }
    sqlite3_finalize(stmt);
    return false;
}

// Projekt-Verwaltung
bool BackendService::createProject(const std::string& name, int ownerId) {
    const char* sql = "INSERT INTO projects (name, owner_id) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(pImpl->db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("SQL-Fehler: {}", sqlite3_errmsg(pImpl->db));
        return false;
    }
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, ownerId);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<BackendService::Project> BackendService::getProjects(int userId) {
    std::vector<Project> projects;
    const char* sql = "SELECT id, name, owner_id, created_at FROM projects WHERE owner_id = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(pImpl->db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("SQL-Fehler: {}", sqlite3_errmsg(pImpl->db));
        return projects;
    }
    sqlite3_bind_int(stmt, 1, userId);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Project project;
        project.id = sqlite3_column_int(stmt, 0);
        project.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        project.ownerId = sqlite3_column_int(stmt, 2);
        project.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        projects.push_back(project);
    }
    sqlite3_finalize(stmt);
    return projects;
}

// Kollaboration
bool BackendService::addCollaborator(int projectId, int userId, const std::string& role) {
    const char* sql = "INSERT INTO collaborations (project_id, user_id, role) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(pImpl->db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("SQL-Fehler: {}", sqlite3_errmsg(pImpl->db));
        return false;
    }
    sqlite3_bind_int(stmt, 1, projectId);
    sqlite3_bind_int(stmt, 2, userId);
    sqlite3_bind_text(stmt, 3, role.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<BackendService::Collaboration> BackendService::getCollaborations(int projectId) {
    std::vector<Collaboration> collaborations;
    const char* sql = "SELECT id, project_id, user_id, role FROM collaborations WHERE project_id = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(pImpl->db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("SQL-Fehler: {}", sqlite3_errmsg(pImpl->db));
        return collaborations;
    }
    sqlite3_bind_int(stmt, 1, projectId);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Collaboration collaboration;
        collaboration.id = sqlite3_column_int(stmt, 0);
        collaboration.projectId = sqlite3_column_int(stmt, 1);
        collaboration.userId = sqlite3_column_int(stmt, 2);
        collaboration.role = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        collaborations.push_back(collaboration);
    }
    sqlite3_finalize(stmt);
    return collaborations;
}

// Datei-Management
bool BackendService::uploadFile(int projectId, const std::string& name, const std::string& path, const std::string& type) {
    const char* sql = "INSERT INTO files (project_id, name, path, type) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(pImpl->db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("SQL-Fehler: {}", sqlite3_errmsg(pImpl->db));
        return false;
    }
    sqlite3_bind_int(stmt, 1, projectId);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, path.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, type.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<std::string> BackendService::getFiles(int projectId) {
    std::vector<std::string> files;
    const char* sql = "SELECT path FROM files WHERE project_id = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(pImpl->db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("SQL-Fehler: {}", sqlite3_errmsg(pImpl->db));
        return files;
    }
    sqlite3_bind_int(stmt, 1, projectId);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        files.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    }
    sqlite3_finalize(stmt);
    return files;
}

// Plugin-Management
bool BackendService::installPlugin(const std::string& name, const std::string& version, const std::string& path) {
    const char* sql = "INSERT INTO plugins (name, version, path) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(pImpl->db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("SQL-Fehler: {}", sqlite3_errmsg(pImpl->db));
        return false;
    }
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, version.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, path.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<std::string> BackendService::getPlugins() {
    std::vector<std::string> plugins;
    const char* sql = "SELECT path FROM plugins;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(pImpl->db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("SQL-Fehler: {}", sqlite3_errmsg(pImpl->db));
        return plugins;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        plugins.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    }
    sqlite3_finalize(stmt);
    return plugins;
}

// Backup & Versionierung
bool BackendService::createBackup(int projectId, const std::string& path) {
    const char* sql = "INSERT INTO backups (project_id, path) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(pImpl->db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("SQL-Fehler: {}", sqlite3_errmsg(pImpl->db));
        return false;
    }
    sqlite3_bind_int(stmt, 1, projectId);
    sqlite3_bind_text(stmt, 2, path.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<std::string> BackendService::getBackups(int projectId) {
    std::vector<std::string> backups;
    const char* sql = "SELECT path FROM backups WHERE project_id = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(pImpl->db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("SQL-Fehler: {}", sqlite3_errmsg(pImpl->db));
        return backups;
    }
    sqlite3_bind_int(stmt, 1, projectId);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        backups.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    }
    sqlite3_finalize(stmt);
    return backups;
}

bool BackendService::createVersion(int projectId, int versionNumber, const std::string& path) {
    const char* sql = "INSERT INTO versions (project_id, version_number, path) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(pImpl->db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("SQL-Fehler: {}", sqlite3_errmsg(pImpl->db));
        return false;
    }
    sqlite3_bind_int(stmt, 1, projectId);
    sqlite3_bind_int(stmt, 2, versionNumber);
    sqlite3_bind_text(stmt, 3, path.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<std::string> BackendService::getVersions(int projectId) {
    std::vector<std::string> versions;
    const char* sql = "SELECT path FROM versions WHERE project_id = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(pImpl->db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("SQL-Fehler: {}", sqlite3_errmsg(pImpl->db));
        return versions;
    }
    sqlite3_bind_int(stmt, 1, projectId);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        versions.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    }
    sqlite3_finalize(stmt);
    return versions;
}

// Realtime-Kollaboration
bool BackendService::startSession(int projectId) {
    std::string sessionId = "session_" + std::to_string(projectId);
    pImpl->activeSessions[projectId] = sessionId;
    return true;
}

bool BackendService::endSession(int projectId) {
    pImpl->activeSessions.erase(projectId);
    return true;
}

bool BackendService::broadcastChange(int projectId, const std::string& change) {
    if (pImpl->activeSessions.count(projectId) == 0) {
        return false;
    }
    broadcastChange(change);
    return true;
}

void BackendService::setChangeCallback(std::function<void(const std::string&)> callback) {
    pImpl->changeCallback = callback;
}

void BackendService::broadcastChange(const std::string& change) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->changeQueue.push([this, change]() {
        if (pImpl->changeCallback) {
            pImpl->changeCallback(change);
        }
    });
    pImpl->queueCondition.notify_one();
}

void BackendService::processChangeQueue() {
    while (pImpl->running) {
        std::unique_lock<std::mutex> lock(pImpl->queueMutex);
        pImpl->queueCondition.wait(lock, [this] { return !pImpl->changeQueue.empty() || !pImpl->running; });
        while (!pImpl->changeQueue.empty()) {
            auto task = pImpl->changeQueue.front();
            pImpl->changeQueue.pop();
            lock.unlock();
            task();
            lock.lock();
        }
    }
}

} // namespace VRMusicStudio 