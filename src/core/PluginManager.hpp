#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <stdexcept>

namespace VR_DAW {

class PluginException : public std::runtime_error {
public:
    explicit PluginException(const std::string& message) : std::runtime_error(message) {}
};

class PluginLoadException : public PluginException {
public:
    explicit PluginLoadException(const std::string& message) : PluginException(message) {}
};

class PluginInitException : public PluginException {
public:
    explicit PluginInitException(const std::string& message) : PluginException(message) {}
};

class AudioPlugin;

class PluginManager {
public:
    PluginManager();
    ~PluginManager();

    // Plugin-Manager initialisieren/beenden
    bool initialize();
    void shutdown();

    // Plugins laden/entladen
    bool loadPlugin(const std::string& pluginPath);
    void unloadPlugin(const std::string& pluginId);
    void unloadAllPlugins();

    // Plugin-Informationen abrufen
    std::vector<std::string> getLoadedPlugins() const;
    std::shared_ptr<AudioPlugin> getPlugin(const std::string& pluginId) const;
    bool isPluginLoaded(const std::string& pluginId) const;

    // Plugin-Manager aktualisieren
    void update();

private:
    struct PluginInfo {
        std::shared_ptr<AudioPlugin> plugin;
        void* handle;
        std::string path;
        std::string version;
        bool isInitialized;
    };

    std::unordered_map<std::string, PluginInfo> plugins;
    mutable std::mutex mutex;
    bool initialized;

    bool validatePlugin(const std::string& pluginPath) const;
    std::string generatePluginId(const std::string& pluginPath) const;
    void cleanupPlugin(PluginInfo& info);
    bool checkPluginCompatibility(const std::string& pluginPath) const;
};

} // namespace VR_DAW 