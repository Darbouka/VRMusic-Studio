#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <glm/glm.hpp>
#include "../core/Logger.hpp"

<<<<<<< HEAD
namespace VR_DAW {

enum class PluginType {
    Effect,
    Instrument,
    Tool,
    SampleBank,
    Mastering,
    Automation
=======
namespace VRMusicStudio {

enum class PluginType {
    Instrument,
    Effect,
    Analyzer,
    Generator,
    Utility
>>>>>>> 0dff1c4 (init 2)
};

// Plugin-Parameter
struct PluginParameter {
<<<<<<< HEAD
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
=======
    std::string name;
    float minValue;
    float maxValue;
    float defaultValue;
    float currentValue;
    bool automated;
    std::string unit;
    std::string description;
>>>>>>> 0dff1c4 (init 2)
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
<<<<<<< HEAD
    virtual const std::string& getId() const = 0;
    virtual const std::string& getName() const = 0;
=======
    virtual std::string getName() const = 0;
    virtual std::string getVendor() const = 0;
    virtual std::string getCategory() const = 0;
    virtual std::string getVersion() const = 0;
>>>>>>> 0dff1c4 (init 2)
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
<<<<<<< HEAD
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
=======

    // Automation
    virtual void addAutomationPoint(const std::string& parameter, float time, float value) = 0;
    virtual void removeAutomationPoint(const std::string& parameter, float time) = 0;
    virtual void clearAutomation(const std::string& parameter) = 0;
    virtual float getAutomatedValue(const std::string& parameter, float time) const = 0;

    // Preset-Management
    virtual void loadPreset(const std::string& presetName) = 0;
    virtual void savePreset(const std::string& presetName) = 0;
    virtual std::vector<std::string> getAvailablePresets() const = 0;

    // VR-spezifische Funktionen
    virtual void setVRPosition(const glm::vec3& position) = 0;
    virtual void setVRRotation(const glm::quat& rotation) = 0;
    virtual void setVRScale(const glm::vec3& scale) = 0;
    virtual void setVRInteraction(const std::string& interaction) = 0;
    virtual void setVRFeedback(const std::string& feedback) = 0;
>>>>>>> 0dff1c4 (init 2)
};

class InstrumentPlugin : public PluginInterface {
public:
<<<<<<< HEAD
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
=======
    virtual ~InstrumentPlugin() = default;

    // MIDI-Verarbeitung
    virtual void processMidi(const std::vector<uint8_t>& midiData) = 0;
    virtual void setMidiChannel(int channel) = 0;
    virtual int getMidiChannel() const = 0;
    virtual void setMidiLearn(bool enabled) = 0;
    virtual bool isMidiLearnEnabled() const = 0;

    // Instrument-spezifische Funktionen
    virtual void setVoiceCount(int count) = 0;
    virtual int getVoiceCount() const = 0;
    virtual void setPolyphony(int voices) = 0;
    virtual int getPolyphony() const = 0;
    virtual void setTuning(float tuning) = 0;
    virtual float getTuning() const = 0;
};

class EffectPlugin : public PluginInterface {
public:
    virtual ~EffectPlugin() = default;

    // Effect-spezifische Funktionen
    virtual void setBypass(bool bypass) = 0;
    virtual bool isBypassed() const = 0;
    virtual void setWetDryMix(float mix) = 0;
    virtual float getWetDryMix() const = 0;
    virtual void setLatency(int samples) = 0;
    virtual int getLatency() const = 0;
};

class AnalyzerPlugin : public PluginInterface {
public:
    virtual ~AnalyzerPlugin() = default;

    // Analyzer-spezifische Funktionen
    virtual void setWindowSize(int size) = 0;
    virtual int getWindowSize() const = 0;
    virtual void setOverlap(float overlap) = 0;
    virtual float getOverlap() const = 0;
    virtual void setFFTSize(int size) = 0;
    virtual int getFFTSize() const = 0;
    virtual void getAnalysisData(float* data, int size) = 0;
};

class GeneratorPlugin : public PluginInterface {
public:
    virtual ~GeneratorPlugin() = default;

    // Generator-spezifische Funktionen
    virtual void setFrequency(float frequency) = 0;
    virtual float getFrequency() const = 0;
    virtual void setAmplitude(float amplitude) = 0;
    virtual float getAmplitude() const = 0;
    virtual void setPhase(float phase) = 0;
    virtual float getPhase() const = 0;
    virtual void setWaveform(const std::string& waveform) = 0;
    virtual std::string getWaveform() const = 0;
};

class UtilityPlugin : public PluginInterface {
public:
    virtual ~UtilityPlugin() = default;

    // Utility-spezifische Funktionen
    virtual void setGain(float gain) = 0;
    virtual float getGain() const = 0;
    virtual void setPan(float pan) = 0;
    virtual float getPan() const = 0;
    virtual void setMute(bool mute) = 0;
    virtual bool isMuted() const = 0;
    virtual void setSolo(bool solo) = 0;
    virtual bool isSoloed() const = 0;
>>>>>>> 0dff1c4 (init 2)
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

<<<<<<< HEAD
} // namespace VR_DAW 
=======
} // namespace VRMusicStudio 
>>>>>>> 0dff1c4 (init 2)
