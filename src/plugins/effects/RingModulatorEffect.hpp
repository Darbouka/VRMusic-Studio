#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>

namespace VR_DAW {

class RingModulatorEffect : public EffectPlugin {
public:
    RingModulatorEffect();
    ~RingModulatorEffect();

    // Plugin-Identifikation
    std::string getName() const override { return "Ring Modulator"; }
    std::string getVendor() const override { return "VR DAW"; }
    std::string getCategory() const override { return "Effect"; }
    std::string getVersion() const override { return "1.0.0"; }

    // Plugin-Lebenszyklus
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
    float frequency;    // 0.0 - 2000.0 Hz
    float mix;         // 0.0 - 1.0
    float depth;       // 0.0 - 1.0
    float phase;       // 0.0 - 360.0 degrees
    float syncRate;    // 0.0 - 1.0
    float stereoWidth; // 0.0 - 1.0
    float quality;     // 0.0 - 1.0

    // Automatisierungs-Flags
    bool automatedFrequency;
    bool automatedMix;
    bool automatedDepth;
    bool automatedPhase;
    bool automatedSyncRate;
    bool automatedStereoWidth;
    bool automatedQuality;

    // Zustandsvariablen
    float lfoPhase;
    float lastFrequency;
    float lastPhase;

    // Hilfsmethoden
    float processLFO();
    float calculateSyncTime();
    void updateOscillator();
};

} // namespace VR_DAW 