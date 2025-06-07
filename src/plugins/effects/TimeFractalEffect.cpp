#include "TimeFractalEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

TimeFractalEffect::TimeFractalEffect()
    : time(0.5f)
    , depth(0.5f)
    , feedback(0.5f)
    , chaos(0.5f)
    , dimension(0.5f)
    , mix(0.5f)
    , quality(1.0f)
    , automatedTime(false)
    , automatedDepth(false)
    , automatedFeedback(false)
    , automatedChaos(false)
    , automatedDimension(false)
    , automatedMix(false)
    , automatedQuality(false)
    , bufferSize(44100 * 2) // 2 Sekunden bei 44.1kHz
{
    std::random_device rd;
    rng.seed(rd());
    initializeStates();
}

TimeFractalEffect::~TimeFractalEffect() {
    shutdown();
}

bool TimeFractalEffect::initialize() {
    initializeStates();
    return true;
}

void TimeFractalEffect::shutdown() {
    states.clear();
}

void TimeFractalEffect::initializeStates() {
    states.clear();
    
    // Erstelle 4 Zeit-Zustände
    for (int i = 0; i < 4; ++i) {
        TimeState state;
        
        // Erstelle 8 Fraktal-Zustände pro Zeit-Zustand
        for (int j = 0; j < 8; ++j) {
            FractalState fractal;
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            
            fractal.position = dist(rng);
            fractal.phase = dist(rng) * 2.0f * M_PI;
            fractal.amplitude = dist(rng);
            fractal.frequency = dist(rng) * 0.1f;
            
            state.fractals.push_back(fractal);
        }
        
        state.time = time;
        states.push_back(state);
    }
}

void TimeFractalEffect::updateStates() {
    // Aktualisiere Zeit-Zustände
    for (auto& state : states) {
        state.time = time;
        
        // Aktualisiere Fraktal-Zustände
        for (auto& fractal : state.fractals) {
            // Aktualisiere Position basierend auf Frequenz
            fractal.position += fractal.frequency * 0.001f;
            fractal.position = std::fmod(fractal.position + 1.0f, 1.0f);
            
            // Aktualisiere Phase
            fractal.phase += fractal.frequency * 0.0001f;
            fractal.phase = std::fmod(fractal.phase, 2.0f * M_PI);
            
            // Aktualisiere Amplitude basierend auf Chaos
            fractal.amplitude += chaos * (std::rand() / RAND_MAX - 0.5f) * 0.01f;
            fractal.amplitude = std::max(0.0f, std::min(1.0f, fractal.amplitude));
        }
    }
}

void TimeFractalEffect::processState(TimeState& state, float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float t = static_cast<float>(i) / framesPerBuffer;
        float output = buffer[i];
        
        // Verarbeite jeden Fraktal-Zustand
        for (auto& fractal : state.fractals) {
            // Berechne Fraktal-Wert
            float value = output * fractal.amplitude;
            value *= std::sin(2.0f * M_PI * fractal.position * t + fractal.phase);
            
            // Modifiziere Output
            output += value * state.time;
        }
        
        // Normalisiere Output
        output /= (state.fractals.size() + 1);
        
        buffer[i] = output;
    }
}

void TimeFractalEffect::applyTime(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float t = static_cast<float>(i) / framesPerBuffer;
        buffer[i] *= (1.0f + time * std::sin(2.0f * M_PI * t));
    }
}

void TimeFractalEffect::applyDepth(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] *= (1.0f + depth);
    }
}

void TimeFractalEffect::applyFeedback(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float t = static_cast<float>(i) / framesPerBuffer;
        buffer[i] += feedback * buffer[static_cast<unsigned long>(t * framesPerBuffer)];
    }
}

void TimeFractalEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateStates();
    
    // Verarbeite jeden Zeit-Zustand
    for (auto& state : states) {
        processState(state, buffer, framesPerBuffer);
    }
    
    // Wende Effekte an
    applyTime(buffer, framesPerBuffer);
    applyDepth(buffer, framesPerBuffer);
    applyFeedback(buffer, framesPerBuffer);
    
    // Wende Mix an
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = buffer[i] * mix;
    }
}

std::vector<PluginParameter> TimeFractalEffect::getParameters() const {
    return {
        {"time", time, 0.0f, 1.0f, ""},
        {"depth", depth, 0.0f, 1.0f, ""},
        {"feedback", feedback, 0.0f, 1.0f, ""},
        {"chaos", chaos, 0.0f, 1.0f, ""},
        {"dimension", dimension, 0.0f, 1.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void TimeFractalEffect::setParameter(const std::string& name, float value) {
    if (name == "time") time = value;
    else if (name == "depth") depth = value;
    else if (name == "feedback") feedback = value;
    else if (name == "chaos") chaos = value;
    else if (name == "dimension") dimension = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
}

float TimeFractalEffect::getParameter(const std::string& name) const {
    if (name == "time") return time;
    if (name == "depth") return depth;
    if (name == "feedback") return feedback;
    if (name == "chaos") return chaos;
    if (name == "dimension") return dimension;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
    return 0.0f;
}

void TimeFractalEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "time") automatedTime = automated;
    else if (name == "depth") automatedDepth = automated;
    else if (name == "feedback") automatedFeedback = automated;
    else if (name == "chaos") automatedChaos = automated;
    else if (name == "dimension") automatedDimension = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool TimeFractalEffect::isParameterAutomated(const std::string& name) const {
    if (name == "time") return automatedTime;
    if (name == "depth") return automatedDepth;
    if (name == "feedback") return automatedFeedback;
    if (name == "chaos") return automatedChaos;
    if (name == "dimension") return automatedDimension;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    return false;
}

void TimeFractalEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        time = 0.5f;
        depth = 0.5f;
        feedback = 0.5f;
        chaos = 0.5f;
        dimension = 0.5f;
        mix = 0.5f;
        quality = 1.0f;
    }
    else if (presetName == "Chaotic") {
        time = 0.7f;
        depth = 0.8f;
        feedback = 0.7f;
        chaos = 0.8f;
        dimension = 0.7f;
        mix = 0.7f;
        quality = 1.0f;
    }
    else if (presetName == "Subtle") {
        time = 0.3f;
        depth = 0.3f;
        feedback = 0.3f;
        chaos = 0.3f;
        dimension = 0.3f;
        mix = 0.3f;
        quality = 1.0f;
    }
    
    updateStates();
}

void TimeFractalEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> TimeFractalEffect::getAvailablePresets() const {
    return {"Standard", "Chaotic", "Subtle"};
}

} // namespace VR_DAW 