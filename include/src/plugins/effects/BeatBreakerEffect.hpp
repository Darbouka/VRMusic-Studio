#pragma once

#include "EffectPlugin.hpp"
#include <vector>
#include <string>
#include <deque>
#include <random>

namespace VR_DAW {

class BeatBreakerEffect : public EffectPlugin {
public:
    BeatBreakerEffect();
    ~BeatBreakerEffect();

    // Plugin-Identifikation
    std::string getName() const override { return "Beat Breaker"; }
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
    float breakSize;      // 1.0 - 1000.0 ms
    float breakDensity;   // 0.1 - 100.0 breaks/sec
    float breakPattern;   // 0.0 - 1.0
    float breakReverse;   // 0.0 - 1.0
    float breakStutter;   // 0.0 - 1.0
    float mix;           // 0.0 - 1.0
    float quality;       // 0.0 - 1.0

    // Automatisierungs-Flags
    bool automatedBreakSize;
    bool automatedBreakDensity;
    bool automatedBreakPattern;
    bool automatedBreakReverse;
    bool automatedBreakStutter;
    bool automatedMix;
    bool automatedQuality;

    // Zustandsvariablen
    struct Break {
        std::deque<float> buffer;
        unsigned long size;
        bool reverse;
        bool stutter;
        float pattern;
        float envelope;
    };
    std::vector<Break> breaks;
    unsigned long currentBreak;
    unsigned long bufferSize;
    float sampleRate;
    std::mt19937 rng;

    // Hilfsmethoden
    void initializeBreaks();
    void updateBreaks();
    void processBreak(Break& break_, float* output, unsigned long frames);
    void generatePattern();
    void cleanupBreaks();
    float calculateEnvelope(float position, float size);
};

} // namespace VR_DAW 