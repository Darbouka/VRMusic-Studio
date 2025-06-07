#pragma once

#include "../PluginInterface.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace VR_DAW {

class Sampler : public InstrumentPlugin {
public:
    Sampler();
    ~Sampler();

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

    // Sampler-spezifische Funktionen
    void loadSample(const std::string& path, int note);
    void unloadSample(int note);
    void setSampleLoop(int note, bool loop);
    void setSampleLoopPoints(int note, float start, float end);
    void setSamplePitch(int note, float pitch);
    void setSampleVolume(int note, float volume);
    void setSamplePan(int note, float pan);
    void setSampleFilter(int note, float cutoff, float resonance);
    void setSampleEnvelope(int note, float attack, float decay, float sustain, float release);
    void setSampleLFO(int note, float rate, float amount, const std::string& destination);
    void setSampleReverb(int note, float amount);
    void setSampleDelay(int note, float time, float feedback);
    void setSampleCompression(int note, float threshold, float ratio);
    void setSampleEQ(int note, float low, float mid, float high);
    void setSampleDistortion(int note, float amount);
    void setSampleGranular(int note, float grainSize, float density, float pitch);
    void setSampleTimeStretch(int note, float rate);
    void setSampleReverse(int note, bool reverse);
    void setSampleSlice(int note, const std::vector<float>& slicePoints);
    void setSampleSliceMode(int note, const std::string& mode);
    void setSampleSliceQuantize(int note, float amount);
    void setSampleSliceRandom(int note, float amount);
    void setSampleSliceReverse(int note, bool reverse);
    void setSampleSlicePitch(int note, float pitch);
    void setSampleSliceVolume(int note, float volume);
    void setSampleSlicePan(int note, float pan);
    void setSampleSliceFilter(int note, float cutoff, float resonance);
    void setSampleSliceEnvelope(int note, float attack, float decay, float sustain, float release);
    void setSampleSliceLFO(int note, float rate, float amount, const std::string& destination);
    void setSampleSliceReverb(int note, float amount);
    void setSampleSliceDelay(int note, float time, float feedback);
    void setSampleSliceCompression(int note, float threshold, float ratio);
    void setSampleSliceEQ(int note, float low, float mid, float high);
    void setSampleSliceDistortion(int note, float amount);
    void setSampleSliceGranular(int note, float grainSize, float density, float pitch);
    void setSampleSliceTimeStretch(int note, float rate);
    void setSampleSliceReverse(int note, bool reverse);

private:
    struct Sample {
        std::vector<float> data;
        float sampleRate;
        int channels;
        bool loop;
        float loopStart;
        float loopEnd;
        float pitch;
        float volume;
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
        int note;
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

    std::map<int, Sample> samples;
    std::map<int, Note> activeNotes;
    float pitchBend;
    float modulation;
    float aftertouch;

    void noteOn(int note, int velocity);
    void noteOff(int note);
    float processSample(const Sample& sample, float position);
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
    float processSlice(const Sample& sample, int slice, float position);
    float processSliceFilter(float input, float cutoff, float resonance);
    float processSliceEnvelope(float input, float attack, float decay, float sustain, float release, float time);
    float processSliceLFO(float rate, float amount, const std::string& destination, float time);
    float processSliceReverb(float input, float amount);
    float processSliceDelay(float input, float time, float feedback);
    float processSliceCompression(float input, float threshold, float ratio);
    float processSliceEQ(float input, float low, float mid, float high);
    float processSliceDistortion(float input, float amount);
    float processSliceTimeStretch(float input, float rate);
    float processSliceReverse(float input);
};

} // namespace VR_DAW 