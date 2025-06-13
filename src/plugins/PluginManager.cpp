#include "plugins/PluginManager.hpp"
#include "core/Logger.hpp"
#include <filesystem>
#include <dlfcn.h>

namespace VRMusicStudio {
namespace Plugins {

PluginManager::PluginManager()
    : m_isInitialized(false)
{
}

PluginManager::~PluginManager()
{
    if (m_isInitialized) {
        shutdown();
    }
}

bool PluginManager::initialize()
{
    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Initialisiere Plugin-Manager...");

        // Plugin-Verzeichnis erstellen
        std::filesystem::create_directories("plugins");

        m_isInitialized = true;
        logger.info("Plugin-Manager erfolgreich initialisiert");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Plugin-Manager-Initialisierung: {}", e.what());
        return false;
    }
}

void PluginManager::shutdown()
{
    if (!m_isInitialized) {
        return;
    }

    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Beende Plugin-Manager...");

        // Alle Plugins entladen
        for (auto& plugin : m_plugins) {
            unloadPlugin(plugin.first);
        }
        m_plugins.clear();

        m_isInitialized = false;
        logger.info("Plugin-Manager erfolgreich beendet");
    } catch (const std::exception& e) {
        spdlog::error("Fehler beim Beenden des Plugin-Managers: {}", e.what());
    }
}

bool PluginManager::loadPlugin(const std::string& path)
{
    if (!m_isInitialized) {
        return false;
    }

    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Lade Plugin: {}", path);

        // Plugin bereits geladen?
        if (m_plugins.find(path) != m_plugins.end()) {
            logger.warn("Plugin bereits geladen: {}", path);
            return false;
        }

        // Plugin laden
        void* handle = dlopen(path.c_str(), RTLD_LAZY);
        if (!handle) {
            logger.error("Fehler beim Laden des Plugins: {}", dlerror());
            return false;
        }

        // Plugin-Interface abrufen
        auto createPlugin = reinterpret_cast<CreatePluginFunc>(dlsym(handle, "createPlugin"));
        if (!createPlugin) {
            logger.error("Fehler beim Abrufen der Plugin-Funktion: {}", dlerror());
            dlclose(handle);
            return false;
        }

        // Plugin-Instanz erstellen
        auto plugin = createPlugin();
        if (!plugin) {
            logger.error("Fehler beim Erstellen der Plugin-Instanz");
            dlclose(handle);
            return false;
        }

        // Plugin initialisieren
        if (!plugin->initialize()) {
            logger.error("Fehler bei der Plugin-Initialisierung");
            delete plugin;
            dlclose(handle);
            return false;
        }

        // Plugin speichern
        m_plugins[path] = {handle, plugin};
        logger.info("Plugin erfolgreich geladen: {}", path);
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Fehler beim Laden des Plugins: {}", e.what());
        return false;
    }
}

void PluginManager::unloadPlugin(const std::string& path)
{
    if (!m_isInitialized) {
        return;
    }

    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Entlade Plugin: {}", path);

        auto it = m_plugins.find(path);
        if (it == m_plugins.end()) {
            logger.warn("Plugin nicht gefunden: {}", path);
            return;
        }

        // Plugin beenden
        it->second.plugin->shutdown();
        delete it->second.plugin;

        // Plugin entladen
        dlclose(it->second.handle);
        m_plugins.erase(it);

        logger.info("Plugin erfolgreich entladen: {}", path);
    } catch (const std::exception& e) {
        spdlog::error("Fehler beim Entladen des Plugins: {}", e.what());
    }
}

void PluginManager::update()
{
    if (!m_isInitialized) {
        return;
    }

    // Alle Plugins aktualisieren
    for (auto& plugin : m_plugins) {
        plugin.second.plugin->update();
    }
}

std::vector<std::string> PluginManager::getLoadedPlugins() const
{
    std::vector<std::string> plugins;
    for (const auto& plugin : m_plugins) {
        plugins.push_back(plugin.first);
    }
    return plugins;
}

Plugin* PluginManager::getPlugin(const std::string& path)
{
    auto it = m_plugins.find(path);
    return it != m_plugins.end() ? it->second.plugin : nullptr;
}

} // namespace Plugins
} // namespace VRMusicStudio
