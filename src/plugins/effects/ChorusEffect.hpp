#pragma once

#include "EffectPlugin.hpp"
#include <string>
#include <vector>
#include <cmath>

namespace VR_DAW {

class ChorusEffect : public EffectPlugin {
public:
    ChorusEffect();
    ~ChorusEffect();

    // Plugin-Identifikation
    const char* getName() const override { return "Chorus"; }
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

    // Preset-Management
    void loadPreset(const std::string& presetName) override;
    void savePreset(const std::string& presetName) override;
    std::vector<std::string> getAvailablePresets() const override;

private:
    // Parameter
    float rate;        // 0.1 - 10 Hz
    float depth;       // 0.0 - 1.0
    float feedback;    // 0.0 - 0.9
    float mix;         // 0.0 - 1.0
    float stereoWidth; // 0.0 - 1.0
    float syncRate;    // 0.0 - 1.0
    float phase;       // 0.0 - 1.0

    // Automatisierung
    bool automatedRate;
    bool automatedDepth;
    bool automatedFeedback;
    bool automatedMix;
    bool automatedStereoWidth;
    bool automatedSyncRate;
    bool automatedPhase;

    // Zustand
    float lfoPhase;
    std::vector<float> delayBuffer;
    size_t writePos;
    size_t readPos;

    // Hilfsmethoden
    float processLFO();
    float calculateSyncTime();
};

} // namespace VR_DAW 