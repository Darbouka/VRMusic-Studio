#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>
#include <deque>

namespace VR_DAW {

class SlapbackDelayEffect : public EffectPlugin {
public:
    SlapbackDelayEffect();
    ~SlapbackDelayEffect();

    // Plugin-Identifikation
    std::string getName() const override { return "Slapback Delay"; }
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
    float time;        // 0.0 - 200.0 ms
    float feedback;    // 0.0 - 0.9
    float mix;        // 0.0 - 1.0
    float tone;       // 0.0 - 1.0
    float stereoWidth; // 0.0 - 1.0
    float quality;    // 0.0 - 1.0

    // Automatisierungs-Flags
    bool automatedTime;
    bool automatedFeedback;
    bool automatedMix;
    bool automatedTone;
    bool automatedStereoWidth;
    bool automatedQuality;

    // Zustandsvariablen
    struct DelayLine {
        std::deque<float> buffer;
        unsigned long writePos;
        unsigned long readPos;
        float time;
        float level;
        float lowpass;
        float highpass;
    };
    DelayLine leftDelay;
    DelayLine rightDelay;
    unsigned long bufferSize;

    // Hilfsmethoden
    void initializeDelayLines();
    void updateDelayLines();
    float processDelayLine(DelayLine& delay, float input);
    void updateDelayTimes();
    void updateFilterCoefficients();
};

} // namespace VR_DAW 