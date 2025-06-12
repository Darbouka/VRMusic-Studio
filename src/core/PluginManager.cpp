#include "PluginManager.hpp"
#include "AudioPlugin.hpp"
#include "Logger.hpp"
<<<<<<< HEAD
#include <filesystem>
=======
#if __has_include(<filesystem>)
    #include <filesystem>
    namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    #error "No filesystem support"
#endif
>>>>>>> 0dff1c4 (init 2)
#include <dlfcn.h>
#include <sstream>

namespace VR_DAW {

PluginManager::PluginManager()
    : initialized(false) {
}

PluginManager::~PluginManager() {
    shutdown();
}

bool PluginManager::initialize() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (initialized) {
        return true;
    }

    try {
        plugins.clear();
        initialized = true;
        Logger("PluginManager").info("Plugin-Manager erfolgreich initialisiert");
        return true;
    }
    catch (const std::exception& e) {
        Logger("PluginManager").error("Fehler bei der Initialisierung des Plugin-Managers: {}", e.what());
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
        Logger("PluginManager").info("Plugin-Manager erfolgreich beendet");
    }
    catch (const std::exception& e) {
        Logger("PluginManager").error("Fehler beim Beenden des Plugin-Managers: {}", e.what());
    }
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        throw PluginException("Plugin-Manager nicht initialisiert");
    }

    try {
        if (!validatePlugin(pluginPath)) {
            throw PluginLoadException("Ungültiges Plugin: " + pluginPath);
        }

        if (!checkPluginCompatibility(pluginPath)) {
            throw PluginLoadException("Inkompatibles Plugin: " + pluginPath);
        }

        std::string pluginId = generatePluginId(pluginPath);
        if (isPluginLoaded(pluginId)) {
            Logger("PluginManager").warn("Plugin bereits geladen: {}", pluginId);
            return true;
        }

        void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
        if (!handle) {
            throw PluginLoadException("Fehler beim Laden des Plugins: " + std::string(dlerror()));
        }

        using CreatePluginFunc = AudioPlugin* (*)();
        auto createPlugin = reinterpret_cast<CreatePluginFunc>(dlsym(handle, "createPlugin"));
        if (!createPlugin) {
            dlclose(handle);
            throw PluginLoadException("Fehler beim Laden der Plugin-Funktion: " + std::string(dlerror()));
        }

        auto plugin = std::shared_ptr<AudioPlugin>(createPlugin());
        if (!plugin) {
            dlclose(handle);
            throw PluginInitException("Fehler beim Erstellen des Plugins");
        }

        if (!plugin->initialize()) {
            dlclose(handle);
            throw PluginInitException("Fehler bei der Initialisierung des Plugins");
        }

        PluginInfo info;
        info.plugin = plugin;
        info.handle = handle;
        info.path = pluginPath;
        info.isInitialized = true;

        plugins[pluginId] = info;
        Logger("PluginManager").info("Plugin erfolgreich geladen: {}", pluginId);
        return true;
    }
    catch (const PluginException& e) {
        Logger("PluginManager").error("Plugin-Fehler: {}", e.what());
        return false;
    }
    catch (const std::exception& e) {
        Logger("PluginManager").error("Unerwarteter Fehler beim Laden des Plugins: {}", e.what());
        return false;
    }
}

void PluginManager::unloadPlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        auto it = plugins.find(pluginId);
        if (it != plugins.end()) {
            cleanupPlugin(it->second);
            plugins.erase(it);
            Logger("PluginManager").info("Plugin erfolgreich entladen: {}", pluginId);
        }
    }
    catch (const std::exception& e) {
        Logger("PluginManager").error("Fehler beim Entladen des Plugins '{}': {}", pluginId, e.what());
    }
}

void PluginManager::unloadAllPlugins() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        for (auto& [pluginId, info] : plugins) {
            cleanupPlugin(info);
        }
        plugins.clear();
        Logger("PluginManager").info("Alle Plugins erfolgreich entladen");
    }
    catch (const std::exception& e) {
        Logger("PluginManager").error("Fehler beim Entladen aller Plugins: {}", e.what());
    }
}

std::vector<std::string> PluginManager::getLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<std::string> pluginIds;
    pluginIds.reserve(plugins.size());
    
    for (const auto& [pluginId, _] : plugins) {
        pluginIds.push_back(pluginId);
    }
    
    return pluginIds;
}

std::shared_ptr<AudioPlugin> PluginManager::getPlugin(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = plugins.find(pluginId);
    if (it != plugins.end()) {
        return it->second.plugin;
    }
    return nullptr;
}

bool PluginManager::isPluginLoaded(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex);
    return plugins.find(pluginId) != plugins.end();
}

void PluginManager::update() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        for (auto& [pluginId, info] : plugins) {
            if (info.isInitialized && info.plugin) {
                info.plugin->update();
            }
        }
    }
    catch (const std::exception& e) {
        Logger("PluginManager").error("Fehler beim Aktualisieren der Plugins: {}", e.what());
    }
}

bool PluginManager::validatePlugin(const std::string& pluginPath) const {
    try {
<<<<<<< HEAD
        if (!std::filesystem::exists(pluginPath)) {
            return false;
        }

        auto extension = std::filesystem::path(pluginPath).extension().string();
=======
        if (!fs::exists(pluginPath)) {
            return false;
        }

        auto extension = fs::path(pluginPath).extension().string();
>>>>>>> 0dff1c4 (init 2)
        if (extension != ".so" && extension != ".dylib" && extension != ".dll") {
            return false;
        }

        return true;
    }
    catch (const std::exception& e) {
        Logger("PluginManager").error("Fehler bei der Plugin-Validierung: {}", e.what());
        return false;
    }
}

std::string PluginManager::generatePluginId(const std::string& pluginPath) const {
    try {
<<<<<<< HEAD
        return std::filesystem::path(pluginPath).stem().string();
=======
        return fs::path(pluginPath).stem().string();
>>>>>>> 0dff1c4 (init 2)
    }
    catch (const std::exception& e) {
        Logger("PluginManager").error("Fehler bei der Plugin-ID-Generierung: {}", e.what());
        return "";
    }
}

void PluginManager::cleanupPlugin(PluginInfo& info) {
    if (info.isInitialized && info.plugin) {
        try {
            info.plugin->shutdown();
        }
        catch (const std::exception& e) {
            Logger("PluginManager").error("Fehler beim Beenden des Plugins: {}", e.what());
        }
    }

    if (info.handle) {
        try {
            dlclose(info.handle);
        }
        catch (const std::exception& e) {
            Logger("PluginManager").error("Fehler beim Schließen des Plugin-Handles: {}", e.what());
        }
    }

    info.plugin.reset();
    info.handle = nullptr;
    info.isInitialized = false;
}

bool PluginManager::checkPluginCompatibility(const std::string& pluginPath) const {
<<<<<<< HEAD
=======
    (void)pluginPath;
>>>>>>> 0dff1c4 (init 2)
    // Hier können spezifische Kompatibilitätsprüfungen implementiert werden
    // z.B. Überprüfung der Plugin-Version, API-Version, etc.
    return true;
}

} // namespace VR_DAW 