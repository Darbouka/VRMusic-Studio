#pragma once

#include "EffectPlugin.hpp"
#include <cmath>

namespace VR_DAW {

class ExciterEffect : public EffectPlugin {
public:
    ExciterEffect();
    ~ExciterEffect();

    // Plugin-Identifikation
    const char* getName() const override { return "Exciter"; }
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
    float amount;      // 0.0 - 1.0
    float mix;         // 0.0 - 1.0
    float frequency;   // 1000 - 8000 Hz
    float q;           // 0.1 - 10.0
    float drive;       // 1.0 - 10.0
    float symmetry;    // 0.0 - 1.0
    float bias;        // -1.0 - 1.0

    // Automatisierung
    bool automatedAmount;
    bool automatedMix;
    bool automatedFrequency;
    bool automatedQ;
    bool automatedDrive;
    bool automatedSymmetry;
    bool automatedBias;

    // Zustand
    float x1, x2, y1, y2; // Filter-Zustand
    float a0, a1, a2, b1, b2; // Filter-Koeffizienten

    // Hilfsmethoden
    void updateFilterCoefficients();
    float processHarmonics(float input);
};

} // namespace VR_DAW 