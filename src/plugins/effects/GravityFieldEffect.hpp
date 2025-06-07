#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <random>
#include <string>

namespace VR_DAW {

class GravityFieldEffect : public EffectPlugin {
public:
    GravityFieldEffect();
    ~GravityFieldEffect() override;
    
    // Plugin-Identifikation
    const char* getName() const override { return "GravityField"; }
    const char* getVendor() const override { return "VR_DAW"; }
    const char* getCategory() const override { return "Modulation"; }
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
    float strength;
    float radius;
    float decay;
    float chaos;
    float dimension;
    float mix;
    float quality;
    
    // Automatisierungs-Zustände
    bool automatedStrength;
    bool automatedRadius;
    bool automatedDecay;
    bool automatedChaos;
    bool automatedDimension;
    bool automatedMix;
    bool automatedQuality;
    
    // Interne Zustände
    struct Particle {
        float position;
        float velocity;
        float mass;
        float charge;
    };
    
    struct Field {
        std::vector<Particle> particles;
        float dimension;
    };
    
    std::vector<Field> fields;
    std::mt19937 rng;
    unsigned long bufferSize;
    
    // Hilfsmethoden
    void initializeFields();
    void updateFields();
    void processField(Field& field, float* buffer, unsigned long framesPerBuffer);
    void applyStrength(float* buffer, unsigned long framesPerBuffer);
    void applyRadius(float* buffer, unsigned long framesPerBuffer);
    void applyDecay(float* buffer, unsigned long framesPerBuffer);
};

} // namespace VR_DAW 