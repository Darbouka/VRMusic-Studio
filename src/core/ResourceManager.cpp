#include "ResourceManager.hpp"
#include "Logger.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stb_image.h>
#include <sndfile.h>

namespace VR_DAW {

ResourceManager::ResourceManager()
    : initialized(false) {
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
        Logger("ResourceManager").info("Resource-Manager erfolgreich initialisiert");
        return true;
    }
    catch (const std::exception& e) {
        Logger("ResourceManager").error("Fehler bei der Initialisierung des Resource-Managers: {}", e.what());
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
        Logger("ResourceManager").info("Resource-Manager erfolgreich beendet");
    }
    catch (const std::exception& e) {
        Logger("ResourceManager").error("Fehler beim Beenden des Resource-Managers: {}", e.what());
    }
}

bool ResourceManager::loadResource(const std::string& resourcePath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        throw ResourceException("Resource-Manager nicht initialisiert");
    }

    try {
        if (!validateResource(resourcePath)) {
            throw ResourceLoadException("Ungültige Ressource: " + resourcePath);
        }

        std::string resourceId = generateResourceId(resourcePath);
        if (isResourceLoaded(resourceId)) {
            Logger("ResourceManager").warn("Ressource bereits geladen: {}", resourceId);
            return true;
        }

        ResourceInfo info;
        info.path = resourcePath;
        info.type = determineResourceType(resourcePath);
        info.data = loadResourceData(resourcePath);
        info.isLoaded = true;
        info.size = std::filesystem::file_size(resourcePath);

        resources[resourceId] = info;
        Logger("ResourceManager").info("Ressource erfolgreich geladen: {}", resourceId);
        return true;
    }
    catch (const ResourceException& e) {
        Logger("ResourceManager").error("Ressourcen-Fehler: {}", e.what());
        return false;
    }
    catch (const std::exception& e) {
        Logger("ResourceManager").error("Unerwarteter Fehler beim Laden der Ressource: {}", e.what());
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
            cleanupResource(it->second);
            resources.erase(it);
            Logger("ResourceManager").info("Ressource erfolgreich entladen: {}", resourceId);
        }
    }
    catch (const std::exception& e) {
        Logger("ResourceManager").error("Fehler beim Entladen der Ressource '{}': {}", resourceId, e.what());
    }
}

void ResourceManager::unloadAllResources() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        for (auto& [resourceId, info] : resources) {
            cleanupResource(info);
        }
        resources.clear();
        Logger("ResourceManager").info("Alle Ressourcen erfolgreich entladen");
    }
    catch (const std::exception& e) {
        Logger("ResourceManager").error("Fehler beim Entladen aller Ressourcen: {}", e.what());
    }
}

std::vector<std::string> ResourceManager::getLoadedResources() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<std::string> resourceIds;
    resourceIds.reserve(resources.size());
    
    for (const auto& [resourceId, _] : resources) {
        resourceIds.push_back(resourceId);
    }
    
    return resourceIds;
}

ResourceData ResourceManager::getResource(const std::string& resourceId) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = resources.find(resourceId);
    if (it != resources.end() && it->second.isLoaded) {
        return it->second.data;
    }
    throw ResourceException("Ressource nicht gefunden oder nicht geladen: " + resourceId);
}

bool ResourceManager::isResourceLoaded(const std::string& resourceId) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = resources.find(resourceId);
    return it != resources.end() && it->second.isLoaded;
}

void ResourceManager::update() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    // Hier können periodische Aktualisierungen der Ressourcen durchgeführt werden
    // z.B. Überprüfung auf Änderungen, Neuladen bei Bedarf, etc.
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
    }
    catch (const std::exception& e) {
        Logger("ResourceManager").error("Fehler bei der Ressourcen-Validierung: {}", e.what());
        return false;
    }
}

std::string ResourceManager::generateResourceId(const std::string& resourcePath) const {
    try {
        return std::filesystem::path(resourcePath).stem().string();
    }
    catch (const std::exception& e) {
        Logger("ResourceManager").error("Fehler bei der Ressourcen-ID-Generierung: {}", e.what());
        return "";
    }
}

ResourceData ResourceManager::loadResourceData(const std::string& resourcePath) const {
    try {
        std::string extension = std::filesystem::path(resourcePath).extension().string();
        
        if (extension == ".wav" || extension == ".ogg" || extension == ".flac") {
            SF_INFO fileInfo;
            SNDFILE* file = sf_open(resourcePath.c_str(), SFM_READ, &fileInfo);
            if (!file) {
                throw ResourceLoadException("Fehler beim Öffnen der Audio-Datei: " + std::string(sf_strerror(file)));
            }

            std::vector<float> samples(fileInfo.frames * fileInfo.channels);
            sf_readf_float(file, samples.data(), fileInfo.frames);
            sf_close(file);

            AudioResource audio;
            audio.samples = std::move(samples);
            audio.sampleRate = fileInfo.samplerate;
            audio.channels = fileInfo.channels;
            audio.format = extension.substr(1);
            return audio;
        }
        else if (extension == ".png" || extension == ".jpg" || extension == ".jpeg") {
            int width, height, channels;
            unsigned char* data = stbi_load(resourcePath.c_str(), &width, &height, &channels, 0);
            if (!data) {
                throw ResourceLoadException("Fehler beim Laden des Bildes: " + std::string(stbi_failure_reason()));
            }

            ImageResource image;
            image.data = std::vector<unsigned char>(data, data + width * height * channels);
            image.width = width;
            image.height = height;
            image.channels = channels;
            image.format = extension.substr(1);
            stbi_image_free(data);
            return image;
        }
        else if (extension == ".txt" || extension == ".json") {
            std::ifstream file(resourcePath);
            if (!file.is_open()) {
                throw ResourceLoadException("Fehler beim Öffnen der Text-Datei");
            }

            std::stringstream buffer;
            buffer << file.rdbuf();

            TextResource text;
            text.content = buffer.str();
            text.encoding = "UTF-8";
            return text;
        }
        else {
            std::ifstream file(resourcePath, std::ios::binary);
            if (!file.is_open()) {
                throw ResourceLoadException("Fehler beim Öffnen der Binär-Datei");
            }

            BinaryResource binary;
            binary.data = std::vector<unsigned char>(
                std::istreambuf_iterator<char>(file),
                std::istreambuf_iterator<char>()
            );
            binary.type = extension.substr(1);
            return binary;
        }
    }
    catch (const std::exception& e) {
        Logger("ResourceManager").error("Fehler beim Laden der Ressourcendaten: {}", e.what());
        throw ResourceLoadException(e.what());
    }
}

void ResourceManager::cleanupResource(ResourceInfo& info) {
    if (info.isLoaded) {
        info.data = ResourceData();
        info.isLoaded = false;
    }
}

std::string ResourceManager::determineResourceType(const std::string& resourcePath) const {
    auto extension = std::filesystem::path(resourcePath).extension().string();
    
    if (extension == ".wav" || extension == ".ogg" || extension == ".flac") {
        return "audio";
    }
    else if (extension == ".png" || extension == ".jpg" || extension == ".jpeg") {
        return "image";
    }
    else if (extension == ".txt" || extension == ".json") {
        return "text";
    }
    else {
        return "binary";
    }
}

} // namespace VR_DAW 