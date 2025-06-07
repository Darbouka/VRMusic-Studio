#include "GateEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

GateEffect::GateEffect()
    : threshold(-40.0f)
    , attack(10.0f)
    , release(100.0f)
    , ratio(10.0f)
    , range(60.0f)
    , knee(10.0f)
    , mix(1.0f)
    , automatedThreshold(false)
    , automatedAttack(false)
    , automatedRelease(false)
    , automatedRatio(false)
    , automatedRange(false)
    , automatedKnee(false)
    , automatedMix(false)
    , envelope(0.0f)
    , gain(1.0f)
{
}

GateEffect::~GateEffect() {
}

bool GateEffect::initialize() {
    return true;
}

void GateEffect::shutdown() {
}

std::vector<PluginParameter> GateEffect::getParameters() const {
    return {
        {"threshold", "Threshold", PluginParameter::Type::Float, -60.0f, 0.0f, threshold},
        {"attack", "Attack", PluginParameter::Type::Float, 0.1f, 100.0f, attack},
        {"release", "Release", PluginParameter::Type::Float, 1.0f, 1000.0f, release},
        {"ratio", "Ratio", PluginParameter::Type::Float, 1.0f, 100.0f, ratio},
        {"range", "Range", PluginParameter::Type::Float, 0.0f, 100.0f, range},
        {"knee", "Knee", PluginParameter::Type::Float, 0.0f, 40.0f, knee},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix}
    };
}

void GateEffect::setParameter(const std::string& name, float value) {
    if (name == "threshold") threshold = value;
    else if (name == "attack") attack = value;
    else if (name == "release") release = value;
    else if (name == "ratio") ratio = value;
    else if (name == "range") range = value;
    else if (name == "knee") knee = value;
    else if (name == "mix") mix = value;
}

float GateEffect::getParameter(const std::string& name) const {
    if (name == "threshold") return threshold;
    if (name == "attack") return attack;
    if (name == "release") return release;
    if (name == "ratio") return ratio;
    if (name == "range") return range;
    if (name == "knee") return knee;
    if (name == "mix") return mix;
    return 0.0f;
}

void GateEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "threshold") automatedThreshold = automated;
    else if (name == "attack") automatedAttack = automated;
    else if (name == "release") automatedRelease = automated;
    else if (name == "ratio") automatedRatio = automated;
    else if (name == "range") automatedRange = automated;
    else if (name == "knee") automatedKnee = automated;
    else if (name == "mix") automatedMix = automated;
}

bool GateEffect::isParameterAutomated(const std::string& name) const {
    if (name == "threshold") return automatedThreshold;
    if (name == "attack") return automatedAttack;
    if (name == "release") return automatedRelease;
    if (name == "ratio") return automatedRatio;
    if (name == "range") return automatedRange;
    if (name == "knee") return automatedKnee;
    if (name == "mix") return automatedMix;
    return false;
}

void GateEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // Eingangspegel berechnen
        float inputLevel = std::max(std::abs(left), std::abs(right));
        float inputDb = linearToDb(inputLevel);
        
        // Hüllkurve berechnen
        envelope = calculateEnvelope(inputDb);
        
        // Verstärkung berechnen
        gain = calculateGain(envelope);
        
        // Verstärkung anwenden
        left *= gain;
        right *= gain;
        
        // Mix
        buffer[i] = buffer[i] * (1.0f - mix) + left * mix;
        buffer[i + 1] = buffer[i + 1] * (1.0f - mix) + right * mix;
    }
}

float GateEffect::calculateGain(float inputDb) {
    float gainDb = 0.0f;
    
    // Knee-Bereich
    float kneeStart = threshold - knee * 0.5f;
    float kneeEnd = threshold + knee * 0.5f;
    
    if (inputDb <= kneeStart) {
        // Unterhalb des Knee-Bereichs
        gainDb = -range;  // Maximale Dämpfung
    }
    else if (inputDb >= kneeEnd) {
        // Oberhalb des Knee-Bereichs
        gainDb = 0.0f;  // Keine Dämpfung
    }
    else {
        // Im Knee-Bereich
        float kneeFactor = (inputDb - kneeStart) / knee;
        gainDb = -range * (1.0f - kneeFactor);
    }
    
    return dbToLinear(gainDb);
}

float GateEffect::calculateEnvelope(float inputDb) {
    float attackCoeff = std::exp(-1.0f / (attack * 0.001f * 44100.0f));
    float releaseCoeff = std::exp(-1.0f / (release * 0.001f * 44100.0f));
    
    if (inputDb > envelope) {
        // Attack
        envelope = inputDb + (envelope - inputDb) * attackCoeff;
    }
    else {
        // Release
        envelope = inputDb + (envelope - inputDb) * releaseCoeff;
    }
    
    return envelope;
}

float GateEffect::dbToLinear(float db) {
    return std::pow(10.0f, db * 0.05f);
}

float GateEffect::linearToDb(float linear) {
    return 20.0f * std::log10(std::max(linear, 1e-6f));
}

void GateEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        threshold = -40.0f;
        attack = 10.0f;
        release = 100.0f;
        ratio = 10.0f;
        range = 60.0f;
        knee = 10.0f;
        mix = 1.0f;
    }
    else if (presetName == "Aggressive") {
        threshold = -30.0f;
        attack = 5.0f;
        release = 50.0f;
        ratio = 20.0f;
        range = 80.0f;
        knee = 5.0f;
        mix = 1.0f;
    }
    else if (presetName == "Subtle") {
        threshold = -50.0f;
        attack = 20.0f;
        release = 200.0f;
        ratio = 5.0f;
        range = 40.0f;
        knee = 20.0f;
        mix = 0.5f;
    }
}

void GateEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> GateEffect::getAvailablePresets() const {
    return {"Standard", "Aggressive", "Subtle"};
}

} // namespace VR_DAW 