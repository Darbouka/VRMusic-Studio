#include "SaturatorEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

SaturatorEffect::SaturatorEffect()
    : drive(2.0f)
    , mix(0.5f)
    , bias(0.0f)
    , symmetry(0.5f)
    , dcOffset(0.0f)
    , oversample(2.0f)
    , quality(0.5f)
    , automatedDrive(false)
    , automatedMix(false)
    , automatedBias(false)
    , automatedSymmetry(false)
    , automatedDcOffset(false)
    , automatedOversample(false)
    , automatedQuality(false)
    , x1(0.0f)
    , x2(0.0f)
    , y1(0.0f)
    , y2(0.0f)
    , a0(0.0f)
    , a1(0.0f)
    , a2(0.0f)
    , b1(0.0f)
    , b2(0.0f)
{
    updateFilterCoefficients();
}

SaturatorEffect::~SaturatorEffect() {
}

bool SaturatorEffect::initialize() {
    x1 = x2 = y1 = y2 = 0.0f;
    updateFilterCoefficients();
    return true;
}

void SaturatorEffect::shutdown() {
}

std::vector<PluginParameter> SaturatorEffect::getParameters() const {
    return {
        {"drive", "Drive", PluginParameter::Type::Float, 1.0f, 10.0f, drive},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"bias", "Bias", PluginParameter::Type::Float, -1.0f, 1.0f, bias},
        {"symmetry", "Symmetry", PluginParameter::Type::Float, 0.0f, 1.0f, symmetry},
        {"dcOffset", "DC Offset", PluginParameter::Type::Float, -1.0f, 1.0f, dcOffset},
        {"oversample", "Oversample", PluginParameter::Type::Float, 1.0f, 4.0f, oversample},
        {"quality", "Quality", PluginParameter::Type::Float, 0.0f, 1.0f, quality}
    };
}

void SaturatorEffect::setParameter(const std::string& name, float value) {
    if (name == "drive") drive = value;
    else if (name == "mix") mix = value;
    else if (name == "bias") bias = value;
    else if (name == "symmetry") symmetry = value;
    else if (name == "dcOffset") dcOffset = value;
    else if (name == "oversample") oversample = value;
    else if (name == "quality") quality = value;
}

float SaturatorEffect::getParameter(const std::string& name) const {
    if (name == "drive") return drive;
    if (name == "mix") return mix;
    if (name == "bias") return bias;
    if (name == "symmetry") return symmetry;
    if (name == "dcOffset") return dcOffset;
    if (name == "oversample") return oversample;
    if (name == "quality") return quality;
    return 0.0f;
}

void SaturatorEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "drive") automatedDrive = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "bias") automatedBias = automated;
    else if (name == "symmetry") automatedSymmetry = automated;
    else if (name == "dcOffset") automatedDcOffset = automated;
    else if (name == "oversample") automatedOversample = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool SaturatorEffect::isParameterAutomated(const std::string& name) const {
    if (name == "drive") return automatedDrive;
    if (name == "mix") return automatedMix;
    if (name == "bias") return automatedBias;
    if (name == "symmetry") return automatedSymmetry;
    if (name == "dcOffset") return automatedDcOffset;
    if (name == "oversample") return automatedOversample;
    if (name == "quality") return automatedQuality;
    return false;
}

void SaturatorEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // DC-Offset entfernen
        left -= dcOffset;
        right -= dcOffset;
        
        // Oversampling
        int oversampleFactor = static_cast<int>(oversample);
        float leftOversampled = 0.0f;
        float rightOversampled = 0.0f;
        
        for (int j = 0; j < oversampleFactor; ++j) {
            float t = static_cast<float>(j) / oversampleFactor;
            float leftInterp = left + t * (left - x1);
            float rightInterp = right + t * (right - x2);
            
            leftOversampled += processSaturation(leftInterp);
            rightOversampled += processSaturation(rightInterp);
        }
        
        leftOversampled /= oversampleFactor;
        rightOversampled /= oversampleFactor;
        
        // Zustand aktualisieren
        x2 = x1;
        x1 = left;
        
        // Mix
        buffer[i] = left * (1.0f - mix) + leftOversampled * mix;
        buffer[i + 1] = right * (1.0f - mix) + rightOversampled * mix;
    }
}

float SaturatorEffect::processSaturation(float input) {
    // Bias anwenden
    input += bias;
    
    // Drive anwenden
    input *= drive;
    
    // Symmetrische Sättigung
    float output;
    if (input > 0.0f) {
        output = std::tanh(input * (1.0f + symmetry));
    } else {
        output = -std::tanh(-input * (1.0f + (1.0f - symmetry)));
    }
    
    // Normalisierung
    return output / drive;
}

void SaturatorEffect::updateFilterCoefficients() {
    // Anti-Aliasing-Filter-Koeffizienten
    float cutoff = 20000.0f / 44100.0f;
    float q = 0.707f;
    
    float w0 = 2.0f * M_PI * cutoff;
    float alpha = std::sin(w0) / (2.0f * q);
    
    float cosw0 = std::cos(w0);
    float a = std::sqrt(2.0f);
    
    a0 = alpha;
    a1 = 0.0f;
    a2 = -alpha;
    b1 = -2.0f * cosw0;
    b2 = 1.0f - alpha;
    
    // Normalisierung
    float scale = 1.0f / (1.0f + alpha);
    a0 *= scale;
    a1 *= scale;
    a2 *= scale;
    b1 *= scale;
    b2 *= scale;
}

void SaturatorEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        drive = 2.0f;
        mix = 0.5f;
        bias = 0.0f;
        symmetry = 0.5f;
        dcOffset = 0.0f;
        oversample = 2.0f;
        quality = 0.5f;
    }
    else if (presetName == "Heavy") {
        drive = 5.0f;
        mix = 0.7f;
        bias = 0.2f;
        symmetry = 0.7f;
        dcOffset = 0.1f;
        oversample = 4.0f;
        quality = 0.7f;
    }
    else if (presetName == "Subtle") {
        drive = 1.5f;
        mix = 0.3f;
        bias = 0.0f;
        symmetry = 0.3f;
        dcOffset = 0.0f;
        oversample = 2.0f;
        quality = 0.3f;
    }
    
    updateFilterCoefficients();
}

void SaturatorEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> SaturatorEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 