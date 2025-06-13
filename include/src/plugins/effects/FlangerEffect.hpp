#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>

namespace VR_DAW {

class FlangerEffect : public EffectPlugin {
public:
    FlangerEffect();
    ~FlangerEffect() override;

    // Plugin-Identifikation
    const char* getName() const override { return "Flanger"; }
    const char* getVendor() const override { return "VR DAW"; }
    int getVersion() const override { return 1; }

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
    float rate;          // 0.1 - 10.0 Hz
    float depth;         // 0.0 - 1.0
    float feedback;      // 0.0 - 0.9
    float mix;           // 0.0 - 1.0
    float stereoWidth;   // 0.0 - 1.0
    float syncRate;      // 0.0625 - 1.0
    float phase;         // 0.0 - 1.0

    // Automatisierung
    bool automatedRate;
    bool automatedDepth;
    bool automatedFeedback;
    bool automatedMix;
    bool automatedStereoWidth;
    bool automatedSyncRate;
    bool automatedPhase;

    // Flanger-Zustand
    float lfoPhase;      // LFO-Phase
    std::vector<float> delayBuffer;  // Verzögerungsbuffer
    size_t writePos;     // Schreibposition
    size_t readPos;      // Leseposition

    // Hilfsfunktionen
    float processLFO();
    float calculateSyncTime();
};

} // namespace VR_DAW 