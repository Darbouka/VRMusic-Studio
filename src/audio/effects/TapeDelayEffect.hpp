#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>
#include <deque>

namespace VRMusicStudio {

class TapeDelayEffect : public EffectPlugin {
public:
    TapeDelayEffect();
    ~TapeDelayEffect();

    // Plugin-Identifikation
    std::string getName() const override { return "Tape Delay"; }
    std::string getVendor() const override { return "VRMusicStudio"; }
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
    float time;        // 0.0 - 2000.0 ms
    float feedback;    // 0.0 - 0.9
    float mix;        // 0.0 - 1.0
    float wow;        // 0.0 - 1.0
    float flutter;    // 0.0 - 1.0
    float saturation; // 0.0 - 1.0
    float quality;    // 0.0 - 1.0

    // Automatisierungs-Flags
    bool automatedTime;
    bool automatedFeedback;
    bool automatedMix;
    bool automatedWow;
    bool automatedFlutter;
    bool automatedSaturation;
    bool automatedQuality;

    // Zustandsvariablen
    std::deque<float> buffer;
    unsigned long bufferSize;
    unsigned long writePos;
    unsigned long readPos;
    float wowPhase;
    float flutterPhase;
    float lastDelayTime;
    float currentTime;
    float currentFeedback;
    float currentMix;
    float currentWow;
    float currentFlutter;
    float currentSaturation;
    float currentQuality;

    // Hilfsfunktionen
    void updateParameters();
    void initializeBuffer();
    void processDelay(float* buffer, unsigned long framesPerBuffer);
    void applyWow(float* buffer, unsigned long framesPerBuffer);
    void applyFlutter(float* buffer, unsigned long framesPerBuffer);
    void applySaturation(float* buffer, unsigned long framesPerBuffer);
    void applyQuality(float* buffer, unsigned long framesPerBuffer);
    float calculateModulation(float phase, float amount);
};

} // namespace VRMusicStudio 