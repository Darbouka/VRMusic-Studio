#include "DistortionEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

DistortionEffect::DistortionEffect()
    : drive(0.5f)
    , mix(0.5f)
    , stereoWidth(0.5f)
    , tone(0.5f)
    , bias(0.0f)
    , symmetry(0.5f)
    , noise(0.0f)
    , automatedDrive(false)
    , automatedMix(false)
    , automatedStereoWidth(false)
    , automatedTone(false)
    , automatedBias(false)
    , automatedSymmetry(false)
    , automatedNoise(false)
{
    x1[0] = x1[1] = 0.0f;
    y1[0] = y1[1] = 0.0f;
    
    updateFilterCoefficients();
}

DistortionEffect::~DistortionEffect() {
}

bool DistortionEffect::initialize() {
    return true;
}

void DistortionEffect::shutdown() {
}

void DistortionEffect::updateFilterCoefficients() {
    // Hier würden die Filter-Koeffizienten basierend auf dem Tone-Parameter aktualisiert
}

std::vector<PluginParameter> DistortionEffect::getParameters() const {
    return {
        {"drive", "Drive", PluginParameter::Type::Float, 0.0f, 1.0f, drive},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"stereoWidth", "Stereo Width", PluginParameter::Type::Float, 0.0f, 1.0f, stereoWidth},
        {"tone", "Tone", PluginParameter::Type::Float, 0.0f, 1.0f, tone},
        {"bias", "Bias", PluginParameter::Type::Float, -1.0f, 1.0f, bias},
        {"symmetry", "Symmetry", PluginParameter::Type::Float, 0.0f, 1.0f, symmetry},
        {"noise", "Noise", PluginParameter::Type::Float, 0.0f, 1.0f, noise}
    };
}

void DistortionEffect::setParameter(const std::string& name, float value) {
    if (name == "drive") drive = value;
    else if (name == "mix") mix = value;
    else if (name == "stereoWidth") stereoWidth = value;
    else if (name == "tone") {
        tone = value;
        updateFilterCoefficients();
    }
    else if (name == "bias") bias = value;
    else if (name == "symmetry") symmetry = value;
    else if (name == "noise") noise = value;
}

float DistortionEffect::getParameter(const std::string& name) const {
    if (name == "drive") return drive;
    if (name == "mix") return mix;
    if (name == "stereoWidth") return stereoWidth;
    if (name == "tone") return tone;
    if (name == "bias") return bias;
    if (name == "symmetry") return symmetry;
    if (name == "noise") return noise;
    return 0.0f;
}

void DistortionEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "drive") automatedDrive = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "stereoWidth") automatedStereoWidth = automated;
    else if (name == "tone") automatedTone = automated;
    else if (name == "bias") automatedBias = automated;
    else if (name == "symmetry") automatedSymmetry = automated;
    else if (name == "noise") automatedNoise = automated;
}

bool DistortionEffect::isParameterAutomated(const std::string& name) const {
    if (name == "drive") return automatedDrive;
    if (name == "mix") return automatedMix;
    if (name == "stereoWidth") return automatedStereoWidth;
    if (name == "tone") return automatedTone;
    if (name == "bias") return automatedBias;
    if (name == "symmetry") return automatedSymmetry;
    if (name == "noise") return automatedNoise;
    return false;
}

void DistortionEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // Filter anwenden
        left = processFilter(left, 0);
        right = processFilter(right, 1);
        
        // Distortion anwenden
        left = applyDistortion(left);
        right = applyDistortion(right);
        
        // Stereo-Width
        float mid = (left + right) * 0.5f;
        float side = (left - right) * 0.5f;
        left = mid + side * stereoWidth;
        right = mid - side * stereoWidth;
        
        // Mix
        buffer[i] = buffer[i] * (1.0f - mix) + left * mix;
        buffer[i + 1] = buffer[i + 1] * (1.0f - mix) + right * mix;
    }
}

float DistortionEffect::processFilter(float input, int channel) {
    // Einfacher Tiefpass-Filter
    float output = input * (1.0f - tone) + y1[channel] * tone;
    y1[channel] = output;
    return output;
}

float DistortionEffect::applyDistortion(float input) {
    // Bias hinzufügen
    input += bias;
    
    // Drive anwenden
    input *= (1.0f + drive * 10.0f);
    
    // Symmetrische Verzerrung
    float sign = input >= 0.0f ? 1.0f : -1.0f;
    float absInput = std::abs(input);
    float threshold = 0.5f;
    
    if (absInput > threshold) {
        float overshoot = absInput - threshold;
        float distortion = overshoot * (1.0f - symmetry);
        input = sign * (threshold + distortion);
    }
    
    // Rauschen hinzufügen
    if (noise > 0.0f) {
        float noiseSample = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
        input += noiseSample * noise * 0.1f;
    }
    
    // Begrenzen
    return std::max(-1.0f, std::min(1.0f, input));
}

void DistortionEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        drive = 0.5f;
        mix = 0.5f;
        stereoWidth = 0.5f;
        tone = 0.5f;
        bias = 0.0f;
        symmetry = 0.5f;
        noise = 0.0f;
    }
    else if (presetName == "Heavy") {
        drive = 0.8f;
        mix = 0.7f;
        stereoWidth = 0.7f;
        tone = 0.3f;
        bias = 0.2f;
        symmetry = 0.7f;
        noise = 0.1f;
    }
    else if (presetName == "Subtle") {
        drive = 0.3f;
        mix = 0.3f;
        stereoWidth = 0.3f;
        tone = 0.7f;
        bias = -0.1f;
        symmetry = 0.3f;
        noise = 0.0f;
    }
    
    updateFilterCoefficients();
}

void DistortionEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> DistortionEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 
} // namespace VR_DAW 