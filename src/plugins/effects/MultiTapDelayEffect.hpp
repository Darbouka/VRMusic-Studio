#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>
#include <deque>

namespace VR_DAW {

class MultiTapDelayEffect : public EffectPlugin {
public:
    MultiTapDelayEffect();
    ~MultiTapDelayEffect();

    // Plugin-Identifikation
    std::string getName() const override { return "Multi-Tap Delay"; }
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
    float time1;       // 0.0 - 2000.0 ms
    float time2;       // 0.0 - 2000.0 ms
    float time3;       // 0.0 - 2000.0 ms
    float time4;       // 0.0 - 2000.0 ms
    float feedback;    // 0.0 - 0.9
    float mix;        // 0.0 - 1.0
    float quality;    // 0.0 - 1.0

    // Automatisierungs-Flags
    bool automatedTime1;
    bool automatedTime2;
    bool automatedTime3;
    bool automatedTime4;
    bool automatedFeedback;
    bool automatedMix;
    bool automatedQuality;

    // Zustandsvariablen
    struct Tap {
        std::deque<float> buffer;
        unsigned long writePos;
        unsigned long readPos;
        float time;
        float level;
    };
    std::vector<Tap> taps;
    unsigned long bufferSize;

    // Hilfsmethoden
    void initializeTaps();
    void updateTaps();
    float processTap(const Tap& tap, float input);
    void updateTapTimes();
};

} // namespace VR_DAW 