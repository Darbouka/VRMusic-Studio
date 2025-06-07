#pragma once

#include "EffectPlugin.hpp"
#include <deque>
#include <vector>
#include <random>

namespace VR_DAW {

class IceverbEffect : public EffectPlugin {
public:
    IceverbEffect();
    ~IceverbEffect();

    // Plugin Identifikation
    std::string getName() const override { return "Iceverb"; }
    std::string getVendor() const override { return "VR DAW"; }
    std::string getCategory() const override { return "Reverb"; }
    std::string getVersion() const override { return "1.0.0"; }

    // Plugin Lifecycle
    bool initialize() override;
    void shutdown() override;

    // Parameter Management
    std::vector<PluginParameter> getParameters() const override;
    void setParameter(const std::string& name, float value) override;
    float getParameter(const std::string& name) const override;
    void setParameterAutomated(const std::string& name, bool automated) override;
    bool isParameterAutomated(const std::string& name) const override;

    // Audio Processing
    void processAudio(float* buffer, unsigned long framesPerBuffer) override;

    // Preset Management
    void loadPreset(const std::string& presetName) override;
    void savePreset(const std::string& presetName) override;
    std::vector<std::string> getAvailablePresets() const override;

private:
    // Parameter
    float size;
    float decay;
    float diffusion;
    float modulation;
    float shimmer;
    float mix;
    float quality;

    // Automation Flags
    bool automatedSize;
    bool automatedDecay;
    bool automatedDiffusion;
    bool automatedModulation;
    bool automatedShimmer;
    bool automatedMix;
    bool automatedQuality;

    // State Variables
    struct DelayLine {
        std::deque<float> buffer;
        size_t writePos;
        size_t readPos;
        float time;
        float feedback;
        float modulation;
        float shimmer;
    };

    std::vector<DelayLine> delayLines;
    size_t bufferSize;
    float currentSize;
    float currentDecay;
    float currentDiffusion;
    float currentModulation;
    float currentShimmer;
    std::mt19937 rng;

    // Private Methods
    void initializeDelayLines();
    void updateDelayLines();
    void processDelayLine(DelayLine& line, float* buffer, unsigned long framesPerBuffer);
    void updateDelayTimes();
    void applyModulation(float* buffer, unsigned long framesPerBuffer);
    void applyShimmer(float* buffer, unsigned long framesPerBuffer);
    void applyDiffusion(float* buffer, unsigned long framesPerBuffer);
};

} // namespace VR_DAW 