#pragma once

#include "PluginInterface.hpp"
<<<<<<< HEAD
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
=======
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>

namespace VRMusicStudio {

class PluginManager {
public:
    PluginManager();
    ~PluginManager();

    // Plugin-Management
    bool loadPlugin(const std::string& path);
    bool unloadPlugin(const std::string& pluginId);
    std::shared_ptr<PluginInterface> createPluginInstance(const std::string& pluginId);
    void destroyPluginInstance(const std::string& instanceId);
    std::vector<std::string> getAvailablePlugins() const;
    std::vector<std::string> getPluginInstances() const;

    // Plugin-Kategorisierung
    std::vector<std::string> getPluginsByType(PluginType type) const;
    std::vector<std::string> getPluginsByCategory(const std::string& category) const;
    std::vector<std::string> getPluginsByVendor(const std::string& vendor) const;

    // Plugin-Informationen
    std::string getPluginName(const std::string& pluginId) const;
    std::string getPluginVendor(const std::string& pluginId) const;
    std::string getPluginCategory(const std::string& pluginId) const;
    std::string getPluginVersion(const std::string& pluginId) const;
    PluginType getPluginType(const std::string& pluginId) const;
    std::vector<PluginParameter> getPluginParameters(const std::string& pluginId) const;

    // Plugin-Instanz-Management
    std::shared_ptr<PluginInterface> getPluginInstance(const std::string& instanceId) const;
    void setPluginParameter(const std::string& instanceId, const std::string& parameter, float value);
    float getPluginParameter(const std::string& instanceId, const std::string& parameter) const;
    void setPluginParameterAutomated(const std::string& instanceId, const std::string& parameter, bool automated);
    bool isPluginParameterAutomated(const std::string& instanceId, const std::string& parameter) const;

    // Plugin-Preset-Management
    void loadPluginPreset(const std::string& instanceId, const std::string& presetName);
    void savePluginPreset(const std::string& instanceId, const std::string& presetName);
    std::vector<std::string> getAvailablePluginPresets(const std::string& instanceId) const;

    // Plugin-Automation
    void addPluginAutomationPoint(const std::string& instanceId, const std::string& parameter, float time, float value);
    void removePluginAutomationPoint(const std::string& instanceId, const std::string& parameter, float time);
    void clearPluginAutomation(const std::string& instanceId, const std::string& parameter);
    float getPluginAutomatedValue(const std::string& instanceId, const std::string& parameter, float time) const;

    // VR-spezifische Funktionen
    void setPluginVRPosition(const std::string& instanceId, const glm::vec3& position);
    void setPluginVRRotation(const std::string& instanceId, const glm::quat& rotation);
    void setPluginVRScale(const std::string& instanceId, const glm::vec3& scale);
    void setPluginVRInteraction(const std::string& instanceId, const std::string& interaction);
    void setPluginVRFeedback(const std::string& instanceId, const std::string& feedback);

private:
    struct PluginInfo {
        std::string path;
        std::string name;
        std::string vendor;
        std::string category;
        std::string version;
        PluginType type;
        std::vector<PluginParameter> parameters;
        void* handle;
        std::function<std::shared_ptr<PluginInterface>()> createInstance;
    };

    struct PluginInstance {
        std::string pluginId;
        std::shared_ptr<PluginInterface> instance;
        std::map<std::string, float> parameters;
        std::map<std::string, bool> automatedParameters;
        std::map<std::string, std::vector<std::pair<float, float>>> automationPoints;
    };

    std::map<std::string, PluginInfo> plugins;
    std::map<std::string, PluginInstance> instances;
    mutable std::mutex mutex;

    // Hilfsfunktionen
    std::string generatePluginId() const;
    std::string generateInstanceId() const;
    bool validatePlugin(const std::string& path) const;
    void initializePlugin(const std::string& pluginId);
    void cleanupPlugin(const std::string& pluginId);
    void updatePluginInstance(const std::string& instanceId);
};

} // namespace VRMusicStudio 
>>>>>>> 0dff1c4 (init 2)
