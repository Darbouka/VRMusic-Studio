#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>

namespace VR_DAW {

class GateEffect : public EffectPlugin {
public:
    GateEffect();
    ~GateEffect() override;

    // Plugin-Identifikation
    const char* getName() const override { return "Gate"; }
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
    float threshold;    // -60.0 - 0.0 dB
    float attack;       // 0.1 - 100.0 ms
    float release;      // 1.0 - 1000.0 ms
    float ratio;        // 1.0 - 100.0
    float range;        // 0.0 - 100.0 dB
    float knee;         // 0.0 - 40.0 dB
    float mix;          // 0.0 - 1.0

    // Automatisierung
    bool automatedThreshold;
    bool automatedAttack;
    bool automatedRelease;
    bool automatedRatio;
    bool automatedRange;
    bool automatedKnee;
    bool automatedMix;

    // Gate-Zustand
    float envelope;     // Hüllkurve
    float gain;         // Verstärkung

    // Hilfsfunktionen
    float calculateGain(float input);
    float calculateEnvelope(float input);
    float dbToLinear(float db);
    float linearToDb(float linear);
};

} // namespace VR_DAW 