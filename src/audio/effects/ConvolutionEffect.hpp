#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>
#include <complex>

namespace VRMusicStudio {

class ConvolutionEffect : public EffectPlugin {
public:
    ConvolutionEffect();
    ~ConvolutionEffect() override;

    // Plugin Identifikation
    std::string getName() const override { return "Convolution"; }
    std::string getVendor() const override { return "VRMusicStudio"; }
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

    // Impulse Response Management
    bool loadImpulseResponse(const std::string& filePath);
    void clearImpulseResponse();
    bool isImpulseResponseLoaded() const;

private:
    // Parameter
    float mix;
    float predelay;
    float decay;
    float damping;
    float width;
    float quality;

    // Automation States
    bool automatedMix;
    bool automatedPredelay;
    bool automatedDecay;
    bool automatedDamping;
    bool automatedWidth;
    bool automatedQuality;

    // State Variables
    std::vector<float> impulseResponse;
    std::vector<float> inputBuffer;
    std::vector<float> outputBuffer;
    std::vector<std::complex<float>> fftBuffer;
    std::vector<std::complex<float>> irFftBuffer;
    unsigned long fftSize;
    unsigned long blockSize;
    unsigned long overlap;
    unsigned long writePos;
    unsigned long readPos;
    bool irLoaded;

    // Current Values
    float currentMix;
    float currentPredelay;
    float currentDecay;
    float currentDamping;
    float currentWidth;
    float currentQuality;

    // Processing Methods
    void updateParameters();
    void processConvolution(float* buffer, unsigned long framesPerBuffer);
    void applyPredelay(float* buffer, unsigned long framesPerBuffer);
    void applyDecay(float* buffer, unsigned long framesPerBuffer);
    void applyDamping(float* buffer, unsigned long framesPerBuffer);
    void applyWidth(float* buffer, unsigned long framesPerBuffer);
    void applyMix(float* buffer, unsigned long framesPerBuffer);
    void applyQuality(float* buffer, unsigned long framesPerBuffer);

    // FFT Methods
    void initializeFFT();
    void performFFT(std::vector<std::complex<float>>& buffer);
    void performIFFT(std::vector<std::complex<float>>& buffer);
    void applyWindow(std::vector<float>& buffer);
    void overlapAdd(std::vector<float>& output, const std::vector<float>& input);

    // Helper Methods
    void initializeBuffers();
    void resizeBuffers(unsigned long size);
    float calculateDecayFactor(float decay, float sampleRate);
    float calculateDampingFactor(float damping, float frequency);
};

} // namespace VRMusicStudio 