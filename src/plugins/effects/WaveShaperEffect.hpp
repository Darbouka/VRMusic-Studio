#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>

namespace VR_DAW {

class WaveShaperEffect : public EffectPlugin {
public:
    WaveShaperEffect();
    ~WaveShaperEffect();

    // Plugin-Identifikation
    std::string getName() const override { return "WaveShaper"; }
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
    float drive;       // 0.0 - 1.0
    float mix;         // 0.0 - 1.0
    float stereoWidth; // 0.0 - 1.0
    float symmetry;    // 0.0 - 1.0
    float bias;        // -1.0 - 1.0
    float oversample;  // 1.0 - 4.0
    float dcOffset;    // -1.0 - 1.0

    // Automation-Flags
    bool automatedDrive;
    bool automatedMix;
    bool automatedStereoWidth;
    bool automatedSymmetry;
    bool automatedBias;
    bool automatedOversample;
    bool automatedDcOffset;

    // Hilfsfunktionen
    float processSample(float input);
    float applyDrive(float input);
    float applySymmetry(float input);
    float applyBias(float input);
    float removeDcOffset(float input);
};

} // namespace VR_DAW 