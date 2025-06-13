#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <random>
#include <deque>

namespace VR_DAW {

class TimeWarpEffect : public EffectPlugin {
public:
    TimeWarpEffect();
    ~TimeWarpEffect();

    // Plugin-Identifikation
    std::string getName() const override { return "TimeWarp"; }
    std::string getVendor() const override { return "VR_DAW"; }
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
    float speed;         // 0.0 - 1.0
    float direction;     // 0.0 - 1.0
    float distortion;    // 0.0 - 1.0
    float glitch;        // 0.0 - 1.0
    float stutter;       // 0.0 - 1.0
    float mix;           // 0.0 - 1.0
    float quality;       // 0.0 - 1.0

    // Automatisierung
    bool automatedSpeed;
    bool automatedDirection;
    bool automatedDistortion;
    bool automatedGlitch;
    bool automatedStutter;
    bool automatedMix;
    bool automatedQuality;

    // Zustandsvariablen
    struct TimeState {
        std::deque<float> buffer;
        float position;
        float speed;
        float direction;
    };

    struct WarpState {
        std::vector<TimeState> states;
        float time;
        float glitch;
    };

    std::vector<WarpState> states;
    std::mt19937 rng;
    unsigned long bufferSize;

    // Private Methoden
    void initializeStates();
    void updateStates();
    void processState(WarpState& state, float* buffer, unsigned long framesPerBuffer);
    void applyDistortion(float* buffer, unsigned long framesPerBuffer);
    void applyGlitch(float* buffer, unsigned long framesPerBuffer);
    void applyStutter(float* buffer, unsigned long framesPerBuffer);
};

} // namespace VR_DAW 