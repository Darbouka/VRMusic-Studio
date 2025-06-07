#include "DrumMachine.hpp"
#include <spdlog/spdlog.h>
#include <random>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <sndfile.h>

namespace VR_DAW {

DrumMachine::DrumMachine() : bpm(120.0f), timeSignatureNumerator(4), timeSignatureDenominator(4), currentStep(0.0f), stepLength(0.25f), pitchBend(0.0f), modulation(0.0f), aftertouch(0.0f) {}

DrumMachine::~DrumMachine() { shutdown(); }

bool DrumMachine::initialize() {
    spdlog::info("DrumMachine initialisiert");
    return true;
}

void DrumMachine::shutdown() {
    activeNotes.clear();
    pads.clear();
    steps.clear();
}

void DrumMachine::update() {
    // Sequencer-Logik: Schrittvorschub (z.B. bei jedem Takt)
    // Dies ist eine vereinfachte Version
    currentStep += 1.0f;
    if (currentStep >= 16.0f) currentStep = 0.0f;
}

std::vector<PluginParameter> DrumMachine::getParameters() const {
    // Platzhalter: Parameter können später ergänzt werden
    return {};
}

void DrumMachine::setParameter(const std::string& name, float value) {}
float DrumMachine::getParameter(const std::string& name) const { return 0.0f; }
void DrumMachine::setParameterAutomated(const std::string& name, bool automated) {}
bool DrumMachine::isParameterAutomated(const std::string& name) const { return false; }

void DrumMachine::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float sample = 0.0f;
        // Aktive Noten (Pads) abspielen
        for (auto& [pad, note] : activeNotes) {
            if (note.active) {
                auto it = pads.find(pad);
                if (it != pads.end()) {
                    const auto& padData = it->second;
                    float position = note.position;
                    float padSample = processDrumPad(padData, position);
                    sample += padSample * note.amplitude;
                    note.position += 1.0f / padData.sampleRate;
                }
            }
        }
        buffer[i] = sample;
    }
}

void DrumMachine::processMidi(const std::vector<uint8_t>& midiData) {
    if (midiData.empty()) return;
    uint8_t status = midiData[0];
    uint8_t pad = midiData[1];
    uint8_t velocity = midiData.size() > 2 ? midiData[2] : 0;
    if ((status & 0xF0) == 0x90) { // Note On
        noteOn(pad, velocity);
    } else if ((status & 0xF0) == 0x80) { // Note Off
        noteOff(pad);
    }
}

void DrumMachine::noteOn(int pad, int velocity) {
    Note newNote;
    newNote.pad = pad;
    newNote.velocity = velocity;
    newNote.amplitude = velocity / 127.0f;
    newNote.position = 0.0f;
    newNote.active = true;
    activeNotes[pad] = newNote;
}

void DrumMachine::noteOff(int pad) {
    auto it = activeNotes.find(pad);
    if (it != activeNotes.end()) {
        it->second.active = false;
    }
}

float DrumMachine::processDrumPad(const DrumPad& pad, float position) {
    if (pad.data.empty()) return 0.0f;
    float sampleLength = static_cast<float>(pad.data.size()) / pad.channels;
    float samplePosition = position * pad.sampleRate;
    int index = static_cast<int>(samplePosition);
    float fraction = samplePosition - index;
    if (index + 1 < pad.data.size()) {
        return pad.data[index] * (1.0f - fraction) + pad.data[index + 1] * fraction;
    }
    return pad.data[index];
}

void DrumMachine::setDrumSample(int pad, const std::string& path) {
    SF_INFO fileInfo;
    SNDFILE* file = sf_open(path.c_str(), SFM_READ, &fileInfo);
    if (!file) {
        spdlog::error("Konnte Drum-Sample nicht laden: {}", path);
        return;
    }
    DrumPad drumPad;
    drumPad.data.resize(fileInfo.frames * fileInfo.channels);
    sf_readf_float(file, drumPad.data.data(), fileInfo.frames);
    drumPad.sampleRate = fileInfo.samplerate;
    drumPad.channels = fileInfo.channels;
    drumPad.volume = 1.0f;
    drumPad.pan = 0.0f;
    drumPad.pitch = 0.0f;
    pads[pad] = drumPad;
    sf_close(file);
}

void DrumMachine::unloadDrumKit() {
    pads.clear();
}

void DrumMachine::loadDrumKit(const std::string& path) {
    // Platzhalter: Drumkit-Ladefunktion kann später erweitert werden
}

float DrumMachine::processFilter(float input, float cutoff, float resonance) {
    // Einfacher IIR-Filter
    static float lastInput = 0.0f;
    static float lastOutput = 0.0f;
    
    float c = std::pow(2.0f, (cutoff - 20000.0f) / 20000.0f);
    float r = resonance * 0.5f;
    
    float output = input * (1.0f - c) + lastOutput * c;
    output = output * (1.0f - r) + lastInput * r;
    
    lastInput = input;
    lastOutput = output;
    
    return output;
}

float DrumMachine::processEnvelope(float input, float attack, float decay, float sustain, float release, float time) {
    static float envelope = 0.0f;
    static float lastTime = 0.0f;
    
    float deltaTime = time - lastTime;
    lastTime = time;
    
    if (time < attack) {
        envelope = time / attack;
    } else if (time < attack + decay) {
        envelope = 1.0f - (1.0f - sustain) * (time - attack) / decay;
    } else if (time < attack + decay + release) {
        envelope = sustain * (1.0f - (time - attack - decay) / release);
    } else {
        envelope = 0.0f;
    }
    
    return input * envelope;
}

float DrumMachine::processLFO(float rate, float amount, const std::string& destination, float time) {
    float phase = fmod(rate * time, 1.0f);
    float lfoValue = std::sin(2.0f * M_PI * phase) * amount;
    
    if (destination == "filter") {
        return 1.0f + lfoValue;
    } else if (destination == "amplitude") {
        return 1.0f + lfoValue;
    } else if (destination == "pitch") {
        return 1.0f + lfoValue;
    }
    
    return 1.0f;
}

float DrumMachine::processReverb(float input, float amount) {
    static const int delayLength = 44100; // 1 Sekunde bei 44.1kHz
    static std::vector<float> delayBuffer(delayLength, 0.0f);
    static int writeIndex = 0;
    
    float output = input;
    float feedback = 0.8f * amount;
    
    // Einfacher Reverb mit verzögertem Feedback
    output += delayBuffer[writeIndex] * feedback;
    delayBuffer[writeIndex] = output;
    writeIndex = (writeIndex + 1) % delayLength;
    
    return output;
}

float DrumMachine::processDelay(float input, float time, float feedback) {
    static const int maxDelayLength = 44100 * 2; // 2 Sekunden bei 44.1kHz
    static std::vector<float> delayBuffer(maxDelayLength, 0.0f);
    static int writeIndex = 0;
    
    int delaySamples = static_cast<int>(time * 44100.0f);
    int readIndex = (writeIndex - delaySamples + maxDelayLength) % maxDelayLength;
    
    float output = input + delayBuffer[readIndex] * feedback;
    delayBuffer[writeIndex] = output;
    writeIndex = (writeIndex + 1) % maxDelayLength;
    
    return output;
}

float DrumMachine::processCompression(float input, float threshold, float ratio) {
    float level = std::abs(input);
    if (level > threshold) {
        float gain = threshold + (level - threshold) / ratio;
        return input * (gain / level);
    }
    return input;
}

float DrumMachine::processEQ(float input, float low, float mid, float high) {
    // Einfacher 3-Band EQ
    static float lastLow = 0.0f;
    static float lastMid = 0.0f;
    static float lastHigh = 0.0f;
    
    float lowFreq = 200.0f;
    float highFreq = 2000.0f;
    
    // Low-Pass
    float lowOutput = input * (lowFreq / 44100.0f) + lastLow * (1.0f - lowFreq / 44100.0f);
    lastLow = lowOutput;
    
    // Band-Pass
    float midOutput = input - lowOutput - lastHigh;
    lastMid = midOutput;
    
    // High-Pass
    float highOutput = input * (1.0f - highFreq / 44100.0f) - lastHigh * (1.0f - highFreq / 44100.0f);
    lastHigh = highOutput;
    
    return lowOutput * std::pow(2.0f, low) + midOutput * std::pow(2.0f, mid) + highOutput * std::pow(2.0f, high);
}

float DrumMachine::processDistortion(float input, float amount) {
    return std::tanh(input * (1.0f + amount * 10.0f));
}

float DrumMachine::processGranular(float input, float grainSize, float density, float pitch) {
    static std::vector<float> buffer(44100, 0.0f);
    static int writeIndex = 0;
    static float lastOutput = 0.0f;
    
    buffer[writeIndex] = input;
    writeIndex = (writeIndex + 1) % buffer.size();
    
    int grainLength = static_cast<int>(grainSize * 44100.0f);
    float output = 0.0f;
    
    for (int i = 0; i < density * 10; ++i) {
        int startPos = (writeIndex - grainLength + buffer.size()) % buffer.size();
        int readPos = static_cast<int>(startPos + i * pitch * grainLength);
        readPos = readPos % buffer.size();
        output += buffer[readPos];
    }
    
    return output / (density * 10);
}

float DrumMachine::processTimeStretch(float input, float rate) {
    static std::vector<float> buffer(44100, 0.0f);
    static int writeIndex = 0;
    static float readIndex = 0.0f;
    
    buffer[writeIndex] = input;
    writeIndex = (writeIndex + 1) % buffer.size();
    
    float output = buffer[static_cast<int>(readIndex)];
    readIndex = fmod(readIndex + rate, buffer.size());
    
    return output;
}

float DrumMachine::processReverse(float input) {
    static std::vector<float> buffer(44100, 0.0f);
    static int writeIndex = 0;
    static int readIndex = 0;
    
    buffer[writeIndex] = input;
    writeIndex = (writeIndex + 1) % buffer.size();
    
    float output = buffer[readIndex];
    readIndex = (readIndex + 1) % buffer.size();
    
    return output;
}

void DrumMachine::setBPM(float newBpm) {
    bpm = newBpm;
    stepLength = 60.0f / (bpm * timeSignatureDenominator / 4.0f);
}

void DrumMachine::setTimeSignature(int numerator, int denominator) {
    timeSignatureNumerator = numerator;
    timeSignatureDenominator = denominator;
    stepLength = 60.0f / (bpm * timeSignatureDenominator / 4.0f);
}

void DrumMachine::setStep(int pad, int step, bool active) {
    if (steps.find(pad) == steps.end()) {
        steps[pad] = std::vector<Step>(16);
    }
    if (step >= 0 && step < 16) {
        steps[pad][step].active = active;
    }
}

void DrumMachine::setStepVelocity(int pad, int step, int velocity) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].velocity = velocity;
    }
}

void DrumMachine::setStepProbability(int pad, int step, float probability) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].probability = probability;
    }
}

void DrumMachine::setStepCondition(int pad, int step, const std::string& condition) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].condition = condition;
    }
}

void DrumMachine::setStepLength(int pad, int step, float length) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].length = length;
    }
}

void DrumMachine::setStepPitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].pitch = pitch;
    }
}

void DrumMachine::setStepPan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].pan = pan;
    }
}

void DrumMachine::setStepFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].filterCutoff = cutoff;
        steps[pad][step].filterResonance = resonance;
    }
}

void DrumMachine::setStepEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].envelopeAttack = attack;
        steps[pad][step].envelopeDecay = decay;
        steps[pad][step].envelopeSustain = sustain;
        steps[pad][step].envelopeRelease = release;
    }
}

void DrumMachine::setStepLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].lfoRate = rate;
        steps[pad][step].lfoAmount = amount;
        steps[pad][step].lfoDestination = destination;
    }
}

void DrumMachine::setStepReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].reverbAmount = amount;
    }
}

void DrumMachine::setStepDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].delayTime = time;
        steps[pad][step].delayFeedback = feedback;
    }
}

void DrumMachine::setStepCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].compressionThreshold = threshold;
        steps[pad][step].compressionRatio = ratio;
    }
}

void DrumMachine::setStepEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].eqLow = low;
        steps[pad][step].eqMid = mid;
        steps[pad][step].eqHigh = high;
    }
}

void DrumMachine::setStepDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].distortionAmount = amount;
    }
}

void DrumMachine::setStepSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDistortion(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDistortionAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlice(int pad, int step, const std::vector<float>& slicePoints) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePoints = slicePoints;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceMode(int pad, int step, const std::string& mode) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceMode = mode;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceQuantize(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceQuantize = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceRandom(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceRandom = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverse(int pad, int step, bool reverse) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverse = reverse;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePitch(int pad, int step, float pitch) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePitch = pitch;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceVolume(int pad, int step, float volume) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceVolume = volume;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSlicePan(int pad, int step, float pan) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].slicePan = pan;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceFilter(int pad, int step, float cutoff, float resonance) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceFilterCutoff = cutoff;
        steps[pad][step].sliceFilterResonance = resonance;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEnvelope(int pad, int step, float attack, float decay, float sustain, float release) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEnvelopeAttack = attack;
        steps[pad][step].sliceEnvelopeDecay = decay;
        steps[pad][step].sliceEnvelopeSustain = sustain;
        steps[pad][step].sliceEnvelopeRelease = release;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceLFO(int pad, int step, float rate, float amount, const std::string& destination) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceLfoRate = rate;
        steps[pad][step].sliceLfoAmount = amount;
        steps[pad][step].sliceLfoDestination = destination;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceReverb(int pad, int step, float amount) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceReverbAmount = amount;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceDelay(int pad, int step, float time, float feedback) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceDelayTime = time;
        steps[pad][step].sliceDelayFeedback = feedback;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceCompression(int pad, int step, float threshold, float ratio) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceCompressionThreshold = threshold;
        steps[pad][step].sliceCompressionRatio = ratio;
    }
}

void DrumMachine::setStepSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceSliceEQ(int pad, int step, float low, float mid, float high) {
    if (steps.find(pad) != steps.end() && step >= 0 && step < 16) {
        steps[pad][step].sliceEqLow = low;
        steps[pad][step].sliceEqMid = mid;
        steps[pad][step].sliceEqHigh = high;
    }
}

} // namespace VR_DAW 