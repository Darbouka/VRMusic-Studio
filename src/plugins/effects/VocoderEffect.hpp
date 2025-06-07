#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>

namespace VR_DAW {

class VocoderEffect : public EffectPlugin {
public:
    VocoderEffect();
    ~VocoderEffect();

    // Plugin-Identifikation
    std::string getName() const override { return "Vocoder"; }
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
    float bands;       // 4.0 - 32.0
    float q;          // 0.1 - 10.0
    float attack;     // 0.1 - 100.0 ms
    float release;    // 10.0 - 1000.0 ms
    float mix;        // 0.0 - 1.0
    float level;      // 0.0 - 1.0
    float quality;    // 0.0 - 1.0

    // Automatisierungs-Flags
    bool automatedBands;
    bool automatedQ;
    bool automatedAttack;
    bool automatedRelease;
    bool automatedMix;
    bool automatedLevel;
    bool automatedQuality;

    // Zustandsvariablen
    struct Band {
        float centerFreq;
        float bandwidth;
        float attackCoeff;
        float releaseCoeff;
        float envelope;
        float carrierEnvelope;
        float modulatorEnvelope;
        std::vector<float> filterState;
    };
    std::vector<Band> bands;

    // Hilfsmethoden
    void initializeBands();
    void updateBandCoefficients();
    float processBand(const Band& band, float input, float carrier);
    void updateEnvelopes(Band& band, float input, float carrier);
};

} // namespace VR_DAW 