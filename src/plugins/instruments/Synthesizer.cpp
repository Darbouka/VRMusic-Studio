#include "Synthesizer.hpp"
#include <spdlog/spdlog.h>
#include <random>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace VR_DAW {

Synthesizer::Synthesizer()
    : ampEnvelope{0.01f, 0.1f, 0.7f, 0.2f}
    , filterEnvelope{0.01f, 0.1f, 0.7f, 0.2f}
{
    generateId();
    name = "Synthesizer";
    initializeOscillators();
    initializeFilter();
    initializeEnvelopes();
    initializeLFO();
}

Synthesizer::~Synthesizer() {
    shutdown();
}

void Synthesizer::generateId() {
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

bool Synthesizer::initialize() {
    spdlog::info("Synthesizer initialisiert: {}", id);
    return true;
}

void Synthesizer::shutdown() {
    activeNotes.clear();
}

void Synthesizer::update() {
    // Hier würden kontinuierliche Updates durchgeführt werden
}

void Synthesizer::initializeOscillators() {
    // Drei Oszillatoren initialisieren
    oscillators.resize(3);
    for (auto& osc : oscillators) {
        osc.waveform = "sine";
        osc.frequency = 440.0f;
        osc.detune = 0.0f;
        osc.mix = 1.0f;
        osc.phase = 0.0f;
    }
}

void Synthesizer::initializeFilter() {
    filter.type = "lowpass";
    filter.cutoff = 1000.0f;
    filter.resonance = 0.7f;
    filter.envelopeAmount = 0.5f;
    filter.envelopeAttack = 0.01f;
    filter.envelopeDecay = 0.1f;
    filter.envelopeSustain = 0.7f;
    filter.envelopeRelease = 0.2f;
}

void Synthesizer::initializeEnvelopes() {
    // Envelopes sind bereits im Konstruktor initialisiert
}

void Synthesizer::initializeLFO() {
    lfo.rate = 5.0f;
    lfo.amount = 0.5f;
    lfo.waveform = "sine";
    lfo.destination = "filter";
    lfo.phase = 0.0f;
}

std::vector<PluginParameter> Synthesizer::getParameters() const {
    std::vector<PluginParameter> parameters;

    // Oszillator-Parameter
    for (int i = 0; i < oscillators.size(); ++i) {
        const auto& osc = oscillators[i];
        std::string prefix = "osc" + std::to_string(i + 1) + "_";

        // Waveform
        parameters.push_back({
            prefix + "waveform",
            "Oscillator " + std::to_string(i + 1) + " Waveform",
            PluginParameter::Type::Choice,
            0.0f,
            0.0f,
            0.0f,
            {"sine", "square", "saw", "triangle"},
            false,
            [this, i](float value) { setOscillatorWaveform(i, {"sine", "square", "saw", "triangle"}[static_cast<int>(value)]); }
        });

        // Frequency
        parameters.push_back({
            prefix + "frequency",
            "Oscillator " + std::to_string(i + 1) + " Frequency",
            PluginParameter::Type::Float,
            20.0f,
            20000.0f,
            440.0f,
            {},
            false,
            [this, i](float value) { setOscillatorFrequency(i, value); }
        });

        // Detune
        parameters.push_back({
            prefix + "detune",
            "Oscillator " + std::to_string(i + 1) + " Detune",
            PluginParameter::Type::Float,
            -100.0f,
            100.0f,
            0.0f,
            {},
            false,
            [this, i](float value) { setOscillatorDetune(i, value); }
        });

        // Mix
        parameters.push_back({
            prefix + "mix",
            "Oscillator " + std::to_string(i + 1) + " Mix",
            PluginParameter::Type::Float,
            0.0f,
            1.0f,
            1.0f,
            {},
            false,
            [this, i](float value) { setOscillatorMix(i, value); }
        });
    }

    // Filter-Parameter
    parameters.push_back({
        "filter_type",
        "Filter Type",
        PluginParameter::Type::Choice,
        0.0f,
        0.0f,
        0.0f,
        {"lowpass", "highpass", "bandpass", "notch"},
        false,
        [this](float value) { setFilterType({"lowpass", "highpass", "bandpass", "notch"}[static_cast<int>(value)]); }
    });

    parameters.push_back({
        "filter_cutoff",
        "Filter Cutoff",
        PluginParameter::Type::Float,
        20.0f,
        20000.0f,
        1000.0f,
        {},
        false,
        [this](float value) { setFilterCutoff(value); }
    });

    parameters.push_back({
        "filter_resonance",
        "Filter Resonance",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        0.7f,
        {},
        false,
        [this](float value) { setFilterResonance(value); }
    });

    // Envelope-Parameter
    parameters.push_back({
        "amp_attack",
        "Amp Attack",
        PluginParameter::Type::Float,
        0.001f,
        10.0f,
        0.01f,
        {},
        false,
        [this](float value) { setAmpEnvelopeAttack(value); }
    });

    parameters.push_back({
        "amp_decay",
        "Amp Decay",
        PluginParameter::Type::Float,
        0.001f,
        10.0f,
        0.1f,
        {},
        false,
        [this](float value) { setAmpEnvelopeDecay(value); }
    });

    parameters.push_back({
        "amp_sustain",
        "Amp Sustain",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        0.7f,
        {},
        false,
        [this](float value) { setAmpEnvelopeSustain(value); }
    });

    parameters.push_back({
        "amp_release",
        "Amp Release",
        PluginParameter::Type::Float,
        0.001f,
        10.0f,
        0.2f,
        {},
        false,
        [this](float value) { setAmpEnvelopeRelease(value); }
    });

    // LFO-Parameter
    parameters.push_back({
        "lfo_rate",
        "LFO Rate",
        PluginParameter::Type::Float,
        0.1f,
        20.0f,
        5.0f,
        {},
        false,
        [this](float value) { setLfoRate(value); }
    });

    parameters.push_back({
        "lfo_amount",
        "LFO Amount",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        0.5f,
        {},
        false,
        [this](float value) { setLfoAmount(value); }
    });

    parameters.push_back({
        "lfo_waveform",
        "LFO Waveform",
        PluginParameter::Type::Choice,
        0.0f,
        0.0f,
        0.0f,
        {"sine", "square", "saw", "triangle"},
        false,
        [this](float value) { setLfoWaveform({"sine", "square", "saw", "triangle"}[static_cast<int>(value)]); }
    });

    parameters.push_back({
        "lfo_destination",
        "LFO Destination",
        PluginParameter::Type::Choice,
        0.0f,
        0.0f,
        0.0f,
        {"filter", "amplitude", "pitch"},
        false,
        [this](float value) { setLfoDestination({"filter", "amplitude", "pitch"}[static_cast<int>(value)]); }
    });

    return parameters;
}

void Synthesizer::setParameter(const std::string& name, float value) {
    auto parameters = getParameters();
    for (const auto& param : parameters) {
        if (param.name == name) {
            value = std::max(param.minValue, std::min(param.maxValue, value));
            param.onChange(value);
            break;
        }
    }
}

float Synthesizer::getParameter(const std::string& name) const {
    // Hier würden die aktuellen Parameter-Werte zurückgegeben werden
    return 0.0f;
}

void Synthesizer::setParameterAutomated(const std::string& name, bool automated) {
    // Hier würde die Automation für den Parameter aktiviert/deaktiviert werden
}

bool Synthesizer::isParameterAutomated(const std::string& name) const {
    // Hier würde der Automation-Status des Parameters zurückgegeben werden
    return false;
}

void Synthesizer::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float time = static_cast<float>(i) / 44100.0f; // Sample-Rate: 44.1kHz
        float sample = 0.0f;

        // Oszillatoren verarbeiten
        for (const auto& osc : oscillators) {
            sample += processOscillator(osc, time) * osc.mix;
        }

        // Filter anwenden
        float cutoff = filter.cutoff;
        if (lfo.destination == "filter") {
            cutoff *= (1.0f + processLFO(time) * lfo.amount);
        }
        sample = processFilter(sample, cutoff, filter.resonance);

        // Amplitude anwenden
        float amplitude = 1.0f;
        if (lfo.destination == "amplitude") {
            amplitude *= (1.0f + processLFO(time) * lfo.amount);
        }
        sample *= amplitude;

        // In den Ausgangspuffer schreiben
        buffer[i] = sample;
    }
}

void Synthesizer::processMidi(const std::vector<uint8_t>& midiData) {
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

void Synthesizer::noteOn(int note, int velocity) {
    Note newNote;
    newNote.note = note;
    newNote.velocity = velocity;
    newNote.amplitude = velocity / 127.0f;
    newNote.filterEnvelope = 0.0f;
    newNote.ampEnvelope = 0.0f;
    newNote.active = true;
    activeNotes[note] = newNote;
}

void Synthesizer::noteOff(int note) {
    auto it = activeNotes.find(note);
    if (it != activeNotes.end()) {
        it->second.active = false;
    }
}

void Synthesizer::setPitchBend(float value) {
    // Hier würde der Pitch-Bend-Wert verarbeitet werden
}

void Synthesizer::setModulation(float value) {
    // Hier würde der Modulations-Wert verarbeitet werden
}

void Synthesizer::setAftertouch(float value) {
    // Hier würde der Aftertouch-Wert verarbeitet werden
}

float Synthesizer::processOscillator(const Oscillator& osc, float time) {
    float frequency = osc.frequency * (1.0f + osc.detune / 100.0f);
    float phase = osc.phase + frequency * time;
    osc.phase = fmod(phase, 1.0f);

    if (osc.waveform == "sine") {
        return std::sin(2.0f * M_PI * phase);
    } else if (osc.waveform == "square") {
        return phase < 0.5f ? 1.0f : -1.0f;
    } else if (osc.waveform == "saw") {
        return 2.0f * phase - 1.0f;
    } else if (osc.waveform == "triangle") {
        return phase < 0.5f ? 4.0f * phase - 1.0f : 3.0f - 4.0f * phase;
    }
    return 0.0f;
}

float Synthesizer::processFilter(float input, float cutoff, float resonance) {
    // Hier würde die Filter-Verarbeitung implementiert werden
    // Dies ist eine vereinfachte Version für Demonstrationszwecke
    return input;
}

float Synthesizer::processEnvelope(const Envelope& env, float time, bool noteOn) {
    // Hier würde die Envelope-Verarbeitung implementiert werden
    // Dies ist eine vereinfachte Version für Demonstrationszwecke
    return 1.0f;
}

float Synthesizer::processLFO(float time) {
    float phase = fmod(lfo.phase + lfo.rate * time, 1.0f);
    lfo.phase = phase;

    if (lfo.waveform == "sine") {
        return std::sin(2.0f * M_PI * phase);
    } else if (lfo.waveform == "square") {
        return phase < 0.5f ? 1.0f : -1.0f;
    } else if (lfo.waveform == "saw") {
        return 2.0f * phase - 1.0f;
    } else if (lfo.waveform == "triangle") {
        return phase < 0.5f ? 4.0f * phase - 1.0f : 3.0f - 4.0f * phase;
    }
    return 0.0f;
}

// Synthesizer-spezifische Setter-Methoden
void Synthesizer::setOscillatorWaveform(int oscillator, const std::string& waveform) {
    if (oscillator >= 0 && oscillator < oscillators.size()) {
        oscillators[oscillator].waveform = waveform;
    }
}

void Synthesizer::setOscillatorFrequency(int oscillator, float frequency) {
    if (oscillator >= 0 && oscillator < oscillators.size()) {
        oscillators[oscillator].frequency = frequency;
    }
}

void Synthesizer::setOscillatorDetune(int oscillator, float detune) {
    if (oscillator >= 0 && oscillator < oscillators.size()) {
        oscillators[oscillator].detune = detune;
    }
}

void Synthesizer::setOscillatorMix(int oscillator, float mix) {
    if (oscillator >= 0 && oscillator < oscillators.size()) {
        oscillators[oscillator].mix = mix;
    }
}

void Synthesizer::setFilterType(const std::string& type) {
    filter.type = type;
}

void Synthesizer::setFilterCutoff(float cutoff) {
    filter.cutoff = cutoff;
}

void Synthesizer::setFilterResonance(float resonance) {
    filter.resonance = resonance;
}

void Synthesizer::setFilterEnvelopeAmount(float amount) {
    filter.envelopeAmount = amount;
}

void Synthesizer::setFilterEnvelopeAttack(float attack) {
    filter.envelopeAttack = attack;
}

void Synthesizer::setFilterEnvelopeDecay(float decay) {
    filter.envelopeDecay = decay;
}

void Synthesizer::setFilterEnvelopeSustain(float sustain) {
    filter.envelopeSustain = sustain;
}

void Synthesizer::setFilterEnvelopeRelease(float release) {
    filter.envelopeRelease = release;
}

void Synthesizer::setAmpEnvelopeAttack(float attack) {
    ampEnvelope.attack = attack;
}

void Synthesizer::setAmpEnvelopeDecay(float decay) {
    ampEnvelope.decay = decay;
}

void Synthesizer::setAmpEnvelopeSustain(float sustain) {
    ampEnvelope.sustain = sustain;
}

void Synthesizer::setAmpEnvelopeRelease(float release) {
    ampEnvelope.release = release;
}

void Synthesizer::setLfoRate(float rate) {
    lfo.rate = rate;
}

void Synthesizer::setLfoAmount(float amount) {
    lfo.amount = amount;
}

void Synthesizer::setLfoWaveform(const std::string& waveform) {
    lfo.waveform = waveform;
}

void Synthesizer::setLfoDestination(const std::string& destination) {
    lfo.destination = destination;
}

} // namespace VR_DAW 