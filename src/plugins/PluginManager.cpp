#include "PluginManager.hpp"
#include <spdlog/spdlog.h>
#include <filesystem>

namespace VR_DAW {

std::unique_ptr<PluginManager> PluginManager::instance = nullptr;

PluginManager& PluginManager::getInstance() {
    if (!instance) {
        instance = std::unique_ptr<PluginManager>(new PluginManager());
    }
    return *instance;
}

PluginManager::PluginManager() {
}

PluginManager::~PluginManager() {
    activePlugins.clear();
    pluginFactories.clear();
    pluginsByType.clear();
}

void PluginManager::registerPluginFactory(const std::string& pluginId, 
                                        std::function<std::shared_ptr<PluginInterface>()> factory) {
    std::lock_guard<std::mutex> lock(mutex);
    pluginFactories[pluginId] = factory;
    spdlog::info("Plugin-Factory registriert: {}", pluginId);
}

void PluginManager::unregisterPluginFactory(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex);
    pluginFactories.erase(pluginId);
    spdlog::info("Plugin-Factory deregistriert: {}", pluginId);
}

std::shared_ptr<PluginInterface> PluginManager::createPlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = pluginFactories.find(pluginId);
    if (it == pluginFactories.end()) {
        spdlog::error("Plugin-Factory nicht gefunden: {}", pluginId);
        return nullptr;
    }

    auto plugin = it->second();
    if (!plugin) {
        spdlog::error("Konnte Plugin nicht erstellen: {}", pluginId);
        return nullptr;
    }

    if (!plugin->initialize()) {
        spdlog::error("Konnte Plugin nicht initialisieren: {}", pluginId);
        return nullptr;
    }

    activePlugins[pluginId] = plugin;
    categorizePlugin(pluginId, plugin->getType());
    spdlog::info("Plugin erstellt: {}", pluginId);
    return plugin;
}

void PluginManager::destroyPlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = activePlugins.find(pluginId);
    if (it != activePlugins.end()) {
        it->second->shutdown();
        activePlugins.erase(it);
        spdlog::info("Plugin zerstört: {}", pluginId);
    }
}

std::vector<std::string> PluginManager::getAvailablePlugins() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> plugins;
    for (const auto& [id, _] : pluginFactories) {
        plugins.push_back(id);
    }
    return plugins;
}

std::vector<std::string> PluginManager::getAvailablePluginsByType(PluginType type) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = pluginsByType.find(type);
    return it != pluginsByType.end() ? it->second : std::vector<std::string>();
}

std::shared_ptr<PluginInterface> PluginManager::getPlugin(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = activePlugins.find(pluginId);
    return it != activePlugins.end() ? it->second : nullptr;
}

std::vector<std::string> PluginManager::getEffectPlugins() const {
    return getAvailablePluginsByType(PluginType::Effect);
}

std::vector<std::string> PluginManager::getInstrumentPlugins() const {
    return getAvailablePluginsByType(PluginType::Instrument);
}

std::vector<std::string> PluginManager::getToolPlugins() const {
    return getAvailablePluginsByType(PluginType::Tool);
}

std::vector<std::string> PluginManager::getSampleBankPlugins() const {
    return getAvailablePluginsByType(PluginType::SampleBank);
}

std::vector<std::string> PluginManager::getMasteringPlugins() const {
    return getAvailablePluginsByType(PluginType::Mastering);
}

std::vector<std::string> PluginManager::getAutomationPlugins() const {
    return getAvailablePluginsByType(PluginType::Automation);
}

void PluginManager::scanPluginDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(mutex);
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".dll") {
                // Hier würde die Plugin-DLL geladen und registriert werden
                spdlog::info("Plugin gefunden: {}", entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler beim Scannen des Plugin-Verzeichnisses: {}", e.what());
    }
}

void PluginManager::rescanPlugins() {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Aktive Plugins beenden
    for (auto& [id, plugin] : activePlugins) {
        plugin->shutdown();
    }
    activePlugins.clear();

    // Factories und Kategorien zurücksetzen
    pluginFactories.clear();
    pluginsByType.clear();

    // Plugins neu scannen
    scanPluginDirectory("plugins");
}

void PluginManager::categorizePlugin(const std::string& pluginId, PluginType type) {
    pluginsByType[type].push_back(pluginId);
}

void PluginManager::loadPluginInfo(const std::string& pluginId) {
    // Hier würden zusätzliche Plugin-Informationen geladen werden
    // z.B. aus einer Konfigurationsdatei oder der Plugin-DLL
}

} // namespace VR_DAW 