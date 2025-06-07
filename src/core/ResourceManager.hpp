#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <any>
#include <variant>
#include <stdexcept>

namespace VR_DAW {

class ResourceException : public std::runtime_error {
public:
    explicit ResourceException(const std::string& message) : std::runtime_error(message) {}
};

class ResourceLoadException : public ResourceException {
public:
    explicit ResourceLoadException(const std::string& message) : ResourceException(message) {}
};

class ResourceTypeException : public ResourceException {
public:
    explicit ResourceTypeException(const std::string& message) : ResourceException(message) {}
};

struct AudioResource {
    std::vector<float> samples;
    int sampleRate;
    int channels;
    std::string format;
};

struct ImageResource {
    std::vector<unsigned char> data;
    int width;
    int height;
    int channels;
    std::string format;
};

struct TextResource {
    std::string content;
    std::string encoding;
};

struct BinaryResource {
    std::vector<unsigned char> data;
    std::string type;
};

using ResourceData = std::variant<
    AudioResource,
    ImageResource,
    TextResource,
    BinaryResource
>;

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    // Resource-Manager initialisieren/beenden
    bool initialize();
    void shutdown();

    // Ressourcen laden/entladen
    bool loadResource(const std::string& resourcePath);
    void unloadResource(const std::string& resourceId);
    void unloadAllResources();

    // Ressourcen-Informationen abrufen
    std::vector<std::string> getLoadedResources() const;
    ResourceData getResource(const std::string& resourceId) const;
    bool isResourceLoaded(const std::string& resourceId) const;

    // Resource-Manager aktualisieren
    void update();

private:
    struct ResourceInfo {
        ResourceData data;
        std::string path;
        std::string type;
        size_t size;
        bool isLoaded;
    };

    std::unordered_map<std::string, ResourceInfo> resources;
    mutable std::mutex mutex;
    bool initialized;

    bool validateResource(const std::string& resourcePath) const;
    std::string generateResourceId(const std::string& resourcePath) const;
    ResourceData loadResourceData(const std::string& resourcePath) const;
    void cleanupResource(ResourceInfo& info);
    std::string determineResourceType(const std::string& resourcePath) const;
};

} // namespace VR_DAW 