#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <glm/glm.hpp>
#include <torch/torch.h>

namespace VRMusicStudio {

class Synthesizers {
public:
    // Analog-Synthesizer
    struct AnalogSynth {
        float oscillator1;
        float oscillator2;
        float oscillator3;
        float filterCutoff;
        float filterResonance;
        float envelopeAttack;
        float envelopeDecay;
        float envelopeSustain;
        float envelopeRelease;
        float lfoRate;
        float lfoDepth;
        float reverb;
        float delay;
        bool enabled;
    };

    // FM-Synthesizer
    struct FMSynth {
        float carrierFrequency;
        float modulatorFrequency;
        float modulationIndex;
        float feedback;
        float algorithm;
        float envelopeAttack;
        float envelopeDecay;
        float envelopeSustain;
        float envelopeRelease;
        float lfoRate;
        float lfoDepth;
        float reverb;
        float delay;
        bool enabled;
    };

    // Wavetable-Synthesizer
    struct WavetableSynth {
        std::vector<float> wavetable;
        float position;
        float interpolation;
        float filterCutoff;
        float filterResonance;
        float envelopeAttack;
        float envelopeDecay;
        float envelopeSustain;
        float envelopeRelease;
        float lfoRate;
        float lfoDepth;
        float reverb;
        float delay;
        bool enabled;
    };

    // Granular-Synthesizer
    struct GranularSynth {
        float grainSize;
        float grainDensity;
        float grainPitch;
        float grainPan;
        float grainEnvelope;
        float filterCutoff;
        float filterResonance;
        float envelopeAttack;
        float envelopeDecay;
        float envelopeSustain;
        float envelopeRelease;
        float lfoRate;
        float lfoDepth;
        float reverb;
        float delay;
        bool enabled;
    };

    // Physical Modeling-Synthesizer
    struct PhysicalModelSynth {
        float excitation;
        float resonator;
        float damping;
        float coupling;
        float filterCutoff;
        float filterResonance;
        float envelopeAttack;
        float envelopeDecay;
        float envelopeSustain;
        float envelopeRelease;
        float lfoRate;
        float lfoDepth;
        float reverb;
        float delay;
        bool enabled;
    };

    Synthesizers();
    ~Synthesizers();

    // Analog-Synthesizer
    void setAnalogSynth(const AnalogSynth& params);
    void processAnalogSynth(float* buffer, int numSamples);

    // FM-Synthesizer
    void setFMSynth(const FMSynth& params);
    void processFMSynth(float* buffer, int numSamples);

    // Wavetable-Synthesizer
    void setWavetableSynth(const WavetableSynth& params);
    void processWavetableSynth(float* buffer, int numSamples);

    // Granular-Synthesizer
    void setGranularSynth(const GranularSynth& params);
    void processGranularSynth(float* buffer, int numSamples);

    // Physical Modeling-Synthesizer
    void setPhysicalModelSynth(const PhysicalModelSynth& params);
    void processPhysicalModelSynth(float* buffer, int numSamples);

    // Callback-Setter
    void setAnalogCallback(std::function<void(const std::vector<float>&)> callback);
    void setFMCallback(std::function<void(const std::vector<float>&)> callback);
    void setWavetableCallback(std::function<void(const std::vector<float>&)> callback);
    void setGranularCallback(std::function<void(const std::vector<float>&)> callback);
    void setPhysicalModelCallback(std::function<void(const std::vector<float>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; 