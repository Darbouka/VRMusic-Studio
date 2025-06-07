#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>

namespace VR_DAW {

class PhaserEffect : public EffectPlugin {
public:
    PhaserEffect();
    ~PhaserEffect() override;

    // Plugin-Identifikation
    const char* getName() const override { return "Phaser"; }
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
    // Parameter
    float rate;          // 0.1 - 10.0 Hz
    float depth;         // 0.0 - 1.0
    float feedback;      // 0.0 - 0.9
    float mix;           // 0.0 - 1.0
    float stereoWidth;   // 0.0 - 1.0
    float syncRate;      // 0.0625 - 1.0
    float stages;        // 2.0 - 12.0
    float phase;         // 0.0 - 1.0

    // Automatisierung
    bool automatedRate;
    bool automatedDepth;
    bool automatedFeedback;
    bool automatedMix;
    bool automatedStereoWidth;
    bool automatedSyncRate;
    bool automatedStages;
    bool automatedPhase;

    // Phaser-Zustand
    float lfoPhase;      // LFO-Phase
    struct AllpassFilter {
        float x1, x2;    // Eingangsverzögerungen
        float y1, y2;    // Ausgangsverzögerungen
        float a1, a2;    // Filter-Koeffizienten
    };
    std::vector<AllpassFilter> filters;

    // Hilfsfunktionen
    float processLFO();
    float calculateSyncTime();
    void updateFilterCoefficients(float frequency);
};

} // namespace VR_DAW 