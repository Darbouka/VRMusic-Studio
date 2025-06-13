#pragma once

#include "EffectPlugin.hpp"
#include <deque>
#include <vector>
#include <random>

namespace VRMusicStudio {

class BeatGrinderEffect : public EffectPlugin {
public:
    BeatGrinderEffect();
    ~BeatGrinderEffect();

    // Plugin Identifikation
    std::string getName() const override { return "BeatGrinder"; }
    std::string getVendor() const override { return "VRMusicStudio"; }
    std::string getCategory() const override { return "Beat"; }
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
    float gridSize;
    float swing;
    float shuffle;
    float stutter;
    float reverse;
    float mix;
    float quality;

    // Automation Flags
    bool automatedGridSize;
    bool automatedSwing;
    bool automatedShuffle;
    bool automatedStutter;
    bool automatedReverse;
    bool automatedMix;
    bool automatedQuality;

    // State Variables
    struct Grid {
        std::deque<float> buffer;
        size_t writePos;
        size_t readPos;
        float time;
        float level;
        bool active;
        bool reverse;
    };

    std::vector<Grid> grids;
    size_t bufferSize;
    float currentGridSize;
    float currentSwing;
    float currentShuffle;
    float currentStutter;
    float currentReverse;
    float currentMix;
    float currentQuality;

    // Helper Functions
    void initializeGrids();
    void updateGrids();
    void processGrids(float* buffer, unsigned long framesPerBuffer);
    void applySwing(float* buffer, unsigned long framesPerBuffer);
    void applyShuffle(float* buffer, unsigned long framesPerBuffer);
    void applyStutter(float* buffer, unsigned long framesPerBuffer);
    void applyReverse(float* buffer, unsigned long framesPerBuffer);
    void applyMix(float* buffer, unsigned long framesPerBuffer);
    void applyQuality(float* buffer, unsigned long framesPerBuffer);
};

} // namespace VRMusicStudio 