#include "PluginInterface.hpp"
#include <dlfcn.h>
#include <filesystem>
#include <stdexcept>

namespace VR_DAW {

namespace {
    std::unordered_map<std::string, void*> loadedLibraries;
    std::unordered_map<std::string, std::shared_ptr<PluginInterface>> loadedPlugins;
    Logger logger("PluginLoader");
}

std::shared_ptr<PluginInterface> PluginLoader::loadPlugin(const std::string& pluginPath) {
    try {
        // Überprüfen, ob das Plugin bereits geladen ist
        if (isPluginLoaded(pluginPath)) {
            logger.warn("Plugin bereits geladen: {}", pluginPath);
            return loadedPlugins[pluginPath];
        }

        // Bibliothek laden
        void* library = dlopen(pluginPath.c_str(), RTLD_LAZY);
        if (!library) {
            logger.error("Fehler beim Laden der Plugin-Bibliothek: {}", dlerror());
            return nullptr;
        }

        // Factory-Funktion abrufen
        using CreatePluginFactory = PluginFactory* (*)();
        auto createFactory = reinterpret_cast<CreatePluginFactory>(dlsym(library, "createPluginFactory"));
        if (!createFactory) {
            logger.error("Fehler beim Abrufen der Factory-Funktion: {}", dlerror());
            dlclose(library);
            return nullptr;
        }

        // Plugin erstellen
        auto factory = std::unique_ptr<PluginFactory>(createFactory());
        if (!factory) {
            logger.error("Fehler beim Erstellen der Plugin-Factory");
            dlclose(library);
            return nullptr;
        }

        // Plugin initialisieren
        auto plugin = factory->createPlugin();
        if (!plugin || !plugin->initialize()) {
            logger.error("Fehler bei der Plugin-Initialisierung");
            dlclose(library);
            return nullptr;
        }

        // Plugin-Info abrufen
        auto info = plugin->getInfo();
        logger.info("Plugin geladen: {} (Version {})", info.name, info.version);

        // Plugin speichern
        loadedLibraries[pluginPath] = library;
        loadedPlugins[pluginPath] = plugin;

        return plugin;
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Laden des Plugins: {}", e.what());
        return nullptr;
    }
}

void PluginLoader::unloadPlugin(const std::string& pluginId) {
    try {
        auto it = loadedPlugins.find(pluginId);
        if (it == loadedPlugins.end()) {
            logger.warn("Plugin nicht gefunden: {}", pluginId);
            return;
        }

        // Plugin beenden
        it->second->shutdown();
        loadedPlugins.erase(it);

        // Bibliothek entladen
        auto libIt = loadedLibraries.find(pluginId);
        if (libIt != loadedLibraries.end()) {
            dlclose(libIt->second);
            loadedLibraries.erase(libIt);
        }

        logger.info("Plugin entladen: {}", pluginId);
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Entladen des Plugins: {}", e.what());
    }
}

std::vector<PluginInfo> PluginLoader::getLoadedPlugins() {
    std::vector<PluginInfo> plugins;
    plugins.reserve(loadedPlugins.size());

    for (const auto& [id, plugin] : loadedPlugins) {
        plugins.push_back(plugin->getInfo());
    }

    return plugins;
}

bool PluginLoader::isPluginLoaded(const std::string& pluginId) {
    return loadedPlugins.find(pluginId) != loadedPlugins.end();
}

} // namespace VR_DAW 