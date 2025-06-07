#include "TimeWarpEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

TimeWarpEffect::TimeWarpEffect()
    : speed(0.5f)
    , direction(0.5f)
    , distortion(0.4f)
    , glitch(0.3f)
    , stutter(0.2f)
    , mix(0.5f)
    , quality(1.0f)
    , automatedSpeed(false)
    , automatedDirection(false)
    , automatedDistortion(false)
    , automatedGlitch(false)
    , automatedStutter(false)
    , automatedMix(false)
    , automatedQuality(false)
    , bufferSize(44100 * 2) // 2 Sekunden bei 44.1kHz
{
    std::random_device rd;
    rng.seed(rd());
    initializeStates();
}

TimeWarpEffect::~TimeWarpEffect() {
    shutdown();
}

bool TimeWarpEffect::initialize() {
    initializeStates();
    return true;
}

void TimeWarpEffect::shutdown() {
    states.clear();
}

void TimeWarpEffect::initializeStates() {
    states.clear();
    
    // Erstelle 4 Warp-Zustände für komplexe Klangtransformation
    for (int i = 0; i < 4; ++i) {
        WarpState state;
        
        // Erstelle 8 Zeit-Zustände pro Warp
        for (int j = 0; j < 8; ++j) {
            TimeState tstate;
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            
            tstate.buffer.resize(bufferSize, 0.0f);
            tstate.position = dist(rng);
            tstate.speed = speed;
            tstate.direction = direction;
            
            state.states.push_back(tstate);
        }
        
        state.time = 0.0f;
        state.glitch = glitch;
        
        states.push_back(state);
    }
}

void TimeWarpEffect::updateStates() {
    // Aktualisiere Zustände basierend auf Parametern
    for (auto& state : states) {
        state.glitch = glitch;
        
        for (auto& tstate : state.states) {
            tstate.speed = speed;
            tstate.direction = direction;
            
            // Aktualisiere Position
            tstate.position += tstate.speed * tstate.direction * 0.01f;
            
            // Begrenze Position
            if (tstate.position > 1.0f) tstate.position -= 1.0f;
            if (tstate.position < 0.0f) tstate.position += 1.0f;
        }
        
        state.time += 1.0f / 44100.0f;
    }
}

void TimeWarpEffect::processState(WarpState& state, float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float output = buffer[i];
        
        // Verarbeite jeden Zeit-Zustand
        for (auto& tstate : state.states) {
            // Berechne Position im Buffer
            float position = tstate.position * bufferSize;
            unsigned long index = static_cast<unsigned long>(position);
            
            if (index < tstate.buffer.size()) {
                // Hole Sample aus Buffer
                float sample = tstate.buffer[index];
                
                // Wende Geschwindigkeit an
                sample *= tstate.speed;
                
                // Wende Richtung an
                if (tstate.direction < 0.5f) {
                    sample = -sample;
                }
                
                output += sample;
            }
            
            // Aktualisiere Buffer
            tstate.buffer.push_back(output);
            tstate.buffer.pop_front();
        }
        
        buffer[i] = output;
    }
}

void TimeWarpEffect::applyDistortion(float* buffer, unsigned long framesPerBuffer) {
    if (distortion <= 0.0f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float sample = buffer[i];
        float distorted = std::tanh(sample * (1.0f + distortion * 10.0f));
        buffer[i] = sample + (distorted - sample) * distortion;
    }
}

void TimeWarpEffect::applyGlitch(float* buffer, unsigned long framesPerBuffer) {
    if (glitch <= 0.0f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        if (std::rand() / RAND_MAX < glitch) {
            // Erstelle Glitch
            std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
            float glitchSample = dist(rng);
            
            // Wende Glitch an
            buffer[i] = buffer[i] * (1.0f - glitch) + glitchSample * glitch;
        }
    }
}

void TimeWarpEffect::applyStutter(float* buffer, unsigned long framesPerBuffer) {
    if (stutter <= 0.0f) return;
    
    unsigned long stutterLength = static_cast<unsigned long>(stutter * 1000); // 1000 Samples max
    
    for (unsigned long i = 0; i < framesPerBuffer; i += stutterLength) {
        if (i + stutterLength < framesPerBuffer) {
            // Wiederhole Samples
            for (unsigned long j = 0; j < stutterLength; ++j) {
                buffer[i + j] = buffer[i];
            }
        }
    }
}

void TimeWarpEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateStates();
    
    // Verarbeite jeden Zustand
    for (auto& state : states) {
        processState(state, buffer, framesPerBuffer);
    }
    
    // Wende Effekte an
    applyDistortion(buffer, framesPerBuffer);
    applyGlitch(buffer, framesPerBuffer);
    applyStutter(buffer, framesPerBuffer);
    
    // Wende Mix an
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = buffer[i] * mix;
    }
}

std::vector<PluginParameter> TimeWarpEffect::getParameters() const {
    return {
        {"speed", speed, 0.0f, 1.0f, ""},
        {"direction", direction, 0.0f, 1.0f, ""},
        {"distortion", distortion, 0.0f, 1.0f, ""},
        {"glitch", glitch, 0.0f, 1.0f, ""},
        {"stutter", stutter, 0.0f, 1.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void TimeWarpEffect::setParameter(const std::string& name, float value) {
    if (name == "speed") speed = value;
    else if (name == "direction") direction = value;
    else if (name == "distortion") distortion = value;
    else if (name == "glitch") glitch = value;
    else if (name == "stutter") stutter = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
}

float TimeWarpEffect::getParameter(const std::string& name) const {
    if (name == "speed") return speed;
    if (name == "direction") return direction;
    if (name == "distortion") return distortion;
    if (name == "glitch") return glitch;
    if (name == "stutter") return stutter;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
    return 0.0f;
}

void TimeWarpEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "speed") automatedSpeed = automated;
    else if (name == "direction") automatedDirection = automated;
    else if (name == "distortion") automatedDistortion = automated;
    else if (name == "glitch") automatedGlitch = automated;
    else if (name == "stutter") automatedStutter = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool TimeWarpEffect::isParameterAutomated(const std::string& name) const {
    if (name == "speed") return automatedSpeed;
    if (name == "direction") return automatedDirection;
    if (name == "distortion") return automatedDistortion;
    if (name == "glitch") return automatedGlitch;
    if (name == "stutter") return automatedStutter;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    return false;
}

void TimeWarpEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        speed = 0.5f;
        direction = 0.5f;
        distortion = 0.4f;
        glitch = 0.3f;
        stutter = 0.2f;
        mix = 0.5f;
        quality = 1.0f;
    }
    else if (presetName == "Heavy") {
        speed = 0.7f;
        direction = 0.7f;
        distortion = 0.6f;
        glitch = 0.5f;
        stutter = 0.4f;
        mix = 0.7f;
        quality = 1.0f;
    }
    else if (presetName == "Subtle") {
        speed = 0.3f;
        direction = 0.3f;
        distortion = 0.2f;
        glitch = 0.1f;
        stutter = 0.1f;
        mix = 0.3f;
        quality = 1.0f;
    }
    
    updateStates();
}

void TimeWarpEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> TimeWarpEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 