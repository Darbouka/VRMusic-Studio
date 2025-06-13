#pragma once

#include "EffectPlugin.hpp"
#include <cmath>

namespace VR_DAW {

class SaturatorEffect : public EffectPlugin {
public:
    SaturatorEffect();
    ~SaturatorEffect();

    // Plugin-Identifikation
    const char* getName() const override { return "Saturator"; }
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
    float drive;       // 1.0 - 10.0
    float mix;         // 0.0 - 1.0
    float bias;        // -1.0 - 1.0
    float symmetry;    // 0.0 - 1.0
    float dcOffset;    // -1.0 - 1.0
    float oversample;  // 1.0 - 4.0
    float quality;     // 0.0 - 1.0

    // Automatisierung
    bool automatedDrive;
    bool automatedMix;
    bool automatedBias;
    bool automatedSymmetry;
    bool automatedDcOffset;
    bool automatedOversample;
    bool automatedQuality;

    // Zustand
    float x1, x2, y1, y2; // Filter-Zustand
    float a0, a1, a2, b1, b2; // Filter-Koeffizienten

    // Hilfsmethoden
    float processSaturation(float input);
    void updateFilterCoefficients();
};

} // namespace VR_DAW 