#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <glm/glm.hpp>
#include "../core/Logger.hpp"

namespace VR_DAW {

enum class PluginType {
    Effect,
    Instrument,
    Tool,
    SampleBank,
    Mastering,
    Automation
};

// Plugin-Parameter
struct PluginParameter {
    enum class Type {
        Float,
        Int,
        Bool,
        Choice,
        String
    };

    std::string name;
    std::string displayName;
    Type type;
    float minValue;
    float maxValue;
    float defaultValue;
    std::vector<std::string> choices;  // Für Choice-Parameter
    bool isAutomated;
    std::function<void(float)> onChange;
};

// Plugin-UI-Element
struct PluginUIElement {
    std::string id;
    std::string type;  // "knob", "slider", "button", etc.
    glm::vec3 position;
    glm::vec3 scale;
    std::string parameterId;
    bool isVisible;
};

// Plugin-Info
struct PluginInfo {
    std::string id;
    std::string name;
    std::string vendor;
    std::string version;
    std::string category;
    bool isSynth;
    bool isEffect;
    bool isMidiEffect;
};

// Plugin-Interface
class PluginInterface {
public:
    virtual ~PluginInterface() = default;

    // Plugin-Identifikation
    virtual const std::string& getId() const = 0;
    virtual const std::string& getName() const = 0;
    virtual PluginType getType() const = 0;

    // Plugin-Lebenszyklus
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void update() = 0;

    // Parameter-Management
    virtual std::vector<PluginParameter> getParameters() const = 0;
    virtual void setParameter(const std::string& name, float value) = 0;
    virtual float getParameter(const std::string& name) const = 0;
    virtual void setParameterAutomated(const std::string& name, bool automated) = 0;
    virtual bool isParameterAutomated(const std::string& name) const = 0;

    // Audio-Verarbeitung
    virtual void processAudio(float* buffer, unsigned long framesPerBuffer) = 0;
    virtual void processMidi(const std::vector<uint8_t>& midiData) = 0;

    // Automation
    virtual void setAutomationPoint(const std::string& parameter, double time, float value) = 0;
    virtual void removeAutomationPoint(const std::string& parameter, double time) = 0;
    virtual void clearAutomation(const std::string& parameter) = 0;
    virtual std::vector<std::pair<double, float>> getAutomationPoints(const std::string& parameter) const = 0;

    // Preset-Management
    virtual void savePreset(const std::string& name) = 0;
    virtual void loadPreset(const std::string& name) = 0;
    virtual std::vector<std::string> getPresetList() const = 0;

    // UI-Integration
    virtual void setUIPosition(const glm::vec3& position) = 0;
    virtual void setUIOrientation(const glm::quat& orientation) = 0;
    virtual void setUIScale(const glm::vec3& scale) = 0;
    virtual void renderUI() = 0;
};

// Spezifische Plugin-Typen
class EffectPlugin : public PluginInterface {
public:
    virtual void setBypass(bool bypass) = 0;
    virtual bool isBypassed() const = 0;
    virtual void setWetDryMix(float mix) = 0;
    virtual float getWetDryMix() const = 0;
};

class InstrumentPlugin : public PluginInterface {
public:
    virtual void noteOn(int note, int velocity) = 0;
    virtual void noteOff(int note) = 0;
    virtual void setPitchBend(float value) = 0;
    virtual void setModulation(float value) = 0;
    virtual void setAftertouch(float value) = 0;
};

class SampleBankPlugin : public PluginInterface {
public:
    virtual void loadSample(const std::string& path) = 0;
    virtual void unloadSample(const std::string& path) = 0;
    virtual void setSampleStart(double time) = 0;
    virtual void setSampleEnd(double time) = 0;
    virtual void setLoopPoints(double start, double end) = 0;
    virtual void setLoopEnabled(bool enabled) = 0;
};

class MasteringPlugin : public PluginInterface {
public:
    virtual void setLimiterThreshold(float threshold) = 0;
    virtual void setCompressorThreshold(float threshold) = 0;
    virtual void setCompressorRatio(float ratio) = 0;
    virtual void setEQBand(int band, float frequency, float gain, float q) = 0;
    virtual void setStereoWidth(float width) = 0;
};

class AutomationPlugin : public PluginInterface {
public:
    virtual void setAutomationMode(bool enabled) = 0;
    virtual void setAutomationCurve(const std::string& parameter, const std::vector<std::pair<double, float>>& points) = 0;
    virtual void setAutomationInterpolation(const std::string& parameter, bool smooth) = 0;
    virtual void setAutomationLoopPoints(double start, double end) = 0;
};

// Plugin-Factory
class PluginFactory {
public:
    virtual ~PluginFactory() = default;
    virtual std::shared_ptr<PluginInterface> createPlugin() = 0;
    virtual PluginInfo getPluginInfo() const = 0;
};

// Plugin-Loader
class PluginLoader {
public:
    static std::shared_ptr<PluginInterface> loadPlugin(const std::string& pluginPath);
    static void unloadPlugin(const std::string& pluginId);
    static std::vector<PluginInfo> getLoadedPlugins();
    static bool isPluginLoaded(const std::string& pluginId);
};

} // namespace VR_DAW 