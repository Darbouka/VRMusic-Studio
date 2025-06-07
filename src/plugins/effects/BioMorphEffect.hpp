#pragma once

#include "EffectPlugin.hpp"
#include <deque>
#include <vector>
#include <random>
#include <array>

namespace VR_DAW {

class BioMorphEffect : public EffectPlugin {
public:
    BioMorphEffect();
    ~BioMorphEffect();

    // Plugin Identifikation
    std::string getName() const override { return "BioMorph"; }
    std::string getVendor() const override { return "VR DAW"; }
    std::string getCategory() const override { return "Bio"; }
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
    float growth;
    float mutation;
    float evolution;
    float adaptation;
    float complexity;
    float mix;
    float quality;

    // Automation Flags
    bool automatedGrowth;
    bool automatedMutation;
    bool automatedEvolution;
    bool automatedAdaptation;
    bool automatedComplexity;
    bool automatedMix;
    bool automatedQuality;

    // Bio State
    struct Cell {
        std::array<float, 64> dna;
        float energy;
        float age;
        bool active;
    };

    struct Organism {
        std::vector<Cell> cells;
        float fitness;
        float adaptation;
        bool alive;
    };

    std::vector<Organism> organisms;
    size_t bufferSize;
    float currentGrowth;
    float currentMutation;
    float currentEvolution;
    float currentAdaptation;
    float currentComplexity;
    std::mt19937 rng;

    // Private Methods
    void initializeOrganisms();
    void updateOrganisms();
    void processOrganism(Organism& organism, float* buffer, unsigned long framesPerBuffer);
    void evolveOrganisms();
    void mutateOrganisms();
    void adaptOrganisms();
    float calculateFitness(const Organism& organism);
    void applyGrowth(float* buffer, unsigned long framesPerBuffer);
};

} // namespace VR_DAW 