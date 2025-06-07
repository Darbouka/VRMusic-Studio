#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <random>
#include <string>

namespace VR_DAW {

class NeuralVocoderEffect : public EffectPlugin {
public:
    NeuralVocoderEffect();
    ~NeuralVocoderEffect() override;
    
    // Plugin-Identifikation
    const char* getName() const override { return "NeuralVocoder"; }
    const char* getVendor() const override { return "VR_DAW"; }
    const char* getCategory() const override { return "Vocoder"; }
    const char* getVersion() const override { return "1.0.0"; }
    
    // Lebenszyklus-Management
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
    float carrier;
    float modulator;
    float bands;
    float learning;
    float adaptation;
    float mix;
    float quality;
    
    // Automatisierungs-Zustände
    bool automatedCarrier;
    bool automatedModulator;
    bool automatedBands;
    bool automatedLearning;
    bool automatedAdaptation;
    bool automatedMix;
    bool automatedQuality;
    
    // Interne Zustände
    struct BandState {
        float frequency;
        float bandwidth;
        float gain;
        float phase;
    };
    
    struct VocoderState {
        std::vector<BandState> bands;
        float carrier;
        float modulator;
    };
    
    std::vector<VocoderState> states;
    std::mt19937 rng;
    unsigned long bufferSize;
    
    // Hilfsmethoden
    void initializeStates();
    void updateStates();
    void processState(VocoderState& state, float* buffer, unsigned long framesPerBuffer);
    void applyCarrier(float* buffer, unsigned long framesPerBuffer);
    void applyModulator(float* buffer, unsigned long framesPerBuffer);
    void applyBands(float* buffer, unsigned long framesPerBuffer);
};

} // namespace VR_DAW 