#include "RingModulatorEffect.hpp"
#include <cmath>

namespace VRMusicStudio {

RingModulatorEffect::RingModulatorEffect() :
    frequency(440.0f),
    depth(0.5f),
    mix(0.5f),
    stereoWidth(0.5f),
    syncRate(1.0f),
    shape(0.5f),
    quality(1.0f),
    automatedFrequency(false),
    automatedDepth(false),
    automatedMix(false),
    automatedStereoWidth(false),
    automatedSyncRate(false),
    automatedShape(false),
    automatedQuality(false),
    phase(0.0f),
    stereoPhase(0.0f),
    lastFrequency(440.0f),
    currentFrequency(440.0f),
    currentDepth(0.5f),
    currentMix(0.5f),
    currentStereoWidth(0.5f),
    currentSyncRate(1.0f),
    currentShape(0.5f),
    currentQuality(1.0f)
{
}

RingModulatorEffect::~RingModulatorEffect() {
    shutdown();
}

bool RingModulatorEffect::initialize() {
    return true;
}

void RingModulatorEffect::shutdown() {
}

std::vector<PluginParameter> RingModulatorEffect::getParameters() const {
    return {
        {"frequency", 440.0f, 20.0f, 20000.0f},
        {"depth", 0.5f, 0.0f, 1.0f},
        {"mix", 0.5f, 0.0f, 1.0f},
        {"stereoWidth", 0.5f, 0.0f, 1.0f},
        {"syncRate", 1.0f, 0.25f, 4.0f},
        {"shape", 0.5f, 0.0f, 1.0f},
        {"quality", 1.0f, 0.0f, 1.0f}
    };
}

void RingModulatorEffect::setParameter(const std::string& name, float value) {
    if (name == "frequency") frequency = value;
    else if (name == "depth") depth = value;
    else if (name == "mix") mix = value;
    else if (name == "stereoWidth") stereoWidth = value;
    else if (name == "syncRate") syncRate = value;
    else if (name == "shape") shape = value;
    else if (name == "quality") quality = value;
}

float RingModulatorEffect::getParameter(const std::string& name) const {
    if (name == "frequency") return frequency;
    else if (name == "depth") return depth;
    else if (name == "mix") return mix;
    else if (name == "stereoWidth") return stereoWidth;
    else if (name == "syncRate") return syncRate;
    else if (name == "shape") return shape;
    else if (name == "quality") return quality;
    return 0.0f;
}

void RingModulatorEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "frequency") automatedFrequency = automated;
    else if (name == "depth") automatedDepth = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "stereoWidth") automatedStereoWidth = automated;
    else if (name == "syncRate") automatedSyncRate = automated;
    else if (name == "shape") automatedShape = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool RingModulatorEffect::isParameterAutomated(const std::string& name) const {
    if (name == "frequency") return automatedFrequency;
    else if (name == "depth") return automatedDepth;
    else if (name == "mix") return automatedMix;
    else if (name == "stereoWidth") return automatedStereoWidth;
    else if (name == "syncRate") return automatedSyncRate;
    else if (name == "shape") return automatedShape;
    else if (name == "quality") return automatedQuality;
    return false;
}

void RingModulatorEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateParameters();
    processModulation(buffer, framesPerBuffer);
    applyDepth(buffer, framesPerBuffer);
    applyMix(buffer, framesPerBuffer);
    applyStereoWidth(buffer, framesPerBuffer);
    applySyncRate(buffer, framesPerBuffer);
    applyShape(buffer, framesPerBuffer);
    applyQuality(buffer, framesPerBuffer);
}

void RingModulatorEffect::loadPreset(const std::string& presetName) {
    // TODO: Implement preset loading
}

void RingModulatorEffect::savePreset(const std::string& presetName) {
    // TODO: Implement preset saving
}

std::vector<std::string> RingModulatorEffect::getAvailablePresets() const {
    return {}; // TODO: Implement preset list
}

void RingModulatorEffect::updateParameters() {
    currentFrequency = frequency;
    currentDepth = depth;
    currentMix = mix;
    currentStereoWidth = stereoWidth;
    currentSyncRate = syncRate;
    currentShape = shape;
    currentQuality = quality;
}

void RingModulatorEffect::processModulation(float* buffer, unsigned long framesPerBuffer) {
    const float sampleRate = 44100.0f;
    float phaseIncrement = currentFrequency / sampleRate;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float modSample = calculateModulation(buffer[i], phase);
        float stereoModSample = calculateStereoModulation(buffer[i], stereoPhase);
        
        buffer[i] = modSample * (1.0f - currentStereoWidth) + stereoModSample * currentStereoWidth;
        
        phase += phaseIncrement;
        if (phase >= 1.0f) {
            phase -= 1.0f;
        }
        
        stereoPhase += phaseIncrement * 1.5f; // Slightly different phase for stereo
        if (stereoPhase >= 1.0f) {
            stereoPhase -= 1.0f;
        }
    }
}

void RingModulatorEffect::applyDepth(float* buffer, unsigned long framesPerBuffer) {
    if (currentDepth == 0.5f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] *= currentDepth;
    }
}

void RingModulatorEffect::applyMix(float* buffer, unsigned long framesPerBuffer) {
    if (currentMix == 0.5f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] *= currentMix;
    }
}

void RingModulatorEffect::applyStereoWidth(float* buffer, unsigned long framesPerBuffer) {
    if (currentStereoWidth == 0.5f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        float mid = (buffer[i] + buffer[i + 1]) * 0.5f;
        float side = (buffer[i] - buffer[i + 1]) * 0.5f;
        
        side *= currentStereoWidth;
        
        buffer[i] = mid + side;
        buffer[i + 1] = mid - side;
    }
}

void RingModulatorEffect::applySyncRate(float* buffer, unsigned long framesPerBuffer) {
    if (currentSyncRate == 1.0f) return;
    
    const float sampleRate = 44100.0f;
    float phaseIncrement = currentFrequency * currentSyncRate / sampleRate;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float modSample = calculateModulation(buffer[i], phase);
        buffer[i] = modSample;
        
        phase += phaseIncrement;
        if (phase >= 1.0f) {
            phase -= 1.0f;
        }
    }
}

void RingModulatorEffect::applyShape(float* buffer, unsigned long framesPerBuffer) {
    if (currentShape == 0.5f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = calculateShape(buffer[i], currentShape);
    }
}

void RingModulatorEffect::applyQuality(float* buffer, unsigned long framesPerBuffer) {
    if (currentQuality >= 1.0f) return;
    
    // Simple low-pass filter based on quality
    float alpha = currentQuality;
    float prev = buffer[0];
    
    for (unsigned long i = 1; i < framesPerBuffer; ++i) {
        float current = buffer[i];
        buffer[i] = prev + alpha * (current - prev);
        prev = buffer[i];
    }
}

float RingModulatorEffect::calculateModulation(float sample, float modPhase) {
    return sample * std::sin(2.0f * M_PI * modPhase);
}

float RingModulatorEffect::calculateStereoModulation(float sample, float modPhase) {
    return sample * std::cos(2.0f * M_PI * modPhase);
}

float RingModulatorEffect::calculateShape(float sample, float shape) {
    // Shape control using different waveforms
    float sine = std::sin(2.0f * M_PI * sample);
    float square = sample > 0.0f ? 1.0f : -1.0f;
    float triangle = 2.0f * std::abs(2.0f * (sample - std::floor(sample + 0.5f))) - 1.0f;
    
    return sine * (1.0f - shape) + square * shape * 0.5f + triangle * shape * 0.5f;
}

} // namespace VRMusicStudio 