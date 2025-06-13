#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>
#include <deque>

namespace VR_DAW {

class AutoWahEffect : public EffectPlugin {
public:
    AutoWahEffect();
    ~AutoWahEffect();

    // Plugin-Identifikation
    std::string getName() const override { return "Auto Wah"; }
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

    // Preset-Management
    void loadPreset(const std::string& presetName) override;
    void savePreset(const std::string& presetName) override;
    std::vector<std::string> getAvailablePresets() const override;

private:
    // Parameter
    float sensitivity;    // 0.0 - 1.0
    float attack;         // 0.1 - 100.0 ms
    float release;        // 10.0 - 1000.0 ms
    float minFreq;        // 100.0 - 1000.0 Hz
    float maxFreq;        // 1000.0 - 10000.0 Hz
    float resonance;      // 0.1 - 10.0
    float mix;           // 0.0 - 1.0
    float quality;       // 0.0 - 1.0

    // Automatisierungs-Flags
    bool automatedSensitivity;
    bool automatedAttack;
    bool automatedRelease;
    bool automatedMinFreq;
    bool automatedMaxFreq;
    bool automatedResonance;
    bool automatedMix;
    bool automatedQuality;

    // Zustandsvariablen
    struct FilterState {
        float b0, b1, b2;
        float a1, a2;
        float x1, x2;
        float y1, y2;
        float envelope;
        float attackCoeff;
        float releaseCoeff;
    };
    FilterState leftFilter;
    FilterState rightFilter;

    // Hilfsmethoden
    void initializeFilters();
    void updateFilters();
    float processFilter(FilterState& filter, float input);
    void updateEnvelope(FilterState& filter, float input);
    void calculateFilterCoefficients(FilterState& filter, float frequency);
};

} // namespace VR_DAW 