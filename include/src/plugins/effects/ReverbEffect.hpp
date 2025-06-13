#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>

namespace VR_DAW {

class ReverbEffect : public EffectPlugin {
public:
    ReverbEffect();
    ~ReverbEffect() override;

    // Plugin-Identifikation
    const char* getName() const override { return "Reverb"; }
    const char* getVendor() const override { return "VR DAW"; }
    int getVersion() const override { return 1; }

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
    float roomSize;      // 0.0 - 1.0
    float damping;       // 0.0 - 1.0
    float width;         // 0.0 - 1.0
    float wetLevel;      // 0.0 - 1.0
    float dryLevel;      // 0.0 - 1.0
    float freeze;        // 0.0 - 1.0
    float mix;           // 0.0 - 1.0

    // Automatisierung
    bool automatedRoomSize;
    bool automatedDamping;
    bool automatedWidth;
    bool automatedWetLevel;
    bool automatedDryLevel;
    bool automatedFreeze;
    bool automatedMix;

    // Reverb-Strukturen
    struct CombFilter {
        std::vector<float> buffer;
        size_t position;
        float feedback;
        float damping;
        float dampInverse;
    };

    struct AllpassFilter {
        std::vector<float> buffer;
        size_t position;
        float feedback;
    };

    std::vector<CombFilter> combFilters;
    std::vector<AllpassFilter> allpassFilters;

    // Hilfsfunktionen
    void initializeFilters();
    float processCombFilter(float input, CombFilter& filter);
    float processAllpassFilter(float input, AllpassFilter& filter);
    void updateBuffers();
};

} // namespace VR_DAW 