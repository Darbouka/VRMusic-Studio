#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>

namespace VR_DAW {

class EQEffect : public EffectPlugin {
public:
    EQEffect();
    ~EQEffect() override;

    // Plugin-Identifikation
    const char* getName() const override { return "EQ"; }
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
    // Parameter für jeden Band
    struct Band {
        float frequency;    // 20.0 - 20000.0 Hz
        float gain;         // -12.0 - 12.0 dB
        float q;            // 0.1 - 10.0
        bool enabled;       // true/false
        bool automated;     // true/false
    };

    // 10 Bands für den EQ
    std::vector<Band> bands;

    // Filter-Koeffizienten
    struct FilterCoeffs {
        float a0, a1, a2;   // Zähler-Koeffizienten
        float b1, b2;       // Nenner-Koeffizienten
    };

    // Filter-Zustand
    struct FilterState {
        float x1, x2;       // Eingangsverzögerungen
        float y1, y2;       // Ausgangsverzögerungen
    };

    std::vector<FilterCoeffs> coeffs;
    std::vector<FilterState> states;

    // Hilfsfunktionen
    void updateFilterCoefficients(size_t bandIndex);
    float processFilter(float input, size_t bandIndex);
    float dbToLinear(float db);
};

} // namespace VR_DAW 