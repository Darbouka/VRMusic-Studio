#include "PluginManager.hpp"
#include <spdlog/spdlog.h>
#include <filesystem>
<<<<<<< HEAD

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
=======
#include <random>
#include <chrono>
#include <algorithm>

namespace VRMusicStudio {

PluginManager::PluginManager() {}

PluginManager::~PluginManager() {
    // Entlade alle Plugins
    for (const auto& [pluginId, plugin] : plugins) {
        cleanupPlugin(pluginId);
    }
    plugins.clear();
    instances.clear();
}

bool PluginManager::loadPlugin(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex);

    try {
        if (!std::filesystem::exists(path)) {
            spdlog::error("Plugin file not found: {}", path);
            return false;
        }

        if (!validatePlugin(path)) {
            spdlog::error("Invalid plugin file: {}", path);
            return false;
        }

        std::string pluginId = generatePluginId();
        PluginInfo info;
        info.path = path;
        info.handle = nullptr; // TODO: Plugin-Bibliothek laden

        // Plugin-Informationen laden
        // TODO: Plugin-Metadaten laden

        plugins[pluginId] = info;
        initializePlugin(pluginId);

        return true;
    }
    catch (const std::exception& e) {
        spdlog::error("Failed to load plugin: {}", e.what());
        return false;
    }
}

bool PluginManager::unloadPlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex);

    try {
        auto it = plugins.find(pluginId);
        if (it == plugins.end()) {
            spdlog::error("Plugin not found: {}", pluginId);
            return false;
        }

        // Entferne alle Instanzen des Plugins
        for (auto it = instances.begin(); it != instances.end();) {
            if (it->second.pluginId == pluginId) {
                it = instances.erase(it);
            }
            else {
                ++it;
            }
        }

        cleanupPlugin(pluginId);
        plugins.erase(it);

        return true;
    }
    catch (const std::exception& e) {
        spdlog::error("Failed to unload plugin: {}", e.what());
        return false;
    }
}

std::shared_ptr<PluginInterface> PluginManager::createPluginInstance(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    try {
        auto it = plugins.find(pluginId);
        if (it == plugins.end()) {
            spdlog::error("Plugin not found: {}", pluginId);
            return nullptr;
        }

        std::string instanceId = generateInstanceId();
        PluginInstance instance;
        instance.pluginId = pluginId;
        instance.instance = it->second.createInstance();

        if (!instance.instance) {
            spdlog::error("Failed to create plugin instance: {}", pluginId);
        return nullptr;
    }

        if (!instance.instance->initialize()) {
            spdlog::error("Failed to initialize plugin instance: {}", pluginId);
        return nullptr;
    }

        instances[instanceId] = instance;
        return instance.instance;
    }
    catch (const std::exception& e) {
        spdlog::error("Failed to create plugin instance: {}", e.what());
        return nullptr;
    }
}

void PluginManager::destroyPluginInstance(const std::string& instanceId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    try {
        auto it = instances.find(instanceId);
        if (it == instances.end()) {
            spdlog::error("Plugin instance not found: {}", instanceId);
            return;
        }

        it->second.instance->shutdown();
        instances.erase(it);
    }
    catch (const std::exception& e) {
        spdlog::error("Failed to destroy plugin instance: {}", e.what());
>>>>>>> 0dff1c4 (init 2)
    }
}

std::vector<std::string> PluginManager::getAvailablePlugins() const {
    std::lock_guard<std::mutex> lock(mutex);
<<<<<<< HEAD
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
=======
    std::vector<std::string> result;
    for (const auto& [pluginId, plugin] : plugins) {
        result.push_back(pluginId);
    }
    return result;
}

std::vector<std::string> PluginManager::getPluginInstances() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> result;
    for (const auto& [instanceId, instance] : instances) {
        result.push_back(instanceId);
    }
    return result;
}

std::vector<std::string> PluginManager::getPluginsByType(PluginType type) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> result;
    for (const auto& [pluginId, plugin] : plugins) {
        if (plugin.type == type) {
            result.push_back(pluginId);
        }
    }
    return result;
}

std::vector<std::string> PluginManager::getPluginsByCategory(const std::string& category) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> result;
    for (const auto& [pluginId, plugin] : plugins) {
        if (plugin.category == category) {
            result.push_back(pluginId);
        }
    }
    return result;
}

std::vector<std::string> PluginManager::getPluginsByVendor(const std::string& vendor) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> result;
    for (const auto& [pluginId, plugin] : plugins) {
        if (plugin.vendor == vendor) {
            result.push_back(pluginId);
        }
    }
    return result;
}

std::string PluginManager::getPluginName(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = plugins.find(pluginId);
    if (it == plugins.end()) {
        return "";
    }
    return it->second.name;
}

std::string PluginManager::getPluginVendor(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = plugins.find(pluginId);
    if (it == plugins.end()) {
        return "";
    }
    return it->second.vendor;
}

std::string PluginManager::getPluginCategory(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = plugins.find(pluginId);
    if (it == plugins.end()) {
        return "";
    }
    return it->second.category;
}

std::string PluginManager::getPluginVersion(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = plugins.find(pluginId);
    if (it == plugins.end()) {
        return "";
    }
    return it->second.version;
}

PluginType PluginManager::getPluginType(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = plugins.find(pluginId);
    if (it == plugins.end()) {
        return PluginType::Utility;
    }
    return it->second.type;
}

std::vector<PluginParameter> PluginManager::getPluginParameters(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = plugins.find(pluginId);
    if (it == plugins.end()) {
        return {};
    }
    return it->second.parameters;
}

std::shared_ptr<PluginInterface> PluginManager::getPluginInstance(const std::string& instanceId) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return nullptr;
    }
    return it->second.instance;
}

void PluginManager::setPluginParameter(const std::string& instanceId, const std::string& parameter, float value) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return;
    }
    it->second.instance->setParameter(parameter, value);
    it->second.parameters[parameter] = value;
}

float PluginManager::getPluginParameter(const std::string& instanceId, const std::string& parameter) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return 0.0f;
    }
    return it->second.instance->getParameter(parameter);
}

void PluginManager::setPluginParameterAutomated(const std::string& instanceId, const std::string& parameter, bool automated) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return;
    }
    it->second.instance->setParameterAutomated(parameter, automated);
    it->second.automatedParameters[parameter] = automated;
}

bool PluginManager::isPluginParameterAutomated(const std::string& instanceId, const std::string& parameter) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return false;
    }
    return it->second.instance->isParameterAutomated(parameter);
}

void PluginManager::loadPluginPreset(const std::string& instanceId, const std::string& presetName) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return;
    }
    it->second.instance->loadPreset(presetName);
}

void PluginManager::savePluginPreset(const std::string& instanceId, const std::string& presetName) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return;
    }
    it->second.instance->savePreset(presetName);
}

std::vector<std::string> PluginManager::getAvailablePluginPresets(const std::string& instanceId) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return {};
    }
    return it->second.instance->getAvailablePresets();
}

void PluginManager::addPluginAutomationPoint(const std::string& instanceId, const std::string& parameter, float time, float value) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return;
    }
    it->second.instance->addAutomationPoint(parameter, time, value);
    it->second.automationPoints[parameter].push_back({time, value});
    std::sort(it->second.automationPoints[parameter].begin(), it->second.automationPoints[parameter].end());
}

void PluginManager::removePluginAutomationPoint(const std::string& instanceId, const std::string& parameter, float time) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return;
    }
    it->second.instance->removeAutomationPoint(parameter, time);
    auto& points = it->second.automationPoints[parameter];
    points.erase(std::remove_if(points.begin(), points.end(),
                              [time](const auto& point) { return std::abs(point.first - time) < 0.001f; }),
                points.end());
}

void PluginManager::clearPluginAutomation(const std::string& instanceId, const std::string& parameter) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return;
    }
    it->second.instance->clearAutomation(parameter);
    it->second.automationPoints[parameter].clear();
}

float PluginManager::getPluginAutomatedValue(const std::string& instanceId, const std::string& parameter, float time) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return 0.0f;
    }
    return it->second.instance->getAutomatedValue(parameter, time);
}

void PluginManager::setPluginVRPosition(const std::string& instanceId, const glm::vec3& position) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return;
    }
    it->second.instance->setVRPosition(position);
}

void PluginManager::setPluginVRRotation(const std::string& instanceId, const glm::quat& rotation) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return;
    }
    it->second.instance->setVRRotation(rotation);
}

void PluginManager::setPluginVRScale(const std::string& instanceId, const glm::vec3& scale) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return;
    }
    it->second.instance->setVRScale(scale);
}

void PluginManager::setPluginVRInteraction(const std::string& instanceId, const std::string& interaction) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return;
    }
    it->second.instance->setVRInteraction(interaction);
}

void PluginManager::setPluginVRFeedback(const std::string& instanceId, const std::string& feedback) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return;
    }
    it->second.instance->setVRFeedback(feedback);
}

std::string PluginManager::generatePluginId() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static const char* hex = "0123456789abcdef";
    std::string uuid;
    uuid.reserve(36);
    for (int i = 0; i < 36; ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            uuid += '-';
        }
        else {
            uuid += hex[dis(gen)];
        }
    }
    return uuid;
}

std::string PluginManager::generateInstanceId() const {
    return generatePluginId();
}

bool PluginManager::validatePlugin(const std::string& path) const {
    // TODO: Plugin-Validierung implementieren
    return true;
}

void PluginManager::initializePlugin(const std::string& pluginId) {
    // TODO: Plugin-Initialisierung implementieren
}

void PluginManager::cleanupPlugin(const std::string& pluginId) {
    // TODO: Plugin-Bereinigung implementieren
}

void PluginManager::updatePluginInstance(const std::string& instanceId) {
    auto it = instances.find(instanceId);
    if (it == instances.end()) {
        return;
    }
    it->second.instance->update();
}

} // namespace VRMusicStudio 
>>>>>>> 0dff1c4 (init 2)
