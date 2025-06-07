#pragma once

#include "PluginInterface.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace VR_DAW {

class PluginManager {
public:
    static PluginManager& getInstance();

    // Plugin-Registrierung
    void registerPluginFactory(const std::string& pluginId, 
                             std::function<std::shared_ptr<PluginInterface>()> factory);
    void unregisterPluginFactory(const std::string& pluginId);

    // Plugin-Erstellung
    std::shared_ptr<PluginInterface> createPlugin(const std::string& pluginId);
    void destroyPlugin(const std::string& pluginId);

    // Plugin-Verwaltung
    std::vector<std::string> getAvailablePlugins() const;
    std::vector<std::string> getAvailablePluginsByType(PluginType type) const;
    std::shared_ptr<PluginInterface> getPlugin(const std::string& pluginId) const;

    // Plugin-Kategorien
    std::vector<std::string> getEffectPlugins() const;
    std::vector<std::string> getInstrumentPlugins() const;
    std::vector<std::string> getToolPlugins() const;
    std::vector<std::string> getSampleBankPlugins() const;
    std::vector<std::string> getMasteringPlugins() const;
    std::vector<std::string> getAutomationPlugins() const;

    // Plugin-Scanning
    void scanPluginDirectory(const std::string& directory);
    void rescanPlugins();

private:
    PluginManager();
    ~PluginManager();

    // Singleton-Instanz
    static std::unique_ptr<PluginManager> instance;

    // Plugin-Factories
    std::map<std::string, std::function<std::shared_ptr<PluginInterface>()>> pluginFactories;
    std::map<std::string, std::shared_ptr<PluginInterface>> activePlugins;
    std::map<PluginType, std::vector<std::string>> pluginsByType;

    // Thread-Sicherheit
    mutable std::mutex mutex;

    // Hilfsfunktionen
    void categorizePlugin(const std::string& pluginId, PluginType type);
    void loadPluginInfo(const std::string& pluginId);
};

} // namespace VR_DAW 