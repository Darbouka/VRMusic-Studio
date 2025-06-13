#pragma once

#include "audio/effects/EffectBase.hpp"

namespace VRMusicStudio {
namespace Audio {

class RingModulatorEffect : public EffectBase {
public:
    RingModulatorEffect();
    ~RingModulatorEffect() override;

    // Plugin identification
    std::string getName() const override { return "Ring Modulator"; }
    std::string getVendor() const override { return "VRMusicStudio"; }
    std::string getCategory() const override { return "Modulation"; }
    std::string getVersion() const override { return "1.0.0"; }

    // Lifecycle management
    bool initialize() override;
    void shutdown() override;

    // Parameter management
    std::vector<std::string> getParameters() const override;
    bool setParameter(const std::string& name, float value) override;
    float getParameter(const std::string& name) const override;

    // Parameter automation
    void setParameterAutomated(const std::string& name, bool automated) override;
    bool isParameterAutomated(const std::string& name) const override;

    // Audio processing
    void processAudio(float* input, float* output, int numFrames, int numChannels) override;

    // Preset management
    bool loadPreset(const std::string& name) override;
    bool savePreset(const std::string& name) override;
    std::vector<std::string> getAvailablePresets() const override;

private:
    // Parameters
    float frequency = 440.0f;
    float depth = 0.5f;
    float stereoWidth = 0.0f;
    float syncRate = 0.0f;
    float shape = 0.0f;

    // Processing
    void processFrequency(float* input, float* output, int numFrames);
    void processDepth(float* input, float* output, int numFrames);
    void processStereoWidth(float* input, float* output, int numFrames);
    void processSyncRate(float* input, float* output, int numFrames);
    void processShape(float* input, float* output, int numFrames);
    void processQuality(float* input, float* output, int numFrames);

    // Helper methods
    float calculateModulation(float input, float phase);
    float calculateSync(float input, float rate);
    float calculateShape(float input, float shape);
};

} // namespace Audio
} // namespace VRMusicStudio 