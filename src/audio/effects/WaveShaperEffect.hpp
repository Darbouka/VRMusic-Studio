#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>

namespace VRMusicStudio {

class WaveShaperEffect : public EffectPlugin {
public:
    WaveShaperEffect();
    ~WaveShaperEffect() override;

    // Plugin Identifikation
    std::string getName() const override { return "Wave Shaper"; }
    std::string getVendor() const override { return "VRMusicStudio"; }
    std::string getCategory() const override { return "Distortion"; }
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
    float drive;
    float bias;
    float mix;
    float tone;
    float symmetry;
    float quality;

    // Automation States
    bool automatedDrive;
    bool automatedBias;
    bool automatedMix;
    bool automatedTone;
    bool automatedSymmetry;
    bool automatedQuality;

    // State Variables
    std::vector<float> driveBuffer;
    std::vector<float> biasBuffer;
    std::vector<float> mixBuffer;
    std::vector<float> toneBuffer;
    std::vector<float> symmetryBuffer;
    std::vector<float> qualityBuffer;

    // Current Values
    float currentDrive;
    float currentBias;
    float currentMix;
    float currentTone;
    float currentSymmetry;
    float currentQuality;

    // Processing Methods
    void updateParameters();
    void processDrive(float* buffer, unsigned long framesPerBuffer);
    void processBias(float* buffer, unsigned long framesPerBuffer);
    void processMix(float* buffer, unsigned long framesPerBuffer);
    void processTone(float* buffer, unsigned long framesPerBuffer);
    void processSymmetry(float* buffer, unsigned long framesPerBuffer);
    void processQuality(float* buffer, unsigned long framesPerBuffer);

    // Helper Methods
    void initializeBuffers();
    float calculateDriveCurve(float sample, float drive);
    float calculateBiasCurve(float sample, float bias);
    float calculateToneCurve(float sample, float tone);
    float calculateSymmetryCurve(float sample, float symmetry);
};

} // namespace VRMusicStudio 