#include "PhaserEffect.hpp"
#include <cmath>

namespace VRMusicStudio {

PhaserEffect::PhaserEffect() :
    rate(1.0f),
    depth(0.5f),
    feedback(0.5f),
    mix(0.5f),
    stereoWidth(0.5f),
    syncRate(0.25f),
    stages(4.0f),
    phase(0.0f),
    automatedRate(false),
    automatedDepth(false),
    automatedFeedback(false),
    automatedMix(false),
    automatedStereoWidth(false),
    automatedSyncRate(false),
    automatedStages(false),
    automatedPhase(false),
    currentRate(1.0f),
    currentDepth(0.5f),
    currentFeedback(0.5f),
    currentMix(0.5f),
    currentStereoWidth(0.5f),
    currentSyncRate(0.25f),
    currentStages(4.0f),
    currentPhase(0.0f),
    phaseAccumulator(0.0f),
    stereoPhaseAccumulator(0.0f)
{
}

PhaserEffect::~PhaserEffect() {
    shutdown();
}

bool PhaserEffect::initialize() {
    initializeAllpassFilters();
    updateParameters();
    return true;
}

void PhaserEffect::shutdown() {
    allpassBuffers.clear();
    stereoAllpassBuffers.clear();
}

std::vector<PluginParameter> PhaserEffect::getParameters() const {
    return {
        {"rate", 1.0f, 0.1f, 10.0f},
        {"depth", 0.5f, 0.0f, 1.0f},
        {"feedback", 0.5f, 0.0f, 0.9f},
        {"mix", 0.5f, 0.0f, 1.0f},
        {"stereoWidth", 0.5f, 0.0f, 1.0f},
        {"syncRate", 0.25f, 0.0625f, 1.0f},
        {"stages", 4.0f, 2.0f, 12.0f},
        {"phase", 0.0f, 0.0f, 1.0f}
    };
}

void PhaserEffect::setParameter(const std::string& name, float value) {
    if (name == "rate") rate = value;
    else if (name == "depth") depth = value;
    else if (name == "feedback") feedback = value;
    else if (name == "mix") mix = value;
    else if (name == "stereoWidth") stereoWidth = value;
    else if (name == "syncRate") syncRate = value;
    else if (name == "stages") stages = value;
    else if (name == "phase") phase = value;
}

float PhaserEffect::getParameter(const std::string& name) const {
    if (name == "rate") return rate;
    else if (name == "depth") return depth;
    else if (name == "feedback") return feedback;
    else if (name == "mix") return mix;
    else if (name == "stereoWidth") return stereoWidth;
    else if (name == "syncRate") return syncRate;
    else if (name == "stages") return stages;
    else if (name == "phase") return phase;
    return 0.0f;
}

void PhaserEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "rate") automatedRate = automated;
    else if (name == "depth") automatedDepth = automated;
    else if (name == "feedback") automatedFeedback = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "stereoWidth") automatedStereoWidth = automated;
    else if (name == "syncRate") automatedSyncRate = automated;
    else if (name == "stages") automatedStages = automated;
    else if (name == "phase") automatedPhase = automated;
}

bool PhaserEffect::isParameterAutomated(const std::string& name) const {
    if (name == "rate") return automatedRate;
    else if (name == "depth") return automatedDepth;
    else if (name == "feedback") return automatedFeedback;
    else if (name == "mix") return automatedMix;
    else if (name == "stereoWidth") return automatedStereoWidth;
    else if (name == "syncRate") return automatedSyncRate;
    else if (name == "stages") return automatedStages;
    else if (name == "phase") return automatedPhase;
    return false;
}

void PhaserEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateParameters();

    if (currentStereoWidth > 0.0f) {
        processStereo(buffer, framesPerBuffer);
    } else {
        processMono(buffer, framesPerBuffer);
    }
}

void PhaserEffect::loadPreset(const std::string& presetName) {
    // TODO: Implement preset loading
}

void PhaserEffect::savePreset(const std::string& presetName) {
    // TODO: Implement preset saving
}

std::vector<std::string> PhaserEffect::getAvailablePresets() const {
    return {}; // TODO: Implement preset list
}

void PhaserEffect::updateParameters() {
    currentRate = rate;
    currentDepth = depth;
    currentFeedback = feedback;
    currentMix = mix;
    currentStereoWidth = stereoWidth;
    currentSyncRate = syncRate;
    currentStages = stages;
    currentPhase = phase;
}

float PhaserEffect::calculateModulation(float phase) {
    return (std::sin(phase * 2.0f * M_PI) + 1.0f) * 0.5f * currentDepth;
}

void PhaserEffect::initializeAllpassFilters() {
    int numStages = static_cast<int>(currentStages);
    allpassBuffers.resize(numStages, 0.0f);
    stereoAllpassBuffers.resize(numStages, 0.0f);
}

void PhaserEffect::processMono(float* buffer, unsigned long framesPerBuffer) {
    const float sampleRate = 44100.0f;
    const float phaseIncrement = currentRate / sampleRate;
    const int numStages = static_cast<int>(currentStages);
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float modulation = calculateModulation(phaseAccumulator);
        float sample = buffer[i];
        float feedbackSample = 0.0f;
        
        // Process through allpass filters
        for (int stage = 0; stage < numStages; ++stage) {
            float allpassCoeff = 0.5f + modulation * 0.5f;
            float allpassInput = sample + feedbackSample * currentFeedback;
            float allpassOutput = allpassCoeff * (allpassInput - allpassBuffers[stage]) + allpassBuffers[stage];
            allpassBuffers[stage] = allpassInput - allpassCoeff * allpassOutput;
            feedbackSample = allpassOutput;
        }
        
        // Mix original and processed signal
        buffer[i] = sample * (1.0f - currentMix) + feedbackSample * currentMix;
        
        phaseAccumulator += phaseIncrement;
        if (phaseAccumulator >= 1.0f) {
            phaseAccumulator -= 1.0f;
        }
    }
}

void PhaserEffect::processStereo(float* buffer, unsigned long framesPerBuffer) {
    const float sampleRate = 44100.0f;
    const float phaseIncrement = currentRate / sampleRate;
    const float stereoPhaseOffset = currentStereoWidth * 0.5f;
    const int numStages = static_cast<int>(currentStages);
    
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Left channel
        float leftModulation = calculateModulation(phaseAccumulator);
        float leftSample = buffer[i];
        float leftFeedbackSample = 0.0f;
        
        // Right channel
        float rightModulation = calculateModulation(stereoPhaseAccumulator);
        float rightSample = buffer[i + 1];
        float rightFeedbackSample = 0.0f;
        
        // Process through allpass filters
        for (int stage = 0; stage < numStages; ++stage) {
            // Left channel
            float leftAllpassCoeff = 0.5f + leftModulation * 0.5f;
            float leftAllpassInput = leftSample + leftFeedbackSample * currentFeedback;
            float leftAllpassOutput = leftAllpassCoeff * (leftAllpassInput - allpassBuffers[stage]) + allpassBuffers[stage];
            allpassBuffers[stage] = leftAllpassInput - leftAllpassCoeff * leftAllpassOutput;
            leftFeedbackSample = leftAllpassOutput;
            
            // Right channel
            float rightAllpassCoeff = 0.5f + rightModulation * 0.5f;
            float rightAllpassInput = rightSample + rightFeedbackSample * currentFeedback;
            float rightAllpassOutput = rightAllpassCoeff * (rightAllpassInput - stereoAllpassBuffers[stage]) + stereoAllpassBuffers[stage];
            stereoAllpassBuffers[stage] = rightAllpassInput - rightAllpassCoeff * rightAllpassOutput;
            rightFeedbackSample = rightAllpassOutput;
        }
        
        // Mix original and processed signals
        buffer[i] = leftSample * (1.0f - currentMix) + leftFeedbackSample * currentMix;
        buffer[i + 1] = rightSample * (1.0f - currentMix) + rightFeedbackSample * currentMix;
        
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