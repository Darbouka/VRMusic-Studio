#include "ResourceManager.hpp"
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <functional>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <spdlog/spdlog.h>
#include "../thirdparty/stb_image.h"
#include "../thirdparty/stb_image_write.h"

namespace VRMusicStudio {
namespace Core {

namespace {
    std::string generateResourceId(const std::string& path) {
        return std::filesystem::path(path).filename().string();
    }

    ResourceType determineResourceType(const std::string& path) {
        std::string ext = std::filesystem::path(path).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") return ResourceType::Image;
        if (ext == ".wav" || ext == ".mp3") return ResourceType::Audio;
        if (ext == ".obj" || ext == ".fbx") return ResourceType::Model;
        return ResourceType::Unknown;
    }

    std::vector<uint8_t> loadResourceData(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Konnte Datei nicht öffnen: " + path);
        }
        return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }
}

ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

ResourceManager::ResourceManager() 
    : initialized(false)
    , logger_(Logger::getInstance()) {
    stbi_set_flip_vertically_on_load(true);
}

ResourceManager::~ResourceManager() {
    shutdown();
}

bool ResourceManager::initialize() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (initialized) {
        return true;
    }

    try {
        resources.clear();
        initialized = true;
        logger_.info("Resource-Manager erfolgreich initialisiert");
        return true;
    } catch (const std::exception& e) {
        logger_.error("Fehler bei der Initialisierung des Resource-Managers: {}", e.what());
        return false;
    }
}

void ResourceManager::shutdown() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        unloadAllResources();
        initialized = false;
        logger_.info("Resource-Manager erfolgreich beendet");
    } catch (const std::exception& e) {
        logger_.error("Fehler beim Beenden des Resource-Managers: {}", e.what());
    }
}

void ResourceManager::update() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    // Hier können periodische Aktualisierungen der Ressourcen durchgeführt werden
    // z.B. Überprüfung auf Änderungen, Neuladen bei Bedarf, etc.
}

void ResourceManager::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    resources.clear();
}

bool ResourceManager::loadResource(const std::string& resourcePath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        throw ResourceException("Resource-Manager nicht initialisiert");
    }

    try {
        if (!validateResource(resourcePath)) {
            throw ResourceException("Ungültige Ressource: " + resourcePath);
        }

        std::string resourceId = generateResourceId(resourcePath);
        if (isResourceLoaded(resourceId)) {
            logger_.warn("Ressource bereits geladen: {}", resourceId);
            return true;
        }

        ResourceInfo info;
        info.path = resourcePath;
        info.type = determineResourceType(resourcePath);
        info.data = loadResourceData(resourcePath);
        info.isLoaded = true;
        info.size = std::filesystem::file_size(resourcePath);

        resources[resourceId] = info;
        logger_.info("Ressource erfolgreich geladen: {}", resourceId);
        return true;
    } catch (const std::exception& e) {
        logger_.error("Fehler beim Laden der Ressource: {}", e.what());
        return false;
    }
}

void ResourceManager::unloadResource(const std::string& resourceId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        auto it = resources.find(resourceId);
        if (it != resources.end()) {
            resources.erase(it);
            logger_.info("Ressource erfolgreich entladen: {}", resourceId);
        }
    } catch (const std::exception& e) {
        logger_.error("Fehler beim Entladen der Ressource '{}': {}", resourceId, e.what());
    }
}

void ResourceManager::unloadAllResources() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        resources.clear();
        logger_.info("Alle Ressourcen erfolgreich entladen");
    } catch (const std::exception& e) {
        logger_.error("Fehler beim Entladen aller Ressourcen: {}", e.what());
    }
}

bool ResourceManager::isResourceLoaded(const std::string& resourceId) const {
    std::lock_guard<std::mutex> lock(mutex);
    return resources.find(resourceId) != resources.end();
}

const ResourceInfo& ResourceManager::getResourceInfo(const std::string& resourceId) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = resources.find(resourceId);
    if (it == resources.end()) {
        throw ResourceException("Ressource nicht gefunden: " + resourceId);
    }
    return it->second;
}

bool ResourceManager::validateResource(const std::string& resourcePath) const {
    try {
        if (!std::filesystem::exists(resourcePath)) {
            return false;
        }

        auto extension = std::filesystem::path(resourcePath).extension().string();
        if (extension.empty()) {
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        logger_.error("Fehler bei der Ressourcen-Validierung: {}", e.what());
        return false;
    }
}

} // namespace Core
} // namespace VRMusicStudio 