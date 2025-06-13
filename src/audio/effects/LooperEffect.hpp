#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>
#include <deque>

namespace VRMusicStudio {

class LooperEffect : public EffectPlugin {
public:
    LooperEffect();
    ~LooperEffect() override;

    // Plugin Identifikation
    std::string getName() const override { return "Looper"; }
    std::string getVendor() const override { return "VRMusicStudio"; }
    std::string getCategory() const override { return "Loop"; }
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

    // Looper Control
    void startRecording();
    void stopRecording();
    void startPlayback();
    void stopPlayback();
    void clearLoop();
    void setLoopLength(float seconds);
    float getLoopLength() const;
    bool isRecording() const;
    bool isPlaying() const;

private:
    // Parameter
    float mix;
    float feedback;
    float reverse;
    float speed;
    float quality;

    // Automation States
    bool automatedMix;
    bool automatedFeedback;
    bool automatedReverse;
    bool automatedSpeed;
    bool automatedQuality;

    // State Variables
    std::deque<float> loopBuffer;
    unsigned long bufferSize;
    unsigned long writePos;
    unsigned long readPos;
    bool recording;
    bool playing;
    float loopLength;
    float currentSpeed;
    float currentReverse;
    float currentMix;
    float currentFeedback;
    float currentQuality;

    // Processing Methods
    void updateParameters();
    void processLoop(float* buffer, unsigned long framesPerBuffer);
    void applySpeed(float* buffer, unsigned long framesPerBuffer);
    void applyReverse(float* buffer, unsigned long framesPerBuffer);
    void applyMix(float* buffer, unsigned long framesPerBuffer);
    void applyFeedback(float* buffer, unsigned long framesPerBuffer);
    void applyQuality(float* buffer, unsigned long framesPerBuffer);

    // Helper Methods
    void initializeBuffer();
    void resizeBuffer(float seconds);
    float interpolateSample(const std::deque<float>& buffer, float position);
};

} // namespace VRMusicStudio 