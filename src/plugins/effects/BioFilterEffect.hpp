#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <random>
#include <string>

namespace VR_DAW {

class BioFilterEffect : public EffectPlugin {
public:
    BioFilterEffect();
    ~BioFilterEffect() override;
    
    // Plugin-Identifikation
    const char* getName() const override { return "BioFilter"; }
    const char* getVendor() const override { return "VR_DAW"; }
    const char* getCategory() const override { return "Filter"; }
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
    float frequency;
    float resonance;
    float mutation;
    float evolution;
    float adaptation;
    float mix;
    float quality;
    
    // Automatisierungs-Zustände
    bool automatedFrequency;
    bool automatedResonance;
    bool automatedMutation;
    bool automatedEvolution;
    bool automatedAdaptation;
    bool automatedMix;
    bool automatedQuality;
    
    // Interne Zustände
    struct Cell {
        float frequency;
        float resonance;
        float energy;
        float age;
    };
    
    struct Organism {
        std::vector<Cell> cells;
        float fitness;
    };
    
    std::vector<Organism> organisms;
    std::mt19937 rng;
    unsigned long bufferSize;
    
    // Hilfsmethoden
    void initializeOrganisms();
    void updateOrganisms();
    void processOrganism(Organism& organism, float* buffer, unsigned long framesPerBuffer);
    void evolveOrganisms();
    void mutateOrganisms();
    void adaptOrganisms();
    float calculateFitness(const Organism& organism) const;
};

} // namespace VR_DAW 