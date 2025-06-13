#pragma once

#include "AudioPlugin.hpp"
#include "Logger.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <stdexcept>

namespace VRMusicStudio {
namespace Core {

class PluginException : public std::runtime_error {
public:
    explicit PluginException(const std::string& message) 
        : std::runtime_error(message) {}
};

class PluginManager {
public:
    static PluginManager& getInstance();

    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    PluginManager(PluginManager&&) = delete;
    PluginManager& operator=(PluginManager&&) = delete;

    bool initialize();
    void shutdown();
    void update();

    bool loadPlugin(const std::string& pluginPath);
    void unloadPlugin(const std::string& pluginName);
    bool isPluginLoaded(const std::string& pluginName) const;
    AudioPluginPtr getPlugin(const std::string& pluginName) const;
    std::vector<std::string> getLoadedPluginNames() const;

private:
    PluginManager();
    ~PluginManager();

    void unloadAllPlugins();
    bool validatePlugin(const std::string& pluginPath) const;
    std::string generatePluginId(const std::string& pluginPath) const;

    mutable std::mutex mutex;
    bool initialized;
    std::unordered_map<std::string, AudioPluginPtr> plugins;
    Logger& logger_;
};

} // namespace Core
} // namespace VRMusicStudio 