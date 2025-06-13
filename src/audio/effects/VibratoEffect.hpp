#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>

namespace VRMusicStudio {

class VibratoEffect : public EffectPlugin {
public:
    VibratoEffect();
    ~VibratoEffect();

    // Plugin-Identifikation
    std::string getName() const override { return "Vibrato"; }
    std::string getVendor() const override { return "VRMusicStudio"; }
    std::string getVersion() const override { return "1.0.0"; }
    PluginType getType() const override { return PluginType::Effect; }

    // Lifecycle
    bool initialize() override;
    void shutdown() override;

    // Parameter-Management
    std::vector<PluginParameter> getParameters() const override;
    void setParameter(const std::string& name, float value) override;
    float getParameter(const std::string& name) const override;
    void setParameterAutomated(const std::string& name, bool automated) override;
    bool isParameterAutomated(const std::string& name) const override;

    // Audio-Verarbeitung
    void processAudio(float* buffer, unsigned long framesPerBuffer) override;

    // Automation
    void addAutomationPoint(const std::string& parameter, float time, float value) override {}
    void removeAutomationPoint(const std::string& parameter, float time) override {}
    void clearAutomation(const std::string& parameter) override {}
    float getAutomatedValue(const std::string& parameter, float time) const override { return 0.0f; }

    // Preset-Management
    void loadPreset(const std::string& presetName) override;
    void savePreset(const std::string& presetName) override;
    std::vector<std::string> getAvailablePresets() const override;

private:
    // Parameter
    float rate;        // 0.1 - 10.0 Hz
    float depth;       // 0.0 - 1.0
    float mix;         // 0.0 - 1.0
    float stereoWidth; // 0.0 - 1.0
    float syncRate;    // 0.0625 - 1.0
    float shape;       // 0.0 - 1.0
    float phase;       // 0.0 - 1.0

    // Automation-Flags
    bool automatedRate;
    bool automatedDepth;
    bool automatedMix;
    bool automatedStereoWidth;
    bool automatedSyncRate;
    bool automatedShape;
    bool automatedPhase;

    // State Variables
    float currentRate;
    float currentDepth;
    float currentMix;
    float currentStereoWidth;
    float currentSyncRate;
    float currentShape;
    float currentPhase;
    float phaseAccumulator;
    float stereoPhaseAccumulator;

    // Helper Functions
    void updateParameters();
    float calculateModulation(float phase);
    void processMono(float* buffer, unsigned long framesPerBuffer);
    void processStereo(float* buffer, unsigned long framesPerBuffer);
};

} // namespace VRMusicStudio 