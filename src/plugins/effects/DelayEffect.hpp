#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>

namespace VR_DAW {

class DelayEffect : public EffectPlugin {
public:
    DelayEffect();
    ~DelayEffect();

    // Plugin-Identifikation
    std::string getName() const override { return "Delay"; }
    std::string getVendor() const override { return "VR DAW"; }
    std::string getVersion() const override { return "1.0.0"; }
    PluginType getType() const override { return PluginType::Effect; }

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
    float time;        // 0.0 - 2.0
    float feedback;    // 0.0 - 0.9
    float mix;         // 0.0 - 1.0
    float stereoWidth; // 0.0 - 1.0
    float syncRate;    // 0.0625 - 1.0
    float lowCut;      // 20.0 - 20000.0
    float highCut;     // 20.0 - 20000.0

    // Automation-Flags
    bool automatedTime;
    bool automatedFeedback;
    bool automatedMix;
    bool automatedStereoWidth;
    bool automatedSyncRate;
    bool automatedLowCut;
    bool automatedHighCut;

    // Delay-Buffer
    std::vector<float> delayBuffer;
    size_t bufferSize;
    size_t writePos;
    size_t readPos;

    // Filter-Koeffizienten
    float a0, a1, a2, b1, b2;  // Low-Cut
    float c0, c1, c2, d1, d2;  // High-Cut
    float x1[2], x2[2], y1[2], y2[2];  // Low-Cut
    float u1[2], u2[2], v1[2], v2[2];  // High-Cut

    // Hilfsfunktionen
    void updateFilterCoefficients();
    float processFilter(float input, int channel);
    float calculateSyncTime();
};

} // namespace VR_DAW 