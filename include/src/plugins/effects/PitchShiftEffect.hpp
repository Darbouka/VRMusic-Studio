#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <cmath>

namespace VR_DAW {

class PitchShiftEffect : public EffectPlugin {
public:
    PitchShiftEffect();
    ~PitchShiftEffect();

    // Plugin-Identifikation
    const char* getName() const override { return "PitchShift"; }
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
    float pitch;       // -12.0 - +12.0 Halbtöne
    float mix;         // 0.0 - 1.0
    float formant;     // 0.0 - 1.0
    float grainSize;   // 0.001 - 0.1 s
    float overlap;     // 0.0 - 0.9
    float window;      // 0.0 - 1.0
    float quality;     // 0.0 - 1.0

    // Automatisierung
    bool automatedPitch;
    bool automatedMix;
    bool automatedFormant;
    bool automatedGrainSize;
    bool automatedOverlap;
    bool automatedWindow;
    bool automatedQuality;

    // Zustand
    std::vector<float> inputBuffer;
    std::vector<float> outputBuffer;
    size_t writePos;
    size_t readPos;

    // Hilfsmethoden
    void processGrain(float* input, float* output, size_t length);
    float calculateWindow(float position);
    void updateBuffers();
};

} // namespace VR_DAW 