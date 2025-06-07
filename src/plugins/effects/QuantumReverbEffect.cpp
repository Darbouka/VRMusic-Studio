#include "QuantumReverbEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

QuantumReverbEffect::QuantumReverbEffect()
    : time(0.5f)
    , size(0.3f)
    , diffusion(0.6f)
    , entanglement(0.5f)
    , superposition(0.4f)
    , mix(0.5f)
    , quality(1.0f)
    , automatedTime(false)
    , automatedSize(false)
    , automatedDiffusion(false)
    , automatedEntanglement(false)
    , automatedSuperposition(false)
    , automatedMix(false)
    , automatedQuality(false)
    , bufferSize(44100 * 2) // 2 Sekunden bei 44.1kHz
{
    std::random_device rd;
    rng.seed(rd());
    initializeStates();
}

QuantumReverbEffect::~QuantumReverbEffect() {
    shutdown();
}

bool QuantumReverbEffect::initialize() {
    initializeStates();
    return true;
}

void QuantumReverbEffect::shutdown() {
    states.clear();
}

void QuantumReverbEffect::initializeStates() {
    states.clear();
    
    // Erstelle 4 Reverb-Zustände
    for (int i = 0; i < 4; ++i) {
        ReverbState state;
        
        // Erstelle 8 Quanten-Zustände pro Reverb-Zustand
        for (int j = 0; j < 8; ++j) {
            QuantumState quantum;
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            
            quantum.amplitude = dist(rng);
            quantum.phase = dist(rng) * 2.0f * M_PI;
            quantum.entanglement = dist(rng);
            quantum.superposition = dist(rng);
            
            state.quantums.push_back(quantum);
        }
        
        state.time = time;
        state.size = size;
        states.push_back(state);
    }
}

void QuantumReverbEffect::updateStates() {
    // Aktualisiere Reverb-Zustände
    for (auto& state : states) {
        state.time = time;
        state.size = size;
        
        // Aktualisiere Quanten-Zustände
        for (auto& quantum : state.quantums) {
            // Aktualisiere Amplitude basierend auf Entanglement
            quantum.amplitude += entanglement * (std::rand() / RAND_MAX - 0.5f) * 0.01f;
            quantum.amplitude = std::max(0.0f, std::min(1.0f, quantum.amplitude));
            
            // Aktualisiere Phase
            quantum.phase += quantum.entanglement * time * 0.0001f;
            quantum.phase = std::fmod(quantum.phase, 2.0f * M_PI);
            
            // Aktualisiere Superposition
            quantum.superposition *= (1.0f + superposition * std::sin(quantum.phase));
            quantum.superposition = std::max(0.0f, std::min(1.0f, quantum.superposition));
        }
    }
}

void QuantumReverbEffect::processState(ReverbState& state, float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float t = static_cast<float>(i) / framesPerBuffer;
        float output = buffer[i];
        
        // Verarbeite jeden Quanten-Zustand
        for (auto& quantum : state.quantums) {
            // Berechne Quanten-Wert
            float value = output * quantum.amplitude;
            value *= std::sin(2.0f * M_PI * quantum.entanglement * t + quantum.phase);
            
            // Modifiziere Output
            output += value * state.size;
        }
        
        // Normalisiere Output
        output /= (state.quantums.size() + 1);
        
        buffer[i] = output;
    }
}

void QuantumReverbEffect::applyTime(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float t = static_cast<float>(i) / framesPerBuffer;
        buffer[i] *= (1.0f + time * std::sin(2.0f * M_PI * t));
    }
}

void QuantumReverbEffect::applySize(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] *= (1.0f + size);
    }
}

void QuantumReverbEffect::applyDiffusion(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float t = static_cast<float>(i) / framesPerBuffer;
        buffer[i] += diffusion * buffer[static_cast<unsigned long>(t * framesPerBuffer)];
    }
}

void QuantumReverbEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateStates();
    
    // Verarbeite jeden Reverb-Zustand
    for (auto& state : states) {
        processState(state, buffer, framesPerBuffer);
    }
    
    // Wende Effekte an
    applyTime(buffer, framesPerBuffer);
    applySize(buffer, framesPerBuffer);
    applyDiffusion(buffer, framesPerBuffer);
    
    // Wende Mix an
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = buffer[i] * mix;
    }
}

std::vector<PluginParameter> QuantumReverbEffect::getParameters() const {
    return {
        {"time", time, 0.0f, 1.0f, ""},
        {"size", size, 0.0f, 1.0f, ""},
        {"diffusion", diffusion, 0.0f, 1.0f, ""},
        {"entanglement", entanglement, 0.0f, 1.0f, ""},
        {"superposition", superposition, 0.0f, 1.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void QuantumReverbEffect::setParameter(const std::string& name, float value) {
    if (name == "time") time = value;
    else if (name == "size") size = value;
    else if (name == "diffusion") diffusion = value;
    else if (name == "entanglement") entanglement = value;
    else if (name == "superposition") superposition = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
}

float QuantumReverbEffect::getParameter(const std::string& name) const {
    if (name == "time") return time;
    if (name == "size") return size;
    if (name == "diffusion") return diffusion;
    if (name == "entanglement") return entanglement;
    if (name == "superposition") return superposition;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
    return 0.0f;
}

void QuantumReverbEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "time") automatedTime = automated;
    else if (name == "size") automatedSize = automated;
    else if (name == "diffusion") automatedDiffusion = automated;
    else if (name == "entanglement") automatedEntanglement = automated;
    else if (name == "superposition") automatedSuperposition = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool QuantumReverbEffect::isParameterAutomated(const std::string& name) const {
    if (name == "time") return automatedTime;
    if (name == "size") return automatedSize;
    if (name == "diffusion") return automatedDiffusion;
    if (name == "entanglement") return automatedEntanglement;
    if (name == "superposition") return automatedSuperposition;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    return false;
}

void QuantumReverbEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        time = 0.5f;
        size = 0.3f;
        diffusion = 0.6f;
        entanglement = 0.5f;
        superposition = 0.4f;
        mix = 0.5f;
        quality = 1.0f;
    }
    else if (presetName == "Quantum") {
        time = 0.7f;
        size = 0.5f;
        diffusion = 0.8f;
        entanglement = 0.7f;
        superposition = 0.6f;
        mix = 0.7f;
        quality = 1.0f;
    }
    else if (presetName == "Subtle") {
        time = 0.3f;
        size = 0.2f;
        diffusion = 0.4f;
        entanglement = 0.3f;
        superposition = 0.2f;
        mix = 0.3f;
        quality = 1.0f;
    }
    
    updateStates();
}

void QuantumReverbEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> QuantumReverbEffect::getAvailablePresets() const {
    return {"Standard", "Quantum", "Subtle"};
}

} // namespace VR_DAW 