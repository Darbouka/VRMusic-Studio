#include "ExciterEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

ExciterEffect::ExciterEffect()
    : amount(0.5f)
    , mix(0.5f)
    , frequency(2000.0f)
    , q(2.0f)
    , drive(2.0f)
    , symmetry(0.5f)
    , bias(0.0f)
    , automatedAmount(false)
    , automatedMix(false)
    , automatedFrequency(false)
    , automatedQ(false)
    , automatedDrive(false)
    , automatedSymmetry(false)
    , automatedBias(false)
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

ExciterEffect::~ExciterEffect() {
}

bool ExciterEffect::initialize() {
    x1 = x2 = y1 = y2 = 0.0f;
    updateFilterCoefficients();
    return true;
}

void ExciterEffect::shutdown() {
}

std::vector<PluginParameter> ExciterEffect::getParameters() const {
    return {
        {"amount", "Amount", PluginParameter::Type::Float, 0.0f, 1.0f, amount},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"frequency", "Frequency", PluginParameter::Type::Float, 1000.0f, 8000.0f, frequency},
        {"q", "Q", PluginParameter::Type::Float, 0.1f, 10.0f, q},
        {"drive", "Drive", PluginParameter::Type::Float, 1.0f, 10.0f, drive},
        {"symmetry", "Symmetry", PluginParameter::Type::Float, 0.0f, 1.0f, symmetry},
        {"bias", "Bias", PluginParameter::Type::Float, -1.0f, 1.0f, bias}
    };
}

void ExciterEffect::setParameter(const std::string& name, float value) {
    if (name == "amount") amount = value;
    else if (name == "mix") mix = value;
    else if (name == "frequency") {
        frequency = value;
        updateFilterCoefficients();
    }
    else if (name == "q") {
        q = value;
        updateFilterCoefficients();
    }
    else if (name == "drive") drive = value;
    else if (name == "symmetry") symmetry = value;
    else if (name == "bias") bias = value;
}

float ExciterEffect::getParameter(const std::string& name) const {
    if (name == "amount") return amount;
    if (name == "mix") return mix;
    if (name == "frequency") return frequency;
    if (name == "q") return q;
    if (name == "drive") return drive;
    if (name == "symmetry") return symmetry;
    if (name == "bias") return bias;
    return 0.0f;
}

void ExciterEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "amount") automatedAmount = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "frequency") automatedFrequency = automated;
    else if (name == "q") automatedQ = automated;
    else if (name == "drive") automatedDrive = automated;
    else if (name == "symmetry") automatedSymmetry = automated;
    else if (name == "bias") automatedBias = automated;
}

bool ExciterEffect::isParameterAutomated(const std::string& name) const {
    if (name == "amount") return automatedAmount;
    if (name == "mix") return automatedMix;
    if (name == "frequency") return automatedFrequency;
    if (name == "q") return automatedQ;
    if (name == "drive") return automatedDrive;
    if (name == "symmetry") return automatedSymmetry;
    if (name == "bias") return automatedBias;
    return false;
}

void ExciterEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // Filter anwenden
        float filteredLeft = a0 * left + a1 * x1 + a2 * x2 - b1 * y1 - b2 * y2;
        float filteredRight = a0 * right + a1 * x1 + a2 * x2 - b1 * y1 - b2 * y2;
        
        // Zustand aktualisieren
        x2 = x1;
        x1 = (left + right) * 0.5f;
        y2 = y1;
        y1 = filteredLeft;
        
        // Harmonische Verzerrung
        float excitedLeft = processHarmonics(filteredLeft);
        float excitedRight = processHarmonics(filteredRight);
        
        // Mix
        buffer[i] = left * (1.0f - mix) + excitedLeft * mix * amount;
        buffer[i + 1] = right * (1.0f - mix) + excitedRight * mix * amount;
    }
}

float ExciterEffect::processHarmonics(float input) {
    // Bias anwenden
    input += bias;
    
    // Drive anwenden
    input *= drive;
    
    // Symmetrische Verzerrung
    float output;
    if (input > 0.0f) {
        output = std::tanh(input * (1.0f + symmetry));
    } else {
        output = -std::tanh(-input * (1.0f + (1.0f - symmetry)));
    }
    
    // Normalisierung
    return output / drive;
}

void ExciterEffect::updateFilterCoefficients() {
    // Biquad-Filter-Koeffizienten berechnen
    float w0 = 2.0f * M_PI * frequency / 44100.0f;
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

void ExciterEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        amount = 0.5f;
        mix = 0.5f;
        frequency = 2000.0f;
        q = 2.0f;
        drive = 2.0f;
        symmetry = 0.5f;
        bias = 0.0f;
    }
    else if (presetName == "Bright") {
        amount = 0.7f;
        mix = 0.7f;
        frequency = 4000.0f;
        q = 3.0f;
        drive = 3.0f;
        symmetry = 0.7f;
        bias = 0.2f;
    }
    else if (presetName == "Warm") {
        amount = 0.7f;
        mix = 0.7f;
        frequency = 1000.0f;
        q = 1.5f;
        drive = 2.0f;
        symmetry = 0.3f;
        bias = -0.2f;
    }
    
    updateFilterCoefficients();
}

void ExciterEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> ExciterEffect::getAvailablePresets() const {
    return {"Standard", "Bright", "Warm"};
}

} // namespace VR_DAW 