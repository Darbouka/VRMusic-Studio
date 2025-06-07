#include "WaveShaperEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

WaveShaperEffect::WaveShaperEffect()
    : drive(0.5f)
    , mix(0.5f)
    , stereoWidth(0.5f)
    , symmetry(0.5f)
    , bias(0.0f)
    , oversample(1.0f)
    , dcOffset(0.0f)
    , automatedDrive(false)
    , automatedMix(false)
    , automatedStereoWidth(false)
    , automatedSymmetry(false)
    , automatedBias(false)
    , automatedOversample(false)
    , automatedDcOffset(false)
{
}

WaveShaperEffect::~WaveShaperEffect() {}

bool WaveShaperEffect::initialize() {
    return true;
}

void WaveShaperEffect::shutdown() {}

std::vector<PluginParameter> WaveShaperEffect::getParameters() const {
    return {
        {"drive", "Drive", PluginParameter::Type::Float, 0.0f, 1.0f, drive},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"stereoWidth", "Stereo Width", PluginParameter::Type::Float, 0.0f, 1.0f, stereoWidth},
        {"symmetry", "Symmetry", PluginParameter::Type::Float, 0.0f, 1.0f, symmetry},
        {"bias", "Bias", PluginParameter::Type::Float, -1.0f, 1.0f, bias},
        {"oversample", "Oversample", PluginParameter::Type::Float, 1.0f, 4.0f, oversample},
        {"dcOffset", "DC Offset", PluginParameter::Type::Float, -1.0f, 1.0f, dcOffset}
    };
}

void WaveShaperEffect::setParameter(const std::string& name, float value) {
    if (name == "drive") drive = value;
    else if (name == "mix") mix = value;
    else if (name == "stereoWidth") stereoWidth = value;
    else if (name == "symmetry") symmetry = value;
    else if (name == "bias") bias = value;
    else if (name == "oversample") oversample = value;
    else if (name == "dcOffset") dcOffset = value;
}

float WaveShaperEffect::getParameter(const std::string& name) const {
    if (name == "drive") return drive;
    if (name == "mix") return mix;
    if (name == "stereoWidth") return stereoWidth;
    if (name == "symmetry") return symmetry;
    if (name == "bias") return bias;
    if (name == "oversample") return oversample;
    if (name == "dcOffset") return dcOffset;
    return 0.0f;
}

void WaveShaperEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "drive") automatedDrive = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "stereoWidth") automatedStereoWidth = automated;
    else if (name == "symmetry") automatedSymmetry = automated;
    else if (name == "bias") automatedBias = automated;
    else if (name == "oversample") automatedOversample = automated;
    else if (name == "dcOffset") automatedDcOffset = automated;
}

bool WaveShaperEffect::isParameterAutomated(const std::string& name) const {
    if (name == "drive") return automatedDrive;
    if (name == "mix") return automatedMix;
    if (name == "stereoWidth") return automatedStereoWidth;
    if (name == "symmetry") return automatedSymmetry;
    if (name == "bias") return automatedBias;
    if (name == "oversample") return automatedOversample;
    if (name == "dcOffset") return automatedDcOffset;
    return false;
}

void WaveShaperEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // Wave-Shaping anwenden
        float shapedLeft = processSample(left);
        float shapedRight = processSample(right);
        
        // Stereo-Width
        float mid = (shapedLeft + shapedRight) * 0.5f;
        float side = (shapedLeft - shapedRight) * 0.5f;
        shapedLeft = mid + side * stereoWidth;
        shapedRight = mid - side * stereoWidth;
        
        // Mix
        buffer[i] = left * (1.0f - mix) + shapedLeft * mix;
        buffer[i + 1] = right * (1.0f - mix) + shapedRight * mix;
    }
}

float WaveShaperEffect::processSample(float input) {
    // Drive anwenden
    float processed = applyDrive(input);
    
    // Symmetrie anwenden
    processed = applySymmetry(processed);
    
    // Bias anwenden
    processed = applyBias(processed);
    
    // DC-Offset entfernen
    processed = removeDcOffset(processed);
    
    return processed;
}

float WaveShaperEffect::applyDrive(float input) {
    // Soft-Clipping mit Drive
    float driveGain = 1.0f + drive * 10.0f;
    float processed = input * driveGain;
    
    // Soft-Clipping-Funktion
    return (2.0f / M_PI) * std::atan(processed);
}

float WaveShaperEffect::applySymmetry(float input) {
    // Symmetrie-Effekt
    if (input >= 0.0f) {
        return input * (1.0f + symmetry);
    } else {
        return input * (1.0f - symmetry);
    }
}

float WaveShaperEffect::applyBias(float input) {
    // Bias-Effekt
    return input + bias;
}

float WaveShaperEffect::removeDcOffset(float input) {
    // DC-Offset entfernen
    return input - dcOffset;
}

void WaveShaperEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        drive = 0.5f;
        mix = 0.5f;
        stereoWidth = 0.5f;
        symmetry = 0.5f;
        bias = 0.0f;
        oversample = 1.0f;
        dcOffset = 0.0f;
    }
    else if (presetName == "Heavy") {
        drive = 0.8f;
        mix = 0.7f;
        stereoWidth = 0.7f;
        symmetry = 0.7f;
        bias = 0.2f;
        oversample = 2.0f;
        dcOffset = 0.1f;
    }
    else if (presetName == "Subtle") {
        drive = 0.3f;
        mix = 0.3f;
        stereoWidth = 0.3f;
        symmetry = 0.3f;
        bias = 0.0f;
        oversample = 1.0f;
        dcOffset = 0.0f;
    }
}

void WaveShaperEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> WaveShaperEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 