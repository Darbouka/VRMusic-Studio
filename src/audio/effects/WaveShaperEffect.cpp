#include "WaveShaperEffect.hpp"
#include <cmath>

namespace VRMusicStudio {

WaveShaperEffect::WaveShaperEffect() :
    drive(0.5f),
    bias(0.0f),
    mix(0.5f),
    tone(0.5f),
    symmetry(0.5f),
    quality(1.0f),
    automatedDrive(false),
    automatedBias(false),
    automatedMix(false),
    automatedTone(false),
    automatedSymmetry(false),
    automatedQuality(false),
    currentDrive(0.5f),
    currentBias(0.0f),
    currentMix(0.5f),
    currentTone(0.5f),
    currentSymmetry(0.5f),
    currentQuality(1.0f)
{
}

WaveShaperEffect::~WaveShaperEffect() {
    shutdown();
}

bool WaveShaperEffect::initialize() {
    initializeBuffers();
    return true;
}

void WaveShaperEffect::shutdown() {
    driveBuffer.clear();
    biasBuffer.clear();
    mixBuffer.clear();
    toneBuffer.clear();
    symmetryBuffer.clear();
    qualityBuffer.clear();
}

std::vector<PluginParameter> WaveShaperEffect::getParameters() const {
    return {
        {"drive", 0.5f, 0.0f, 1.0f},
        {"bias", 0.0f, -1.0f, 1.0f},
        {"mix", 0.5f, 0.0f, 1.0f},
        {"tone", 0.5f, 0.0f, 1.0f},
        {"symmetry", 0.5f, 0.0f, 1.0f},
        {"quality", 1.0f, 0.0f, 1.0f}
    };
}

void WaveShaperEffect::setParameter(const std::string& name, float value) {
    if (name == "drive") drive = value;
    else if (name == "bias") bias = value;
    else if (name == "mix") mix = value;
    else if (name == "tone") tone = value;
    else if (name == "symmetry") symmetry = value;
    else if (name == "quality") quality = value;
}

float WaveShaperEffect::getParameter(const std::string& name) const {
    if (name == "drive") return drive;
    else if (name == "bias") return bias;
    else if (name == "mix") return mix;
    else if (name == "tone") return tone;
    else if (name == "symmetry") return symmetry;
    else if (name == "quality") return quality;
    return 0.0f;
}

void WaveShaperEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "drive") automatedDrive = automated;
    else if (name == "bias") automatedBias = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "tone") automatedTone = automated;
    else if (name == "symmetry") automatedSymmetry = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool WaveShaperEffect::isParameterAutomated(const std::string& name) const {
    if (name == "drive") return automatedDrive;
    else if (name == "bias") return automatedBias;
    else if (name == "mix") return automatedMix;
    else if (name == "tone") return automatedTone;
    else if (name == "symmetry") return automatedSymmetry;
    else if (name == "quality") return automatedQuality;
    return false;
}

void WaveShaperEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateParameters();
    processDrive(buffer, framesPerBuffer);
    processBias(buffer, framesPerBuffer);
    processMix(buffer, framesPerBuffer);
    processTone(buffer, framesPerBuffer);
    processSymmetry(buffer, framesPerBuffer);
    processQuality(buffer, framesPerBuffer);
}

void WaveShaperEffect::loadPreset(const std::string& presetName) {
    // TODO: Implement preset loading
}

void WaveShaperEffect::savePreset(const std::string& presetName) {
    // TODO: Implement preset saving
}

std::vector<std::string> WaveShaperEffect::getAvailablePresets() const {
    return {}; // TODO: Implement preset list
}

void WaveShaperEffect::updateParameters() {
    currentDrive = drive;
    currentBias = bias;
    currentMix = mix;
    currentTone = tone;
    currentSymmetry = symmetry;
    currentQuality = quality;
}

void WaveShaperEffect::processDrive(float* buffer, unsigned long framesPerBuffer) {
    if (currentDrive == 0.5f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = calculateDriveCurve(buffer[i], currentDrive);
    }
}

void WaveShaperEffect::processBias(float* buffer, unsigned long framesPerBuffer) {
    if (currentBias == 0.0f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = calculateBiasCurve(buffer[i], currentBias);
    }
}

void WaveShaperEffect::processMix(float* buffer, unsigned long framesPerBuffer) {
    if (currentMix == 0.5f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] *= currentMix;
    }
}

void WaveShaperEffect::processTone(float* buffer, unsigned long framesPerBuffer) {
    if (currentTone == 0.5f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = calculateToneCurve(buffer[i], currentTone);
    }
}

void WaveShaperEffect::processSymmetry(float* buffer, unsigned long framesPerBuffer) {
    if (currentSymmetry == 0.5f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = calculateSymmetryCurve(buffer[i], currentSymmetry);
    }
}

void WaveShaperEffect::processQuality(float* buffer, unsigned long framesPerBuffer) {
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

void WaveShaperEffect::initializeBuffers() {
    const unsigned long bufferSize = 4096;
    driveBuffer.resize(bufferSize, 0.0f);
    biasBuffer.resize(bufferSize, 0.0f);
    mixBuffer.resize(bufferSize, 0.0f);
    toneBuffer.resize(bufferSize, 0.0f);
    symmetryBuffer.resize(bufferSize, 0.0f);
    qualityBuffer.resize(bufferSize, 0.0f);
}

float WaveShaperEffect::calculateDriveCurve(float sample, float drive) {
    // Soft clipping with drive control
    float driveFactor = 1.0f + drive * 10.0f;
    return std::tanh(sample * driveFactor);
}

float WaveShaperEffect::calculateBiasCurve(float sample, float bias) {
    // DC offset with bias control
    return sample + bias;
}

float WaveShaperEffect::calculateToneCurve(float sample, float tone) {
    // Tone control using a simple filter
    float cutoff = 1000.0f + tone * 19000.0f;
    float alpha = 1.0f / (1.0f + std::exp((std::abs(sample) - cutoff) / 1000.0f));
    return sample * alpha;
}

float WaveShaperEffect::calculateSymmetryCurve(float sample, float symmetry) {
    // Asymmetric distortion
    float positive = std::tanh(sample * (1.0f + symmetry));
    float negative = std::tanh(sample * (1.0f - symmetry));
    return (positive + negative) * 0.5f;
}

} // namespace VRMusicStudio 