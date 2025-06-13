#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>

namespace VRMusicStudio {

class VoiceEditorEffect : public EffectPlugin {
public:
    VoiceEditorEffect();
    ~VoiceEditorEffect() override;

    // Plugin Identifikation
    std::string getName() const override { return "Voice Editor"; }
    std::string getVendor() const override { return "VRMusicStudio"; }
    std::string getCategory() const override { return "Voice"; }
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
    float pitch;
    float formant;
    float gender;
    float breathiness;
    float clarity;
    float mix;
    float quality;

    // Automation States
    bool automatedPitch;
    bool automatedFormant;
    bool automatedGender;
    bool automatedBreathiness;
    bool automatedClarity;
    bool automatedMix;
    bool automatedQuality;

    // State Variables
    std::vector<float> pitchBuffer;
    std::vector<float> formantBuffer;
    std::vector<float> genderBuffer;
    std::vector<float> breathinessBuffer;
    std::vector<float> clarityBuffer;
    std::vector<float> mixBuffer;
    std::vector<float> qualityBuffer;

    // Current Values
    float currentPitch;
    float currentFormant;
    float currentGender;
    float currentBreathiness;
    float currentClarity;
    float currentMix;
    float currentQuality;

    // Processing Methods
    void updateParameters();
    void processPitch(float* buffer, unsigned long framesPerBuffer);
    void processFormant(float* buffer, unsigned long framesPerBuffer);
    void processGender(float* buffer, unsigned long framesPerBuffer);
    void processBreathiness(float* buffer, unsigned long framesPerBuffer);
    void processClarity(float* buffer, unsigned long framesPerBuffer);
    void processMix(float* buffer, unsigned long framesPerBuffer);
    void processQuality(float* buffer, unsigned long framesPerBuffer);

    // Helper Methods
    void initializeBuffers();
    float calculatePitchShift(float sample, float shift);
    float calculateFormantShift(float sample, float shift);
    float calculateGenderShift(float sample, float shift);
    float calculateBreathiness(float sample, float amount);
    float calculateClarity(float sample, float amount);
};

} // namespace VRMusicStudio 