#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>
#include <deque>

namespace VR_DAW {

class VoiceVocoderEffect : public EffectPlugin {
public:
    VoiceVocoderEffect();
    ~VoiceVocoderEffect();

    // Plugin-Identifikation
    std::string getName() const override { return "Voice Vocoder"; }
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

    // KI-Stimmen
    void setVoicePreset(const std::string& voiceName);
    std::string getVoicePreset() const;
    std::vector<std::string> getAvailableVoices() const;

private:
    // Parameter
    float bands;         // 4 - 32
    float mix;           // 0.0 - 1.0
    float formant;       // 0.5 - 2.0
    float robotize;      // 0.0 - 1.0
    float quality;       // 0.0 - 1.0

    // KI-Stimmen
    std::string currentVoice;
    std::vector<std::string> availableVoices;

    // Automatisierungs-Flags
    bool automatedBands;
    bool automatedMix;
    bool automatedFormant;
    bool automatedRobotize;
    bool automatedQuality;

    // Filter- und Modulationszustand
    struct Band {
        float carrier;
        float modulator;
        float envelope;
    };
    std::vector<Band> bandsState;

    // Hilfsmethoden
    void initializeBands();
    void updateBands();
    void processBands(float* buffer, unsigned long framesPerBuffer);
    void applyVoicePreset(float* buffer, unsigned long framesPerBuffer);
};

} // namespace VR_DAW 