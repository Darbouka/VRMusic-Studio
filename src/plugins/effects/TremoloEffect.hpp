#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>
#include <cmath>

namespace VR_DAW {

class TremoloEffect : public EffectPlugin {
public:
    TremoloEffect();
    ~TremoloEffect();

    // Plugin-Identifikation
    const char* getName() const override { return "Tremolo"; }
    const char* getManufacturer() const override { return "VR DAW"; }
    const char* getCategory() const override { return "Effect"; }
    const char* getVersion() const override { return "1.0.0"; }

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
    float rate;        // 0.1 - 20 Hz
    float depth;       // 0.0 - 1.0
    float shape;       // 0.0 - 1.0 (Sine to Square)
    float mix;         // 0.0 - 1.0
    float syncRate;    // 0.0 - 1.0
    float phase;       // 0.0 - 1.0
    float stereoPhase; // 0.0 - 1.0

    // Automatisierung
    bool automatedRate;
    bool automatedDepth;
    bool automatedShape;
    bool automatedMix;
    bool automatedSyncRate;
    bool automatedPhase;
    bool automatedStereoPhase;

    // Zustand
    float lfoPhase;

    // Hilfsmethoden
    float processLFO();
    float calculateSyncTime();
};

} // namespace VR_DAW 