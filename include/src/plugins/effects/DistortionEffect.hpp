#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>

namespace VR_DAW {

class DistortionEffect : public EffectPlugin {
public:
    DistortionEffect();
    ~DistortionEffect() override;

    // Plugin-Identifikation
    const char* getName() const override { return "Distortion"; }
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
    float drive;        // 0.0 - 1.0
    float mix;          // 0.0 - 1.0
    float stereoWidth;  // 0.0 - 1.0
    float tone;         // 0.0 - 1.0
    float bias;         // -1.0 - 1.0
    float symmetry;     // 0.0 - 1.0
    float noise;        // 0.0 - 1.0

    // Automatisierung
    bool automatedDrive;
    bool automatedMix;
    bool automatedStereoWidth;
    bool automatedTone;
    bool automatedBias;
    bool automatedSymmetry;
    bool automatedNoise;

    // Filter-Zustand
    float x1[2];  // Eingangsverzögerung (Stereo)
    float y1[2];  // Ausgangsverzögerung (Stereo)

    // Hilfsfunktionen
    float processFilter(float input, int channel);
    float applyDistortion(float input);
    void updateFilterCoefficients();
};

} // namespace VR_DAW 