#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>
#include <deque>

namespace VRMusicStudio {

class GranularEffect : public EffectPlugin {
public:
    GranularEffect();
    ~GranularEffect();

    // Plugin-Identifikation
    std::string getName() const override { return "Granular"; }
    std::string getVendor() const override { return "VRMusicStudio"; }
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

private:
    // Parameter
    float grainSize;    // 1.0 - 1000.0 ms
    float density;      // 1.0 - 100.0 grains/sec
    float pitch;        // 0.25 - 4.0
    float spread;       // 0.0 - 1.0
    float mix;         // 0.0 - 1.0
    float feedback;    // 0.0 - 0.9
    float quality;     // 0.0 - 1.0

    // Automatisierungs-Flags
    bool automatedGrainSize;
    bool automatedDensity;
    bool automatedPitch;
    bool automatedSpread;
    bool automatedMix;
    bool automatedFeedback;
    bool automatedQuality;

    // Zustandsvariablen
    struct Grain {
        std::vector<float> buffer;
        size_t position;
        float pitch;
        float volume;
        float pan;
        bool active;
    };

    std::vector<Grain> grains;
    std::deque<float> inputBuffer;
    size_t bufferSize;
    float currentGrainSize;
    float currentDensity;
    float currentPitch;
    float currentSpread;
    float currentMix;
    float currentFeedback;
    float currentQuality;
    float grainTimer;
    float densityTimer;

    // Hilfsfunktionen
    void updateParameters();
    void initializeGrains();
    void updateGrains();
    void processGrains(float* buffer, unsigned long framesPerBuffer);
    void createGrain();
    void destroyGrain(size_t index);
    float calculateGrainEnvelope(float position, float size);
    float calculateGrainPitch();
    float calculateGrainPan();
};

} // namespace VRMusicStudio 