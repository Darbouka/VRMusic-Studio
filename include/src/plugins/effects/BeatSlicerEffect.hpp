#pragma once

#include "EffectPlugin.hpp"
#include <deque>
#include <vector>
#include <random>

namespace VR_DAW {

class BeatSlicerEffect : public EffectPlugin {
public:
    BeatSlicerEffect();
    ~BeatSlicerEffect();

    // Plugin Identifikation
    std::string getName() const override { return "BeatSlicer"; }
    std::string getVendor() const override { return "VR DAW"; }
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
    float sliceSize;
    float sliceCount;
    float sliceOffset;
    float sliceRandom;
    float sliceReverse;
    float mix;
    float quality;

    // Automation Flags
    bool automatedSliceSize;
    bool automatedSliceCount;
    bool automatedSliceOffset;
    bool automatedSliceRandom;
    bool automatedSliceReverse;
    bool automatedMix;
    bool automatedQuality;

    // State Variables
    struct Slice {
        std::deque<float> buffer;
        size_t startPos;
        size_t endPos;
        size_t currentPos;
        float level;
        bool reverse;
        bool active;
    };

    std::vector<Slice> slices;
    size_t bufferSize;
    float currentSliceSize;
    float currentSliceCount;
    float currentSliceOffset;
    float currentSliceRandom;
    float currentSliceReverse;
    std::mt19937 rng;

    // Private Methods
    void initializeSlices();
    void updateSlices();
    void processSlice(Slice& slice, float* buffer, unsigned long framesPerBuffer);
    void updateSlicePositions();
    void applySliceRandomization();
    void applySliceReversal();
};

} // namespace VR_DAW 