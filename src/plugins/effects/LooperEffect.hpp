#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>
#include <deque>

namespace VR_DAW {

class LooperEffect : public EffectPlugin {
public:
    LooperEffect();
    ~LooperEffect();

    // Plugin-Identifikation
    std::string getName() const override { return "Looper"; }
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
    float loopLength;   // 0.1 - 60.0 seconds
    float feedback;     // 0.0 - 0.9
    float mix;         // 0.0 - 1.0
    float speed;       // 0.25 - 4.0
    float reverse;     // 0.0 - 1.0
    float quality;     // 0.0 - 1.0

    // Automatisierungs-Flags
    bool automatedLoopLength;
    bool automatedFeedback;
    bool automatedMix;
    bool automatedSpeed;
    bool automatedReverse;
    bool automatedQuality;

    // Zustandsvariablen
    std::deque<float> buffer;
    unsigned long bufferSize;
    unsigned long writePos;
    unsigned long readPos;
    bool isRecording;
    bool isPlaying;
    float currentSpeed;
    float currentReverse;

    // Hilfsmethoden
    void initializeBuffer();
    void updateBuffer();
    float processSample(float input);
    void startRecording();
    void stopRecording();
    void startPlayback();
    void stopPlayback();
};

} // namespace VR_DAW 