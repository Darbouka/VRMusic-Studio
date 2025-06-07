#include "QuantumDelayEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

QuantumDelayEffect::QuantumDelayEffect()
    : time(0.5f)
    , feedback(0.5f)
    , entanglement(0.3f)
    , superposition(0.5f)
    , interference(0.4f)
    , mix(0.5f)
    , quality(1.0f)
    , automatedTime(false)
    , automatedFeedback(false)
    , automatedEntanglement(false)
    , automatedSuperposition(false)
    , automatedInterference(false)
    , automatedMix(false)
    , automatedQuality(false)
    , bufferSize(44100 * 4) // 4 Sekunden bei 44.1kHz
{
    std::random_device rd;
    rng.seed(rd());
    initializeDelayLines();
}

QuantumDelayEffect::~QuantumDelayEffect() {
    shutdown();
}

bool QuantumDelayEffect::initialize() {
    initializeDelayLines();
    return true;
}

void QuantumDelayEffect::shutdown() {
    delayLines.clear();
}

void QuantumDelayEffect::initializeDelayLines() {
    delayLines.clear();
    
    // Erstelle 8 Quanten-Delay-Lines
    for (int i = 0; i < 8; ++i) {
        DelayLine line;
        line.buffer.resize(bufferSize, 0.0f);
        line.writePos = 0;
        line.readPos = 0;
        line.time = time;
        line.feedback = feedback;
        
        // Initialisiere Quantenzustand
        line.state.wavefunction.resize(64, std::complex<float>(0.0f, 0.0f));
        line.state.probabilities.resize(64, 0.0f);
        line.state.phase = 0.0f;
        line.state.entangled = false;
        
        delayLines.push_back(line);
    }
}

void QuantumDelayEffect::updateDelayLines() {
    for (auto& line : delayLines) {
        // Aktualisiere Delay-Positionen
        line.writePos = (line.writePos + 1) % bufferSize;
        line.readPos = (line.readPos + 1) % bufferSize;
        
        // Wende Time an
        if (time != currentTime) {
            line.time = time;
            currentTime = time;
        }
        
        // Wende Feedback an
        if (feedback != currentFeedback) {
            line.feedback = feedback;
            currentFeedback = feedback;
        }
        
        // Wende Quantenzustände an
        updateQuantumStates();
    }
}

void QuantumDelayEffect::updateQuantumStates() {
    // Aktualisiere Quantenzustände basierend auf Parametern
    if (entanglement != currentEntanglement) {
        applyEntanglement();
        currentEntanglement = entanglement;
    }
    
    if (superposition != currentSuperposition) {
        applySuperposition();
        currentSuperposition = superposition;
    }
    
    if (interference != currentInterference) {
        applyInterference();
        currentInterference = interference;
    }
}

void QuantumDelayEffect::applyEntanglement() {
    // Verschränke Quantenzustände
    for (size_t i = 0; i < delayLines.size() - 1; ++i) {
        if (std::rand() / RAND_MAX < entanglement) {
            delayLines[i].state.entangled = true;
            delayLines[i + 1].state.entangled = true;
        }
    }
}

void QuantumDelayEffect::applySuperposition() {
    // Erzeuge Überlagerungszustände
    for (auto& line : delayLines) {
        for (size_t i = 0; i < line.state.wavefunction.size(); ++i) {
            float phase = 2.0f * M_PI * i / line.state.wavefunction.size();
            line.state.wavefunction[i] = std::complex<float>(
                std::cos(phase) * superposition,
                std::sin(phase) * superposition
            );
        }
    }
}

void QuantumDelayEffect::applyInterference() {
    // Erzeuge Interferenzmuster
    for (auto& line : delayLines) {
        for (size_t i = 0; i < line.state.wavefunction.size(); ++i) {
            line.state.wavefunction[i] *= std::complex<float>(
                std::cos(interference * i),
                std::sin(interference * i)
            );
        }
    }
}

std::complex<float> QuantumDelayEffect::calculateWavefunction(float input) {
    // Berechne Wellenfunktion für Eingangssignal
    return std::complex<float>(
        input * std::cos(2.0f * M_PI * input),
        input * std::sin(2.0f * M_PI * input)
    );
}

void QuantumDelayEffect::collapseWavefunction(QuantumState& state) {
    // Kollabiere Wellenfunktion zu klassischem Zustand
    for (size_t i = 0; i < state.wavefunction.size(); ++i) {
        state.probabilities[i] = std::norm(state.wavefunction[i]);
    }
}

void QuantumDelayEffect::processDelayLine(DelayLine& line, float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        // Berechne Delay-Position
        size_t delayPos = static_cast<size_t>(line.time * bufferSize);
        
        // Lese Sample aus Delay-Line
        float sample = line.buffer[delayPos];
        
        // Wende Quantenzustand an
        std::complex<float> wave = calculateWavefunction(sample);
        line.state.wavefunction[0] = wave;
        
        // Kollabiere Wellenfunktion
        collapseWavefunction(line.state);
        
        // Wende Feedback an
        sample *= line.feedback;
        
        // Schreibe in Output Buffer
        buffer[i] = sample;
        
        // Schreibe in Delay-Line
        line.buffer[line.writePos] = buffer[i];
    }
}

void QuantumDelayEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateDelayLines();
    
    // Verarbeite jede Delay-Line
    for (auto& line : delayLines) {
        processDelayLine(line, buffer, framesPerBuffer);
    }
    
    // Wende Mix an
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = buffer[i] * mix;
    }
}

std::vector<PluginParameter> QuantumDelayEffect::getParameters() const {
    return {
        {"time", time, 0.0f, 1.0f, ""},
        {"feedback", feedback, 0.0f, 1.0f, ""},
        {"entanglement", entanglement, 0.0f, 1.0f, ""},
        {"superposition", superposition, 0.0f, 1.0f, ""},
        {"interference", interference, 0.0f, 1.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void QuantumDelayEffect::setParameter(const std::string& name, float value) {
    if (name == "time") time = value;
    else if (name == "feedback") feedback = value;
    else if (name == "entanglement") entanglement = value;
    else if (name == "superposition") superposition = value;
    else if (name == "interference") interference = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
}

float QuantumDelayEffect::getParameter(const std::string& name) const {
    if (name == "time") return time;
    if (name == "feedback") return feedback;
    if (name == "entanglement") return entanglement;
    if (name == "superposition") return superposition;
    if (name == "interference") return interference;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
    return 0.0f;
}

void QuantumDelayEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "time") automatedTime = automated;
    else if (name == "feedback") automatedFeedback = automated;
    else if (name == "entanglement") automatedEntanglement = automated;
    else if (name == "superposition") automatedSuperposition = automated;
    else if (name == "interference") automatedInterference = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool QuantumDelayEffect::isParameterAutomated(const std::string& name) const {
    if (name == "time") return automatedTime;
    if (name == "feedback") return automatedFeedback;
    if (name == "entanglement") return automatedEntanglement;
    if (name == "superposition") return automatedSuperposition;
    if (name == "interference") return automatedInterference;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    return false;
}

void QuantumDelayEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        time = 0.5f;
        feedback = 0.5f;
        entanglement = 0.3f;
        superposition = 0.5f;
        interference = 0.4f;
        mix = 0.5f;
        quality = 1.0f;
    }
    else if (presetName == "Heavy") {
        time = 0.7f;
        feedback = 0.7f;
        entanglement = 0.5f;
        superposition = 0.7f;
        interference = 0.6f;
        mix = 0.7f;
        quality = 1.0f;
    }
    else if (presetName == "Subtle") {
        time = 0.3f;
        feedback = 0.3f;
        entanglement = 0.2f;
        superposition = 0.3f;
        interference = 0.2f;
        mix = 0.3f;
        quality = 1.0f;
    }
    
    updateDelayLines();
}

void QuantumDelayEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> QuantumDelayEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 