#include "Sampler.hpp"
#include <spdlog/spdlog.h>
#include <random>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <sndfile.h>

namespace VR_DAW {

Sampler::Sampler() {
    generateId();
    name = "Sampler";
    pitchBend = 0.0f;
    modulation = 0.0f;
    aftertouch = 0.0f;
}

Sampler::~Sampler() {
    shutdown();
}

void Sampler::generateId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    const char* hex = "0123456789abcdef";
    
    std::stringstream ss;
    for (int i = 0; i < 32; ++i) {
        ss << hex[dis(gen)];
    }
    id = ss.str();
}

bool Sampler::initialize() {
    spdlog::info("Sampler initialisiert: {}", id);
    return true;
}

void Sampler::shutdown() {
    activeNotes.clear();
    samples.clear();
}

void Sampler::update() {
    // Hier würden kontinuierliche Updates durchgeführt werden
}

std::vector<PluginParameter> Sampler::getParameters() const {
    std::vector<PluginParameter> parameters;

    // Sample-Parameter
    parameters.push_back({
        "sample_loop",
        "Sample Loop",
        PluginParameter::Type::Bool,
        0.0f,
        1.0f,
        0.0f,
        {},
        false,
        [this](float value) { setSampleLoop(60, value > 0.5f); }
    });

    parameters.push_back({
        "sample_loop_start",
        "Sample Loop Start",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        0.0f,
        {},
        false,
        [this](float value) { setSampleLoopPoints(60, value, 1.0f); }
    });

    parameters.push_back({
        "sample_loop_end",
        "Sample Loop End",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        1.0f,
        {},
        false,
        [this](float value) { setSampleLoopPoints(60, 0.0f, value); }
    });

    parameters.push_back({
        "sample_pitch",
        "Sample Pitch",
        PluginParameter::Type::Float,
        -12.0f,
        12.0f,
        0.0f,
        {},
        false,
        [this](float value) { setSamplePitch(60, value); }
    });

    parameters.push_back({
        "sample_volume",
        "Sample Volume",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        1.0f,
        {},
        false,
        [this](float value) { setSampleVolume(60, value); }
    });

    parameters.push_back({
        "sample_pan",
        "Sample Pan",
        PluginParameter::Type::Float,
        -1.0f,
        1.0f,
        0.0f,
        {},
        false,
        [this](float value) { setSamplePan(60, value); }
    });

    // Filter-Parameter
    parameters.push_back({
        "sample_filter_cutoff",
        "Sample Filter Cutoff",
        PluginParameter::Type::Float,
        20.0f,
        20000.0f,
        1000.0f,
        {},
        false,
        [this](float value) { setSampleFilter(60, value, 0.7f); }
    });

    parameters.push_back({
        "sample_filter_resonance",
        "Sample Filter Resonance",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        0.7f,
        {},
        false,
        [this](float value) { setSampleFilter(60, 1000.0f, value); }
    });

    // Envelope-Parameter
    parameters.push_back({
        "sample_envelope_attack",
        "Sample Envelope Attack",
        PluginParameter::Type::Float,
        0.001f,
        10.0f,
        0.01f,
        {},
        false,
        [this](float value) { setSampleEnvelope(60, value, 0.1f, 0.7f, 0.2f); }
    });

    parameters.push_back({
        "sample_envelope_decay",
        "Sample Envelope Decay",
        PluginParameter::Type::Float,
        0.001f,
        10.0f,
        0.1f,
        {},
        false,
        [this](float value) { setSampleEnvelope(60, 0.01f, value, 0.7f, 0.2f); }
    });

    parameters.push_back({
        "sample_envelope_sustain",
        "Sample Envelope Sustain",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        0.7f,
        {},
        false,
        [this](float value) { setSampleEnvelope(60, 0.01f, 0.1f, value, 0.2f); }
    });

    parameters.push_back({
        "sample_envelope_release",
        "Sample Envelope Release",
        PluginParameter::Type::Float,
        0.001f,
        10.0f,
        0.2f,
        {},
        false,
        [this](float value) { setSampleEnvelope(60, 0.01f, 0.1f, 0.7f, value); }
    });

    // LFO-Parameter
    parameters.push_back({
        "sample_lfo_rate",
        "Sample LFO Rate",
        PluginParameter::Type::Float,
        0.1f,
        20.0f,
        5.0f,
        {},
        false,
        [this](float value) { setSampleLFO(60, value, 0.5f, "filter"); }
    });

    parameters.push_back({
        "sample_lfo_amount",
        "Sample LFO Amount",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        0.5f,
        {},
        false,
        [this](float value) { setSampleLFO(60, 5.0f, value, "filter"); }
    });

    parameters.push_back({
        "sample_lfo_destination",
        "Sample LFO Destination",
        PluginParameter::Type::Choice,
        0.0f,
        0.0f,
        0.0f,
        {"filter", "amplitude", "pitch"},
        false,
        [this](float value) { setSampleLFO(60, 5.0f, 0.5f, {"filter", "amplitude", "pitch"}[static_cast<int>(value)]); }
    });

    // Effekt-Parameter
    parameters.push_back({
        "sample_reverb",
        "Sample Reverb",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        0.0f,
        {},
        false,
        [this](float value) { setSampleReverb(60, value); }
    });

    parameters.push_back({
        "sample_delay_time",
        "Sample Delay Time",
        PluginParameter::Type::Float,
        0.0f,
        2.0f,
        0.0f,
        {},
        false,
        [this](float value) { setSampleDelay(60, value, 0.0f); }
    });

    parameters.push_back({
        "sample_delay_feedback",
        "Sample Delay Feedback",
        PluginParameter::Type::Float,
        0.0f,
        0.9f,
        0.0f,
        {},
        false,
        [this](float value) { setSampleDelay(60, 0.0f, value); }
    });

    parameters.push_back({
        "sample_compression_threshold",
        "Sample Compression Threshold",
        PluginParameter::Type::Float,
        -60.0f,
        0.0f,
        -20.0f,
        {},
        false,
        [this](float value) { setSampleCompression(60, value, 4.0f); }
    });

    parameters.push_back({
        "sample_compression_ratio",
        "Sample Compression Ratio",
        PluginParameter::Type::Float,
        1.0f,
        20.0f,
        4.0f,
        {},
        false,
        [this](float value) { setSampleCompression(60, -20.0f, value); }
    });

    parameters.push_back({
        "sample_eq_low",
        "Sample EQ Low",
        PluginParameter::Type::Float,
        -12.0f,
        12.0f,
        0.0f,
        {},
        false,
        [this](float value) { setSampleEQ(60, value, 0.0f, 0.0f); }
    });

    parameters.push_back({
        "sample_eq_mid",
        "Sample EQ Mid",
        PluginParameter::Type::Float,
        -12.0f,
        12.0f,
        0.0f,
        {},
        false,
        [this](float value) { setSampleEQ(60, 0.0f, value, 0.0f); }
    });

    parameters.push_back({
        "sample_eq_high",
        "Sample EQ High",
        PluginParameter::Type::Float,
        -12.0f,
        12.0f,
        0.0f,
        {},
        false,
        [this](float value) { setSampleEQ(60, 0.0f, 0.0f, value); }
    });

    parameters.push_back({
        "sample_distortion",
        "Sample Distortion",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        0.0f,
        {},
        false,
        [this](float value) { setSampleDistortion(60, value); }
    });

    parameters.push_back({
        "sample_bitcrusher_bits",
        "Sample Bitcrusher Bits",
        PluginParameter::Type::Int,
        1.0f,
        16.0f,
        16.0f,
        {},
        false,
        [this](float value) { setSampleBitCrusher(60, static_cast<int>(value), 1.0f); }
    });

    parameters.push_back({
        "sample_bitcrusher_rate",
        "Sample Bitcrusher Rate",
        PluginParameter::Type::Float,
        1.0f,
        44100.0f,
        44100.0f,
        {},
        false,
        [this](float value) { setSampleBitCrusher(60, 16, value); }
    });

    // Granular-Parameter
    parameters.push_back({
        "sample_granular_grain_size",
        "Sample Granular Grain Size",
        PluginParameter::Type::Float,
        0.001f,
        1.0f,
        0.1f,
        {},
        false,
        [this](float value) { setSampleGranular(60, value, 0.5f, 0.0f); }
    });

    parameters.push_back({
        "sample_granular_density",
        "Sample Granular Density",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        0.5f,
        {},
        false,
        [this](float value) { setSampleGranular(60, 0.1f, value, 0.0f); }
    });

    parameters.push_back({
        "sample_granular_pitch",
        "Sample Granular Pitch",
        PluginParameter::Type::Float,
        -12.0f,
        12.0f,
        0.0f,
        {},
        false,
        [this](float value) { setSampleGranular(60, 0.1f, 0.5f, value); }
    });

    // Time Stretch Parameter
    parameters.push_back({
        "sample_time_stretch",
        "Sample Time Stretch",
        PluginParameter::Type::Float,
        0.25f,
        4.0f,
        1.0f,
        {},
        false,
        [this](float value) { setSampleTimeStretch(60, value); }
    });

    // Reverse Parameter
    parameters.push_back({
        "sample_reverse",
        "Sample Reverse",
        PluginParameter::Type::Bool,
        0.0f,
        1.0f,
        0.0f,
        {},
        false,
        [this](float value) { setSampleReverse(60, value > 0.5f); }
    });

    return parameters;
}

void Sampler::setParameter(const std::string& name, float value) {
    auto parameters = getParameters();
    for (const auto& param : parameters) {
        if (param.name == name) {
            value = std::max(param.minValue, std::min(param.maxValue, value));
            param.onChange(value);
            break;
        }
    }
}

float Sampler::getParameter(const std::string& name) const {
    // Hier würden die aktuellen Parameter-Werte zurückgegeben werden
    return 0.0f;
}

void Sampler::setParameterAutomated(const std::string& name, bool automated) {
    // Hier würde die Automation für den Parameter aktiviert/deaktiviert werden
}

bool Sampler::isParameterAutomated(const std::string& name) const {
    // Hier würde der Automation-Status des Parameters zurückgegeben werden
    return false;
}

void Sampler::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float time = static_cast<float>(i) / 44100.0f; // Sample-Rate: 44.1kHz
        float sample = 0.0f;

        // Aktive Noten verarbeiten
        for (auto& [note, noteData] : activeNotes) {
            if (noteData.active) {
                auto it = samples.find(note);
                if (it != samples.end()) {
                    const auto& sampleData = it->second;
                    float position = noteData.position;
                    float processedSample = processSample(sampleData, position);
                    
                    // Filter anwenden
                    float cutoff = sampleData.filterCutoff;
                    if (sampleData.lfoDestination == "filter") {
                        cutoff *= (1.0f + processLFO(sampleData.lfoRate, sampleData.lfoAmount, sampleData.lfoDestination, time));
                    }
                    processedSample = processFilter(processedSample, cutoff, sampleData.filterResonance);

                    // Envelope anwenden
                    float envelope = processEnvelope(processedSample, sampleData.envelopeAttack, sampleData.envelopeDecay, sampleData.envelopeSustain, sampleData.envelopeRelease, time);
                    processedSample *= envelope;

                    // Effekte anwenden
                    processedSample = processReverb(processedSample, sampleData.reverbAmount);
                    processedSample = processDelay(processedSample, sampleData.delayTime, sampleData.delayFeedback);
                    processedSample = processCompression(processedSample, sampleData.compressionThreshold, sampleData.compressionRatio);
                    processedSample = processEQ(processedSample, sampleData.eqLow, sampleData.eqMid, sampleData.eqHigh);
                    processedSample = processDistortion(processedSample, sampleData.distortionAmount);
                    processedSample = processBitCrusher(processedSample, sampleData.bitCrusherBits, sampleData.bitCrusherRate);
                    processedSample = processGranular(processedSample, sampleData.granularGrainSize, sampleData.granularDensity, sampleData.granularPitch);
                    processedSample = processTimeStretch(processedSample, sampleData.timeStretchRate);
                    if (sampleData.reverse) {
                        processedSample = processReverse(processedSample);
                    }

                    // Slices verarbeiten
                    if (!sampleData.slicePoints.empty()) {
                        processedSample = processSlice(sampleData, noteData.currentSlice, noteData.slicePosition);
                    }

                    sample += processedSample * noteData.amplitude;
                    noteData.position += 1.0f / sampleData.sampleRate;
                }
            }
        }

        // In den Ausgangspuffer schreiben
        buffer[i] = sample;
    }
}

void Sampler::processMidi(const std::vector<uint8_t>& midiData) {
    if (midiData.empty()) return;

    uint8_t status = midiData[0];
    uint8_t note = midiData[1];
    uint8_t velocity = midiData.size() > 2 ? midiData[2] : 0;

    if ((status & 0xF0) == 0x90) { // Note On
        noteOn(note, velocity);
    } else if ((status & 0xF0) == 0x80) { // Note Off
        noteOff(note);
    }
}

void Sampler::loadSample(const std::string& path, int note) {
    SF_INFO fileInfo;
    SNDFILE* file = sf_open(path.c_str(), SFM_READ, &fileInfo);
    if (!file) {
        spdlog::error("Konnte Sample nicht laden: {}", path);
        return;
    }

    Sample sample;
    sample.data.resize(fileInfo.frames * fileInfo.channels);
    sf_readf_float(file, sample.data.data(), fileInfo.frames);
    sample.sampleRate = fileInfo.samplerate;
    sample.channels = fileInfo.channels;
    sample.loop = false;
    sample.loopStart = 0.0f;
    sample.loopEnd = 1.0f;
    sample.pitch = 0.0f;
    sample.volume = 1.0f;
    sample.pan = 0.0f;
    sample.filterCutoff = 1000.0f;
    sample.filterResonance = 0.7f;
    sample.envelopeAttack = 0.01f;
    sample.envelopeDecay = 0.1f;
    sample.envelopeSustain = 0.7f;
    sample.envelopeRelease = 0.2f;
    sample.lfoRate = 5.0f;
    sample.lfoAmount = 0.5f;
    sample.lfoDestination = "filter";
    sample.reverbAmount = 0.0f;
    sample.delayTime = 0.0f;
    sample.delayFeedback = 0.0f;
    sample.compressionThreshold = -20.0f;
    sample.compressionRatio = 4.0f;
    sample.eqLow = 0.0f;
    sample.eqMid = 0.0f;
    sample.eqHigh = 0.0f;
    sample.distortionAmount = 0.0f;
    sample.bitCrusherBits = 16;
    sample.bitCrusherRate = 44100.0f;
    sample.granularGrainSize = 0.1f;
    sample.granularDensity = 0.5f;
    sample.granularPitch = 0.0f;
    sample.timeStretchRate = 1.0f;
    sample.reverse = false;

    samples[note] = sample;
    sf_close(file);
}

void Sampler::unloadSample(int note) {
    samples.erase(note);
}

void Sampler::setSampleLoop(int note, bool loop) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.loop = loop;
    }
}

void Sampler::setSampleLoopPoints(int note, float start, float end) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.loopStart = start;
        it->second.loopEnd = end;
    }
}

void Sampler::setSamplePitch(int note, float pitch) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.pitch = pitch;
    }
}

void Sampler::setSampleVolume(int note, float volume) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.volume = volume;
    }
}

void Sampler::setSamplePan(int note, float pan) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.pan = pan;
    }
}

void Sampler::setSampleFilter(int note, float cutoff, float resonance) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.filterCutoff = cutoff;
        it->second.filterResonance = resonance;
    }
}

void Sampler::setSampleEnvelope(int note, float attack, float decay, float sustain, float release) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.envelopeAttack = attack;
        it->second.envelopeDecay = decay;
        it->second.envelopeSustain = sustain;
        it->second.envelopeRelease = release;
    }
}

void Sampler::setSampleLFO(int note, float rate, float amount, const std::string& destination) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.lfoRate = rate;
        it->second.lfoAmount = amount;
        it->second.lfoDestination = destination;
    }
}

void Sampler::setSampleReverb(int note, float amount) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.reverbAmount = amount;
    }
}

void Sampler::setSampleDelay(int note, float time, float feedback) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.delayTime = time;
        it->second.delayFeedback = feedback;
    }
}

void Sampler::setSampleCompression(int note, float threshold, float ratio) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.compressionThreshold = threshold;
        it->second.compressionRatio = ratio;
    }
}

void Sampler::setSampleEQ(int note, float low, float mid, float high) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.eqLow = low;
        it->second.eqMid = mid;
        it->second.eqHigh = high;
    }
}

void Sampler::setSampleDistortion(int note, float amount) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.distortionAmount = amount;
    }
}

void Sampler::setSampleBitCrusher(int note, int bits, float rate) {
    // Entfernt
}

void Sampler::setSampleSliceBitCrusher(int note, int bits, float rate) {
    // Entfernt
}

float Sampler::processBitCrusher(float input, int bits, float rate) {
    // Entfernt
    return input;
}

void Sampler::setSampleGranular(int note, float grainSize, float density, float pitch) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.granularGrainSize = grainSize;
        it->second.granularDensity = density;
        it->second.granularPitch = pitch;
    }
}

void Sampler::setSampleTimeStretch(int note, float rate) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.timeStretchRate = rate;
    }
}

void Sampler::setSampleReverse(int note, bool reverse) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.reverse = reverse;
    }
}

void Sampler::setSampleSlice(int note, const std::vector<float>& slicePoints) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.slicePoints = slicePoints;
    }
}

void Sampler::setSampleSliceMode(int note, const std::string& mode) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.sliceMode = mode;
    }
}

void Sampler::setSampleSliceQuantize(int note, float amount) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.sliceQuantize = amount;
    }
}

void Sampler::setSampleSliceRandom(int note, float amount) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.sliceRandom = amount;
    }
}

void Sampler::setSampleSliceReverse(int note, bool reverse) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.sliceReverse = reverse;
    }
}

void Sampler::setSampleSlicePitch(int note, float pitch) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.slicePitch = pitch;
    }
}

void Sampler::setSampleSliceVolume(int note, float volume) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.sliceVolume = volume;
    }
}

void Sampler::setSampleSlicePan(int note, float pan) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.slicePan = pan;
    }
}

void Sampler::setSampleSliceFilter(int note, float cutoff, float resonance) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.sliceFilterCutoff = cutoff;
        it->second.sliceFilterResonance = resonance;
    }
}

void Sampler::setSampleSliceEnvelope(int note, float attack, float decay, float sustain, float release) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.sliceEnvelopeAttack = attack;
        it->second.sliceEnvelopeDecay = decay;
        it->second.sliceEnvelopeSustain = sustain;
        it->second.sliceEnvelopeRelease = release;
    }
}

void Sampler::setSampleSliceLFO(int note, float rate, float amount, const std::string& destination) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.sliceLfoRate = rate;
        it->second.sliceLfoAmount = amount;
        it->second.sliceLfoDestination = destination;
    }
}

void Sampler::setSampleSliceReverb(int note, float amount) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.sliceReverbAmount = amount;
    }
}

void Sampler::setSampleSliceDelay(int note, float time, float feedback) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.sliceDelayTime = time;
        it->second.sliceDelayFeedback = feedback;
    }
}

void Sampler::setSampleSliceCompression(int note, float threshold, float ratio) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.sliceCompressionThreshold = threshold;
        it->second.sliceCompressionRatio = ratio;
    }
}

void Sampler::setSampleSliceEQ(int note, float low, float mid, float high) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.sliceEqLow = low;
        it->second.sliceEqMid = mid;
        it->second.sliceEqHigh = high;
    }
}

void Sampler::setSampleSliceDistortion(int note, float amount) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.sliceDistortionAmount = amount;
    }
}

void Sampler::setSampleSliceGranular(int note, float grainSize, float density, float pitch) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.sliceGranularGrainSize = grainSize;
        it->second.sliceGranularDensity = density;
        it->second.sliceGranularPitch = pitch;
    }
}

void Sampler::setSampleSliceTimeStretch(int note, float rate) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.sliceTimeStretchRate = rate;
    }
}

void Sampler::setSampleSliceReverse(int note, bool reverse) {
    auto it = samples.find(note);
    if (it != samples.end()) {
        it->second.sliceReverse = reverse;
    }
}

void Sampler::noteOn(int note, int velocity) {
    Note newNote;
    newNote.note = note;
    newNote.velocity = velocity;
    newNote.amplitude = velocity / 127.0f;
    newNote.position = 0.0f;
    newNote.filterEnvelope = 0.0f;
    newNote.ampEnvelope = 0.0f;
    newNote.active = true;
    newNote.currentSlice = 0;
    newNote.slicePosition = 0.0f;
    newNote.sliceFilterEnvelope = 0.0f;
    newNote.sliceAmpEnvelope = 0.0f;
    newNote.sliceActive = false;
    activeNotes[note] = newNote;
}

void Sampler::noteOff(int note) {
    auto it = activeNotes.find(note);
    if (it != activeNotes.end()) {
        it->second.active = false;
    }
}

float Sampler::processSample(const Sample& sample, float position) {
    if (sample.data.empty()) return 0.0f;

    // Position im Sample berechnen
    float sampleLength = static_cast<float>(sample.data.size()) / sample.channels;
    float normalizedPosition = position / sampleLength;

    // Loop-Punkte berücksichtigen
    if (sample.loop && normalizedPosition >= sample.loopEnd) {
        normalizedPosition = sample.loopStart + fmod(normalizedPosition - sample.loopStart, sample.loopEnd - sample.loopStart);
    }

    // Sample-Position berechnen
    float samplePosition = normalizedPosition * sampleLength;
    int index = static_cast<int>(samplePosition);
    float fraction = samplePosition - index;

    // Lineare Interpolation
    if (index + 1 < sample.data.size()) {
        return sample.data[index] * (1.0f - fraction) + sample.data[index + 1] * fraction;
    }
    return sample.data[index];
}

float Sampler::processFilter(float input, float cutoff, float resonance) {
    // Hier würde die Filter-Verarbeitung implementiert werden
    // Dies ist eine vereinfachte Version für Demonstrationszwecke
    return input;
}

float Sampler::processEnvelope(float input, float attack, float decay, float sustain, float release, float time) {
    // Hier würde die Envelope-Verarbeitung implementiert werden
    // Dies ist eine vereinfachte Version für Demonstrationszwecke
    return 1.0f;
}

float Sampler::processLFO(float rate, float amount, const std::string& destination, float time) {
    float phase = fmod(rate * time, 1.0f);
    return std::sin(2.0f * M_PI * phase) * amount;
}

float Sampler::processReverb(float input, float amount) {
    // Hier würde die Reverb-Verarbeitung implementiert werden
    // Dies ist eine vereinfachte Version für Demonstrationszwecke
    return input;
}

float Sampler::processDelay(float input, float time, float feedback) {
    // Hier würde die Delay-Verarbeitung implementiert werden
    // Dies ist eine vereinfachte Version für Demonstrationszwecke
    return input;
}

float Sampler::processCompression(float input, float threshold, float ratio) {
    // Hier würde die Kompressions-Verarbeitung implementiert werden
    // Dies ist eine vereinfachte Version für Demonstrationszwecke
    return input;
}

float Sampler::processEQ(float input, float low, float mid, float high) {
    // Hier würde die EQ-Verarbeitung implementiert werden
    // Dies ist eine vereinfachte Version für Demonstrationszwecke
    return input;
}

float Sampler::processDistortion(float input, float amount) {
    // Hier würde die Distortion-Verarbeitung implementiert werden
    // Dies ist eine vereinfachte Version für Demonstrationszwecke
    return input;
}

float Sampler::processGranular(float input, float grainSize, float density, float pitch) {
    // Hier würde die Granular-Verarbeitung implementiert werden
    // Dies ist eine vereinfachte Version für Demonstrationszwecke
    return input;
}

float Sampler::processTimeStretch(float input, float rate) {
    // Hier würde die Time-Stretch-Verarbeitung implementiert werden
    // Dies ist eine vereinfachte Version für Demonstrationszwecke
    return input;
}

float Sampler::processReverse(float input) {
    // Hier würde die Reverse-Verarbeitung implementiert werden
    // Dies ist eine vereinfachte Version für Demonstrationszwecke
    return input;
}

float Sampler::processSlice(const Sample& sample, int slice, float position) {
    // Hier würde die Slice-Verarbeitung implementiert werden
    // Dies ist eine vereinfachte Version für Demonstrationszwecke
    return 0.0f;
}

} // namespace VR_DAW 