#pragma once

#include "../PluginInterface.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace VR_DAW {

class DrumMachine : public InstrumentPlugin {
public:
    DrumMachine();
    ~DrumMachine();

    // Plugin Interface Implementation
    bool initialize() override;
    void shutdown() override;
    void update() override;
    std::vector<PluginParameter> getParameters() const override;
    void setParameter(const std::string& name, float value) override;
    float getParameter(const std::string& name) const override;
    void setParameterAutomated(const std::string& name, bool automated) override;
    bool isParameterAutomated(const std::string& name) const override;
    void processAudio(float* buffer, unsigned long framesPerBuffer) override;
    void processMidi(const std::vector<uint8_t>& midiData) override;

    // Drum Machine spezifische Funktionen
    void loadDrumKit(const std::string& path);
    void unloadDrumKit();
    void setDrumSample(int pad, const std::string& path);
    void setDrumVolume(int pad, float volume);
    void setDrumPan(int pad, float pan);
    void setDrumPitch(int pad, float pitch);
    void setDrumFilter(int pad, float cutoff, float resonance);
    void setDrumEnvelope(int pad, float attack, float decay, float sustain, float release);
    void setDrumLFO(int pad, float rate, float amount, const std::string& destination);
    void setDrumReverb(int pad, float amount);
    void setDrumDelay(int pad, float time, float feedback);
    void setDrumCompression(int pad, float threshold, float ratio);
    void setDrumEQ(int pad, float low, float mid, float high);
    void setDrumDistortion(int pad, float amount);
    void setDrumGranular(int pad, float grainSize, float density, float pitch);
    void setDrumTimeStretch(int pad, float rate);
    void setDrumReverse(int pad, bool reverse);
    void setDrumSlice(int pad, const std::vector<float>& slicePoints);
    void setDrumSliceMode(int pad, const std::string& mode);
    void setDrumSliceQuantize(int pad, float amount);
    void setDrumSliceRandom(int pad, float amount);
    void setDrumSliceReverse(int pad, bool reverse);
    void setDrumSlicePitch(int pad, float pitch);
    void setDrumSliceVolume(int pad, float volume);
    void setDrumSlicePan(int pad, float pan);
    void setDrumSliceFilter(int pad, float cutoff, float resonance);
    void setDrumSliceEnvelope(int pad, float attack, float decay, float sustain, float release);
    void setDrumSliceLFO(int pad, float rate, float amount, const std::string& destination);
    void setDrumSliceReverb(int pad, float amount);
    void setDrumSliceDelay(int pad, float time, float feedback);
    void setDrumSliceCompression(int pad, float threshold, float ratio);
    void setDrumSliceEQ(int pad, float low, float mid, float high);
    void setDrumSliceDistortion(int pad, float amount);
    void setDrumSliceGranular(int pad, float grainSize, float density, float pitch);
    void setDrumSliceTimeStretch(int pad, float rate);
    void setDrumSliceReverse(int pad, bool reverse);

    // Sequencer Funktionen
    void setBPM(float bpm);
    void setTimeSignature(int numerator, int denominator);
    void setStep(int pad, int step, bool active);
    void setStepVelocity(int pad, int step, int velocity);
    void setStepProbability(int pad, int step, float probability);
    void setStepCondition(int pad, int step, const std::string& condition);
    void setStepLength(int pad, int step, float length);
    void setStepPitch(int pad, int step, float pitch);
    void setStepPan(int pad, int step, float pan);
    void setStepFilter(int pad, int step, float cutoff, float resonance);
    void setStepEnvelope(int pad, int step, float attack, float decay, float sustain, float release);
    void setStepLFO(int pad, int step, float rate, float amount, const std::string& destination);
    void setStepReverb(int pad, int step, float amount);
    void setStepDelay(int pad, int step, float time, float feedback);
    void setStepCompression(int pad, int step, float threshold, float ratio);
    void setStepEQ(int pad, int step, float low, float mid, float high);
    void setStepDistortion(int pad, int step, float amount);
    void setStepGranular(int pad, int step, float grainSize, float density, float pitch);
    void setStepTimeStretch(int pad, int step, float rate);
    void setStepReverse(int pad, int step, bool reverse);
    void setStepSlice(int pad, int step, const std::vector<float>& slicePoints);
    void setStepSliceMode(int pad, int step, const std::string& mode);
    void setStepSliceQuantize(int pad, int step, float amount);
    void setStepSliceRandom(int pad, int step, float amount);
    void setStepSliceReverse(int pad, int step, bool reverse);
    void setStepSlicePitch(int pad, int step, float pitch);
    void setStepSliceVolume(int pad, int step, float volume);
    void setStepSlicePan(int pad, int step, float pan);
    void setStepSliceFilter(int pad, int step, float cutoff, float resonance);
    void setStepSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release);
    void setStepSliceLFO(int pad, int step, float rate, float amount, const std::string& destination);
    void setStepSliceReverb(int pad, int step, float amount);
    void setStepSliceDelay(int pad, int step, float time, float feedback);
    void setStepSliceCompression(int pad, int step, float threshold, float ratio);
    void setStepSliceEQ(int pad, int step, float low, float mid, float high);
    void setStepSliceDistortion(int pad, int step, float amount);
    void setStepSliceGranular(int pad, int step, float grainSize, float density, float pitch);
    void setStepSliceTimeStretch(int pad, int step, float rate);
    void setStepSliceReverse(int pad, int step, bool reverse);

private:
    struct DrumPad {
        std::vector<float> data;
        float sampleRate;
        int channels;
        float volume;
        float pan;
        float pitch;
        float filterCutoff;
        float filterResonance;
        float envelopeAttack;
        float envelopeDecay;
        float envelopeSustain;
        float envelopeRelease;
        float lfoRate;
        float lfoAmount;
        std::string lfoDestination;
        float reverbAmount;
        float delayTime;
        float delayFeedback;
        float compressionThreshold;
        float compressionRatio;
        float eqLow;
        float eqMid;
        float eqHigh;
        float distortionAmount;
        float granularGrainSize;
        float granularDensity;
        float granularPitch;
        float timeStretchRate;
        bool reverse;
        std::vector<float> slicePoints;
        std::string sliceMode;
        float sliceQuantize;
        float sliceRandom;
        bool sliceReverse;
        float slicePitch;
        float sliceVolume;
        float slicePan;
        float sliceFilterCutoff;
        float sliceFilterResonance;
        float sliceEnvelopeAttack;
        float sliceEnvelopeDecay;
        float sliceEnvelopeSustain;
        float sliceEnvelopeRelease;
        float sliceLfoRate;
        float sliceLfoAmount;
        std::string sliceLfoDestination;
        float sliceReverbAmount;
        float sliceDelayTime;
        float sliceDelayFeedback;
        float sliceCompressionThreshold;
        float sliceCompressionRatio;
        float sliceEqLow;
        float sliceEqMid;
        float sliceEqHigh;
        float sliceDistortionAmount;
        float sliceGranularGrainSize;
        float sliceGranularDensity;
        float sliceGranularPitch;
        float sliceTimeStretchRate;
        bool sliceReverse;
    };

    struct Step {
        bool active;
        int velocity;
        float probability;
        std::string condition;
        float length;
        float pitch;
        float pan;
        float filterCutoff;
        float filterResonance;
        float envelopeAttack;
        float envelopeDecay;
        float envelopeSustain;
        float envelopeRelease;
        float lfoRate;
        float lfoAmount;
        std::string lfoDestination;
        float reverbAmount;
        float delayTime;
        float delayFeedback;
        float compressionThreshold;
        float compressionRatio;
        float eqLow;
        float eqMid;
        float eqHigh;
        float distortionAmount;
        float granularGrainSize;
        float granularDensity;
        float granularPitch;
        float timeStretchRate;
        bool reverse;
        std::vector<float> slicePoints;
        std::string sliceMode;
        float sliceQuantize;
        float sliceRandom;
        bool sliceReverse;
        float slicePitch;
        float sliceVolume;
        float slicePan;
        float sliceFilterCutoff;
        float sliceFilterResonance;
        float sliceEnvelopeAttack;
        float sliceEnvelopeDecay;
        float sliceEnvelopeSustain;
        float sliceEnvelopeRelease;
        float sliceLfoRate;
        float sliceLfoAmount;
        std::string sliceLfoDestination;
        float sliceReverbAmount;
        float sliceDelayTime;
        float sliceDelayFeedback;
        float sliceCompressionThreshold;
        float sliceCompressionRatio;
        float sliceEqLow;
        float sliceEqMid;
        float sliceEqHigh;
        float sliceDistortionAmount;
        float sliceGranularGrainSize;
        float sliceGranularDensity;
        float sliceGranularPitch;
        float sliceTimeStretchRate;
        bool sliceReverse;
    };

    struct Note {
        int pad;
        int velocity;
        float amplitude;
        float position;
        float filterEnvelope;
        float ampEnvelope;
        bool active;
        int currentSlice;
        float slicePosition;
        float sliceFilterEnvelope;
        float sliceAmpEnvelope;
        bool sliceActive;
    };

    std::map<int, DrumPad> pads;
    std::map<int, std::vector<Step>> steps;
    std::map<int, Note> activeNotes;
    float bpm;
    int timeSignatureNumerator;
    int timeSignatureDenominator;
    float currentStep;
    float stepLength;
    float pitchBend;
    float modulation;
    float aftertouch;

    void noteOn(int pad, int velocity);
    void noteOff(int pad);
    float processDrumPad(const DrumPad& pad, float position);
    float processFilter(float input, float cutoff, float resonance);
    float processEnvelope(float input, float attack, float decay, float sustain, float release, float time);
    float processLFO(float rate, float amount, const std::string& destination, float time);
    float processReverb(float input, float amount);
    float processDelay(float input, float time, float feedback);
    float processCompression(float input, float threshold, float ratio);
    float processEQ(float input, float low, float mid, float high);
    float processDistortion(float input, float amount);
    float processGranular(float input, float grainSize, float density, float pitch);
    float processTimeStretch(float input, float rate);
    float processReverse(float input);
    float processSlice(const DrumPad& pad, int slice, float position);
    float processSliceFilter(float input, float cutoff, float resonance);
    float processSliceEnvelope(float input, float attack, float decay, float sustain, float release, float time);
    float processSliceLFO(float rate, float amount, const std::string& destination, float time);
    float processSliceReverb(float input, float amount);
    float processSliceDelay(float input, float time, float feedback);
    float processSliceCompression(float input, float threshold, float ratio);
    float processSliceEQ(float input, float low, float mid, float high);
    float processSliceDistortion(float input, float amount);
    float processSliceGranular(float input, float grainSize, float density, float pitch);
    float processSliceTimeStretch(float input, float rate);
    float processSliceReverse(float input);
};

} // namespace VR_DAW 