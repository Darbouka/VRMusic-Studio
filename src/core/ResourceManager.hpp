#pragma once

#include "Logger.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <vector>

namespace VRMusicStudio {
namespace Core {

enum class ResourceType {
    Unknown,
    Image,
    Audio,
    Model
};

struct ResourceInfo {
    std::string path;
    ResourceType type;
    std::vector<uint8_t> data;
    bool isLoaded;
    size_t size;
};

class ResourceException : public std::runtime_error {
public:
    explicit ResourceException(const std::string& message) 
        : std::runtime_error(message) {}
};

class ResourceManager {
public:
    static ResourceManager& getInstance();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    bool initialize();
    void shutdown();
    void update();
    bool loadResource(const std::string& resourcePath);
    void unloadResource(const std::string& resourceName);
    bool isResourceLoaded(const std::string& resourceId) const;
    const ResourceInfo& getResourceInfo(const std::string& resourceId) const;

private:
    ResourceManager();
    ~ResourceManager();

    bool validateResource(const std::string& resourcePath) const;
    void unloadAllResources();
    void clear();

    mutable std::mutex mutex;
    bool initialized;
    std::unordered_map<std::string, ResourceInfo> resources;
    Logger& logger_;
};

} // namespace Core
} // namespace VRMusicStudio 