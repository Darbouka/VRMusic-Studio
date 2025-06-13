#include "VoiceEditorEffect.hpp"
#include <cmath>

namespace VRMusicStudio {

VoiceEditorEffect::VoiceEditorEffect() :
    pitch(0.0f),
    formant(0.0f),
    gender(0.0f),
    breathiness(0.0f),
    clarity(0.0f),
    mix(0.5f),
    quality(1.0f),
    automatedPitch(false),
    automatedFormant(false),
    automatedGender(false),
    automatedBreathiness(false),
    automatedClarity(false),
    automatedMix(false),
    automatedQuality(false),
    currentPitch(0.0f),
    currentFormant(0.0f),
    currentGender(0.0f),
    currentBreathiness(0.0f),
    currentClarity(0.0f),
    currentMix(0.5f),
    currentQuality(1.0f)
{
}

VoiceEditorEffect::~VoiceEditorEffect() {
    shutdown();
}

bool VoiceEditorEffect::initialize() {
    initializeBuffers();
    return true;
}

void VoiceEditorEffect::shutdown() {
    pitchBuffer.clear();
    formantBuffer.clear();
    genderBuffer.clear();
    breathinessBuffer.clear();
    clarityBuffer.clear();
    mixBuffer.clear();
    qualityBuffer.clear();
}

std::vector<PluginParameter> VoiceEditorEffect::getParameters() const {
    return {
        {"pitch", 0.0f, -12.0f, 12.0f},
        {"formant", 0.0f, -12.0f, 12.0f},
        {"gender", 0.0f, -1.0f, 1.0f},
        {"breathiness", 0.0f, 0.0f, 1.0f},
        {"clarity", 0.0f, 0.0f, 1.0f},
        {"mix", 0.5f, 0.0f, 1.0f},
        {"quality", 1.0f, 0.0f, 1.0f}
    };
}

void VoiceEditorEffect::setParameter(const std::string& name, float value) {
    if (name == "pitch") pitch = value;
    else if (name == "formant") formant = value;
    else if (name == "gender") gender = value;
    else if (name == "breathiness") breathiness = value;
    else if (name == "clarity") clarity = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
}

float VoiceEditorEffect::getParameter(const std::string& name) const {
    if (name == "pitch") return pitch;
    else if (name == "formant") return formant;
    else if (name == "gender") return gender;
    else if (name == "breathiness") return breathiness;
    else if (name == "clarity") return clarity;
    else if (name == "mix") return mix;
    else if (name == "quality") return quality;
    return 0.0f;
}

void VoiceEditorEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "pitch") automatedPitch = automated;
    else if (name == "formant") automatedFormant = automated;
    else if (name == "gender") automatedGender = automated;
    else if (name == "breathiness") automatedBreathiness = automated;
    else if (name == "clarity") automatedClarity = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool VoiceEditorEffect::isParameterAutomated(const std::string& name) const {
    if (name == "pitch") return automatedPitch;
    else if (name == "formant") return automatedFormant;
    else if (name == "gender") return automatedGender;
    else if (name == "breathiness") return automatedBreathiness;
    else if (name == "clarity") return automatedClarity;
    else if (name == "mix") return automatedMix;
    else if (name == "quality") return automatedQuality;
    return false;
}

void VoiceEditorEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateParameters();
    processPitch(buffer, framesPerBuffer);
    processFormant(buffer, framesPerBuffer);
    processGender(buffer, framesPerBuffer);
    processBreathiness(buffer, framesPerBuffer);
    processClarity(buffer, framesPerBuffer);
    processMix(buffer, framesPerBuffer);
    processQuality(buffer, framesPerBuffer);
}

void VoiceEditorEffect::loadPreset(const std::string& presetName) {
    // TODO: Implement preset loading
}

void VoiceEditorEffect::savePreset(const std::string& presetName) {
    // TODO: Implement preset saving
}

std::vector<std::string> VoiceEditorEffect::getAvailablePresets() const {
    return {}; // TODO: Implement preset list
}

void VoiceEditorEffect::updateParameters() {
    currentPitch = pitch;
    currentFormant = formant;
    currentGender = gender;
    currentBreathiness = breathiness;
    currentClarity = clarity;
    currentMix = mix;
    currentQuality = quality;
}

void VoiceEditorEffect::initializeBuffers() {
    const unsigned long bufferSize = 4096;
    pitchBuffer.resize(bufferSize, 0.0f);
    formantBuffer.resize(bufferSize, 0.0f);
    genderBuffer.resize(bufferSize, 0.0f);
    breathinessBuffer.resize(bufferSize, 0.0f);
    clarityBuffer.resize(bufferSize, 0.0f);
    mixBuffer.resize(bufferSize, 0.0f);
    qualityBuffer.resize(bufferSize, 0.0f);
}

void VoiceEditorEffect::processPitch(float* buffer, unsigned long framesPerBuffer) {
    if (currentPitch == 0.0f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = calculatePitchShift(buffer[i], currentPitch);
    }
}

void VoiceEditorEffect::processFormant(float* buffer, unsigned long framesPerBuffer) {
    if (currentFormant == 0.0f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = calculateFormantShift(buffer[i], currentFormant);
    }
}

void VoiceEditorEffect::processGender(float* buffer, unsigned long framesPerBuffer) {
    if (currentGender == 0.0f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = calculateGenderShift(buffer[i], currentGender);
    }
}

void VoiceEditorEffect::processBreathiness(float* buffer, unsigned long framesPerBuffer) {
    if (currentBreathiness <= 0.0f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = calculateBreathiness(buffer[i], currentBreathiness);
    }
}

void VoiceEditorEffect::processClarity(float* buffer, unsigned long framesPerBuffer) {
    if (currentClarity <= 0.0f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = calculateClarity(buffer[i], currentClarity);
    }
}

void VoiceEditorEffect::processMix(float* buffer, unsigned long framesPerBuffer) {
    if (currentMix == 0.5f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] *= currentMix;
    }
}

void VoiceEditorEffect::processQuality(float* buffer, unsigned long framesPerBuffer) {
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

float VoiceEditorEffect::calculatePitchShift(float sample, float shift) {
    // Simple pitch shifting using linear interpolation
    float shiftFactor = std::pow(2.0f, shift / 12.0f);
    return sample * shiftFactor;
}

float VoiceEditorEffect::calculateFormantShift(float sample, float shift) {
    // Simple formant shifting using frequency scaling
    float shiftFactor = std::pow(2.0f, shift / 12.0f);
    return sample * shiftFactor;
}

float VoiceEditorEffect::calculateGenderShift(float sample, float shift) {
    // Gender shifting by combining pitch and formant shifts
    float pitchShift = shift * 12.0f;
    float formantShift = -shift * 12.0f;
    
    float pitchShifted = calculatePitchShift(sample, pitchShift);
    float formantShifted = calculateFormantShift(sample, formantShift);
    
    return pitchShifted * 0.5f + formantShifted * 0.5f;
}

float VoiceEditorEffect::calculateBreathiness(float sample, float amount) {
    // Add noise based on signal amplitude
    float noise = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
    return sample * (1.0f - amount) + noise * amount * std::abs(sample);
}

float VoiceEditorEffect::calculateClarity(float sample, float amount) {
    // Enhance high frequencies for clarity
    float enhanced = sample * (1.0f + amount);
    return std::tanh(enhanced); // Soft clipping to prevent distortion
}

} // namespace VRMusicStudio 