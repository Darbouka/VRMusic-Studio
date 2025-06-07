#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>
#include <map>

namespace VR_DAW {

class VoiceEditor : public EffectPlugin {
public:
    VoiceEditor();
    ~VoiceEditor();

    // Plugin-Identifikation
    std::string getName() const override { return "Voice Editor"; }
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

    // KI-Stimmen-Management
    void setVoice(const std::string& voiceName);
    std::string getCurrentVoice() const;
    std::vector<std::string> getAvailableVoices() const;
    void applyVoiceSettings(const std::string& voiceName);

private:
    // Parameter
    float pitch;          // 0.5 - 2.0
    float formant;        // 0.5 - 2.0
    float breathiness;    // 0.0 - 1.0
    float clarity;        // 0.0 - 1.0
    float warmth;         // 0.0 - 1.0
    float presence;       // 0.0 - 1.0
    float mix;           // 0.0 - 1.0
    float quality;       // 0.0 - 1.0

    // Automatisierungs-Flags
    bool automatedPitch;
    bool automatedFormant;
    bool automatedBreathiness;
    bool automatedClarity;
    bool automatedWarmth;
    bool automatedPresence;
    bool automatedMix;
    bool automatedQuality;

    // KI-Stimmen
    std::string currentVoice;
    std::vector<std::string> availableVoices;
    std::map<std::string, std::map<std::string, float>> voiceSettings;

    // Hilfsmethoden
    void initializeVoiceSettings();
    void updateVoiceSettings();
    void processVoice(float* buffer, unsigned long framesPerBuffer);
    void applyEQSettings(float* buffer, unsigned long framesPerBuffer);
    void applyFormantSettings(float* buffer, unsigned long framesPerBuffer);
};

} // namespace VR_DAW 