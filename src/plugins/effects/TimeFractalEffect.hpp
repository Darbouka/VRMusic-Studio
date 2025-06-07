#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <random>
#include <string>

namespace VR_DAW {

class TimeFractalEffect : public EffectPlugin {
public:
    TimeFractalEffect();
    ~TimeFractalEffect();
    
    // Plugin-Identifikation
    const char* getName() const override { return "TimeFractal"; }
    const char* getVendor() const override { return "VR_DAW"; }
    const char* getCategory() const override { return "Effect"; }
    const char* getVersion() const override { return "1.0.0"; }
    
    // Lifecycle
    bool initialize() override;
    void shutdown() override;
    
    // Parameter-Management
    std::vector<PluginParameter> getParameters() const override;
    void setParameter(const std::string& name, float value) override;
    float getParameter(const std::string& name) const override;
    void setParameterAutomated(const std::string& name, bool automated) override;
    bool isParameterAutomated(const std::string& name) const override;
    
    // Preset-Management
    void loadPreset(const std::string& presetName) override;
    void savePreset(const std::string& presetName) override;
    std::vector<std::string> getAvailablePresets() const override;
    
    // Audio-Verarbeitung
    void processAudio(float* buffer, unsigned long framesPerBuffer) override;
    
private:
    // Parameter
    float time;
    float depth;
    float feedback;
    float chaos;
    float dimension;
    float mix;
    float quality;
    
    // Automatisierung
    bool automatedTime;
    bool automatedDepth;
    bool automatedFeedback;
    bool automatedChaos;
    bool automatedDimension;
    bool automatedMix;
    bool automatedQuality;
    
    // Interne Zustände
    struct FractalState {
        float position;
        float phase;
        float amplitude;
        float frequency;
    };
    
    struct TimeState {
        std::vector<FractalState> fractals;
        float time;
        float depth;
    };
    
    std::vector<TimeState> states;
    std::mt19937 rng;
    unsigned long bufferSize;
    
    // Hilfsmethoden
    void initializeStates();
    void updateStates();
    void processState(TimeState& state, float* buffer, unsigned long framesPerBuffer);
    void applyTime(float* buffer, unsigned long framesPerBuffer);
    void applyDepth(float* buffer, unsigned long framesPerBuffer);
    void applyFeedback(float* buffer, unsigned long framesPerBuffer);
};

} // namespace VR_DAW 