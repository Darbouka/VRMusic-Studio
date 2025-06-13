#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>

namespace VR_DAW {

class LimiterEffect : public EffectPlugin {
public:
    LimiterEffect();
    ~LimiterEffect() override;

    // Plugin-Identifikation
    const char* getName() const override { return "Limiter"; }
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
    float attack;       // 0.1 - 10.0 ms
    float release;      // 1.0 - 100.0 ms
    float makeup;       // 0.0 - 24.0 dB
    float lookahead;    // 0.0 - 10.0 ms
    float knee;         // 0.0 - 10.0 dB
    float mix;          // 0.0 - 1.0

    // Automatisierung
    bool automatedThreshold;
    bool automatedAttack;
    bool automatedRelease;
    bool automatedMakeup;
    bool automatedLookahead;
    bool automatedKnee;
    bool automatedMix;

    // Limiter-Zustand
    float envelope;     // Hüllkurve
    float gain;         // Verstärkung
    std::vector<float> delayBuffer;  // Lookahead-Buffer
    size_t writePos;    // Schreibposition im Buffer
    size_t readPos;     // Leseposition im Buffer

    // Hilfsfunktionen
    float calculateGain(float input);
    float calculateEnvelope(float input);
    float dbToLinear(float db);
    float linearToDb(float linear);
    void updateDelayBuffer();
};

} // namespace VR_DAW 