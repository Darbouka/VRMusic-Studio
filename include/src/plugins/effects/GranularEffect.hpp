#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>
#include <deque>

namespace VR_DAW {

class GranularEffect : public EffectPlugin {
public:
    GranularEffect();
    ~GranularEffect();

    // Plugin-Identifikation
    std::string getName() const override { return "Granular"; }
    std::string getVendor() const override { return "VR DAW"; }
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
        float position;
        float length;
        float pitch;
        float pan;
        float volume;
        float age;
        bool active;
    };
    std::vector<Grain> grains;
    std::deque<float> buffer;
    unsigned long bufferSize;
    unsigned long writePos;
    float timeSinceLastGrain;

    // Hilfsmethoden
    void initializeGrains();
    void updateGrains();
    float processGrain(const Grain& grain);
    void addGrain();
    float getWindowValue(float position);
};

} // namespace VR_DAW 