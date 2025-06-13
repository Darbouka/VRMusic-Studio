#include "VibratoEffect.hpp"
#include <cmath>

namespace VRMusicStudio {

VibratoEffect::VibratoEffect() :
    rate(5.0f),
    depth(0.5f),
    mix(0.5f),
    stereoWidth(0.5f),
    syncRate(0.25f),
    shape(0.5f),
    phase(0.0f),
    automatedRate(false),
    automatedDepth(false),
    automatedMix(false),
    automatedStereoWidth(false),
    automatedSyncRate(false),
    automatedShape(false),
    automatedPhase(false),
    currentRate(5.0f),
    currentDepth(0.5f),
    currentMix(0.5f),
    currentStereoWidth(0.5f),
    currentSyncRate(0.25f),
    currentShape(0.5f),
    currentPhase(0.0f),
    phaseAccumulator(0.0f),
    stereoPhaseAccumulator(0.0f)
{
}

VibratoEffect::~VibratoEffect() {
    shutdown();
}

bool VibratoEffect::initialize() {
    updateParameters();
    return true;
}

void VibratoEffect::shutdown() {
    // Cleanup if needed
}

std::vector<PluginParameter> VibratoEffect::getParameters() const {
    return {
        {"rate", 5.0f, 0.1f, 10.0f},
        {"depth", 0.5f, 0.0f, 1.0f},
        {"mix", 0.5f, 0.0f, 1.0f},
        {"stereoWidth", 0.5f, 0.0f, 1.0f},
        {"syncRate", 0.25f, 0.0625f, 1.0f},
        {"shape", 0.5f, 0.0f, 1.0f},
        {"phase", 0.0f, 0.0f, 1.0f}
    };
}

void VibratoEffect::setParameter(const std::string& name, float value) {
    if (name == "rate") rate = value;
    else if (name == "depth") depth = value;
    else if (name == "mix") mix = value;
    else if (name == "stereoWidth") stereoWidth = value;
    else if (name == "syncRate") syncRate = value;
    else if (name == "shape") shape = value;
    else if (name == "phase") phase = value;
}

float VibratoEffect::getParameter(const std::string& name) const {
    if (name == "rate") return rate;
    else if (name == "depth") return depth;
    else if (name == "mix") return mix;
    else if (name == "stereoWidth") return stereoWidth;
    else if (name == "syncRate") return syncRate;
    else if (name == "shape") return shape;
    else if (name == "phase") return phase;
    return 0.0f;
}

void VibratoEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "rate") automatedRate = automated;
    else if (name == "depth") automatedDepth = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "stereoWidth") automatedStereoWidth = automated;
    else if (name == "syncRate") automatedSyncRate = automated;
    else if (name == "shape") automatedShape = automated;
    else if (name == "phase") automatedPhase = automated;
}

bool VibratoEffect::isParameterAutomated(const std::string& name) const {
    if (name == "rate") return automatedRate;
    else if (name == "depth") return automatedDepth;
    else if (name == "mix") return automatedMix;
    else if (name == "stereoWidth") return automatedStereoWidth;
    else if (name == "syncRate") return automatedSyncRate;
    else if (name == "shape") return automatedShape;
    else if (name == "phase") return automatedPhase;
    return false;
}

void VibratoEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateParameters();

    if (currentStereoWidth > 0.0f) {
        processStereo(buffer, framesPerBuffer);
    } else {
        processMono(buffer, framesPerBuffer);
    }
}

void VibratoEffect::loadPreset(const std::string& presetName) {
    // TODO: Implement preset loading
}

void VibratoEffect::savePreset(const std::string& presetName) {
    // TODO: Implement preset saving
}

std::vector<std::string> VibratoEffect::getAvailablePresets() const {
    return {}; // TODO: Implement preset list
}

void VibratoEffect::updateParameters() {
    currentRate = rate;
    currentDepth = depth;
    currentMix = mix;
    currentStereoWidth = stereoWidth;
    currentSyncRate = syncRate;
    currentShape = shape;
    currentPhase = phase;
}

float VibratoEffect::calculateModulation(float phase) {
    float modulation;
    
    if (currentShape < 0.33f) {
        // Sine wave
        modulation = std::sin(phase * 2.0f * M_PI);
    } else if (currentShape < 0.66f) {
        // Triangle wave
        modulation = 2.0f * std::abs(2.0f * (phase - std::floor(phase + 0.5f))) - 1.0f;
    } else {
        // Square wave
        modulation = phase < 0.5f ? 1.0f : -1.0f;
    }
    
    return modulation * currentDepth;
}

void VibratoEffect::processMono(float* buffer, unsigned long framesPerBuffer) {
    const float sampleRate = 44100.0f;
    const float phaseIncrement = currentRate / sampleRate;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float modulation = calculateModulation(phaseAccumulator);
        float delay = modulation * 0.01f; // Max 10ms delay
        
        // Simple delay line implementation
        int delaySamples = static_cast<int>(delay * sampleRate);
        int readIndex = i - delaySamples;
        
        float delayedSample = (readIndex >= 0) ? buffer[readIndex] : 0.0f;
        
        // Mix original and delayed signal
        buffer[i] = buffer[i] * (1.0f - currentMix) + delayedSample * currentMix;
        
        phaseAccumulator += phaseIncrement;
        if (phaseAccumulator >= 1.0f) {
            phaseAccumulator -= 1.0f;
        }
    }
}

void VibratoEffect::processStereo(float* buffer, unsigned long framesPerBuffer) {
    const float sampleRate = 44100.0f;
    const float phaseIncrement = currentRate / sampleRate;
    const float stereoPhaseOffset = currentStereoWidth * 0.5f;
    
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Left channel
        float leftModulation = calculateModulation(phaseAccumulator);
        float leftDelay = leftModulation * 0.01f;
        int leftDelaySamples = static_cast<int>(leftDelay * sampleRate);
        int leftReadIndex = i - leftDelaySamples;
        float leftDelayedSample = (leftReadIndex >= 0) ? buffer[leftReadIndex] : 0.0f;
        
        // Right channel
        float rightModulation = calculateModulation(stereoPhaseAccumulator);
        float rightDelay = rightModulation * 0.01f;
        int rightDelaySamples = static_cast<int>(rightDelay * sampleRate);
        int rightReadIndex = i + 1 - rightDelaySamples;
        float rightDelayedSample = (rightReadIndex >= 0) ? buffer[rightReadIndex] : 0.0f;
        
        // Mix original and delayed signals
        buffer[i] = buffer[i] * (1.0f - currentMix) + leftDelayedSample * currentMix;
        buffer[i + 1] = buffer[i + 1] * (1.0f - currentMix) + rightDelayedSample * currentMix;
        
        phaseAccumulator += phaseIncrement;
        stereoPhaseAccumulator += phaseIncrement;
        
        if (phaseAccumulator >= 1.0f) {
            phaseAccumulator -= 1.0f;
        }
        if (stereoPhaseAccumulator >= 1.0f) {
            stereoPhaseAccumulator -= 1.0f;
        }
    }
}

} // namespace VRMusicStudio 