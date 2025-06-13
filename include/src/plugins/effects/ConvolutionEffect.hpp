#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>

namespace VR_DAW {

class ConvolutionEffect : public EffectPlugin {
public:
    ConvolutionEffect();
    ~ConvolutionEffect();

    // Plugin-Identifikation
    std::string getName() const override { return "Convolution"; }
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
    float mix;          // 0.0 - 1.0
    float stereoWidth;  // 0.0 - 1.0
    float gain;         // 0.0 - 2.0
    float predelay;     // 0.0 - 1.0
    float damping;      // 0.0 - 1.0
    float size;         // 0.0 - 1.0
    float diffusion;    // 0.0 - 1.0

    // Automation-Flags
    bool automatedMix;
    bool automatedStereoWidth;
    bool automatedGain;
    bool automatedPredelay;
    bool automatedDamping;
    bool automatedSize;
    bool automatedDiffusion;

    // Impulsantwort
    std::vector<float> impulseResponse;
    size_t irLength;
    size_t blockSize;
    std::vector<float> inputBuffer;
    std::vector<float> outputBuffer;
    size_t bufferPos;

    // Hilfsfunktionen
    void loadImpulseResponse(const std::string& name);
    void processBlock(float* input, float* output, size_t length);
    void updateBuffers();
};

} // namespace VR_DAW 