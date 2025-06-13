#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <sqlite3.h>
#include <openssl/ssl.h>
#include <nlohmann/json.hpp>

namespace VRMusicStudio {

class BackendService {
public:
    struct Project {
        std::string id;
        std::string name;
        std::string owner;
        std::string created;
        std::string modified;
        nlohmann::json metadata;
    };

    struct User {
        std::string id;
        std::string username;
        std::string email;
        std::string role;
        bool isActive;
    };

    struct Collaboration {
        std::string projectId;
        std::string userId;
        std::string role;
        std::string permissions;
    };

    BackendService();
    ~BackendService();

    bool initialize();
    void shutdown();

    // Authentifizierung & Benutzerverwaltung
    bool authenticate(const std::string& username, const std::string& password);
    bool registerUser(const std::string& username, const std::string& email, const std::string& password);
    bool updateUser(const User& user);
    bool deleteUser(const std::string& userId);
    User getUser(const std::string& userId);

    // Projektverwaltung
    std::string createProject(const std::string& name, const std::string& owner);
    bool updateProject(const Project& project);
    bool deleteProject(const std::string& projectId);
    Project getProject(const std::string& projectId);
    std::vector<Project> getUserProjects(const std::string& userId);

    // Kollaboration
    bool addCollaborator(const std::string& projectId, const std::string& userId, const std::string& role);
    bool removeCollaborator(const std::string& projectId, const std::string& userId);
    bool updateCollaboratorRole(const std::string& projectId, const std::string& userId, const std::string& role);
    std::vector<Collaboration> getProjectCollaborators(const std::string& projectId);

    // Datei-Management
    bool uploadFile(const std::string& projectId, const std::string& filePath, const std::string& type);
    bool downloadFile(const std::string& fileId, const std::string& destination);
    bool deleteFile(const std::string& fileId);
    std::vector<std::string> getProjectFiles(const std::string& projectId);

    // Plugin-Management
    bool installPlugin(const std::string& pluginId, const std::string& version);
    bool uninstallPlugin(const std::string& pluginId);
    bool updatePlugin(const std::string& pluginId, const std::string& version);
    std::vector<std::string> getInstalledPlugins();
    std::vector<std::string> getAvailablePlugins();

    // Backup & Versionierung
    bool createBackup(const std::string& projectId);
    bool restoreBackup(const std::string& backupId);
    std::vector<std::string> getProjectBackups(const std::string& projectId);
    bool createVersion(const std::string& projectId, const std::string& description);
    bool restoreVersion(const std::string& versionId);

    // Echtzeit-Kollaboration
    bool joinSession(const std::string& projectId, const std::string& userId);
    bool leaveSession(const std::string& projectId, const std::string& userId);
    bool broadcastChange(const std::string& projectId, const nlohmann::json& change);
    void setChangeCallback(std::function<void(const std::string&, const nlohmann::json&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    bool initializeDatabase();
    bool initializeSSL();
    bool validateUser(const std::string& userId);
    bool validateProject(const std::string& projectId);
    bool validateCollaboration(const std::string& projectId, const std::string& userId);
    void handleRealtimeUpdates();
    void processChangeQueue();
};

} // namespace VRMusicStudio 