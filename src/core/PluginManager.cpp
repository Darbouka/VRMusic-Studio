#include "PluginManager.hpp"
#include "AudioPlugin.hpp"
#include "Logger.hpp"
#if __has_include(<filesystem>)
    #include <filesystem>
    namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    #error "No filesystem support"
#endif
#include <dlfcn.h>
#include <sstream>
#include <algorithm>

namespace VR_DAW {

namespace {
    std::string generatePluginId(const std::string& pluginPath) {
        return fs::path(pluginPath).filename().string();
    }

    bool validatePlugin(const std::string& pluginPath) {
        return fs::exists(pluginPath) && 
               fs::is_regular_file(pluginPath);
    }
}

PluginManager::PluginManager()
    : initialized(false)
    , logger_(Logger::getInstance()) {
}

PluginManager::~PluginManager() {
    shutdown();
}

bool PluginManager::initialize() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (initialized) {
        logger_.warn("Plugin-Manager bereits initialisiert");
        return true;
    }

    try {
        plugins.clear();
        initialized = true;
        logger_.info("Plugin-Manager erfolgreich initialisiert");
        return true;
    } catch (const std::exception& e) {
        logger_.error("Fehler bei der Initialisierung des Plugin-Managers: {}", e.what());
        return false;
    }
}

void PluginManager::shutdown() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        unloadAllPlugins();
        initialized = false;
        logger_.info("Plugin-Manager erfolgreich heruntergefahren");
    } catch (const std::exception& e) {
        logger_.error("Fehler beim Herunterfahren des Plugin-Managers: {}", e.what());
    }
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        throw PluginException("Plugin-Manager nicht initialisiert");
    }

    if (!validatePlugin(pluginPath)) {
        throw PluginException("Ungültiger Plugin-Pfad: " + pluginPath);
    }

    const std::string pluginId = generatePluginId(pluginPath);
    
    if (isPluginLoaded(pluginId)) {
        logger_.warn("Plugin {} bereits geladen", pluginId);
        return true;
    }

    try {
        auto plugin = std::make_shared<AudioPlugin>();
        if (!plugin->initialize()) {
            throw PluginException("Plugin konnte nicht initialisiert werden: " + pluginId);
        }

        plugins[pluginId] = plugin;
        logger_.info("Plugin {} erfolgreich geladen", pluginId);
        return true;
    } catch (const std::exception& e) {
        logger_.error("Fehler beim Laden des Plugins {}: {}", pluginId, e.what());
        return false;
    }
}

void PluginManager::unloadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        logger_.warn("Plugin {} nicht gefunden", pluginName);
        return;
    }

    try {
        it->second->shutdown();
        plugins.erase(it);
        logger_.info("Plugin {} erfolgreich entladen", pluginName);
    } catch (const std::exception& e) {
        logger_.error("Fehler beim Entladen des Plugins {}: {}", pluginName, e.what());
    }
}

bool PluginManager::isPluginLoaded(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(mutex);
    return plugins.find(pluginName) != plugins.end();
}

AudioPluginPtr PluginManager::getPlugin(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        throw PluginException("Plugin nicht gefunden: " + pluginName);
    }
    
    return it->second;
}

std::vector<std::string> PluginManager::getLoadedPluginNames() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<std::string> names;
    names.reserve(plugins.size());
    
    for (const auto& [name, _] : plugins) {
        names.push_back(name);
    }
    
    return names;
}

void PluginManager::unloadAllPlugins() {
    for (const auto& [name, _] : plugins) {
        unloadPlugin(name);
    }
}

void PluginManager::update() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    for (const auto& [name, plugin] : plugins) {
        try {
            plugin->update();
        } catch (const std::exception& e) {
            logger_.error("Fehler beim Update des Plugins {}: {}", name, e.what());
        }
    }
}

} // namespace VR_DAW 