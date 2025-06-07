#include "AutoWahEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

AutoWahEffect::AutoWahEffect()
    : sensitivity(0.5f)
    , attack(10.0f)
    , release(100.0f)
    , minFreq(500.0f)
    , maxFreq(3000.0f)
    , resonance(2.0f)
    , mix(0.5f)
    , quality(1.0f)
    , automatedSensitivity(false)
    , automatedAttack(false)
    , automatedRelease(false)
    , automatedMinFreq(false)
    , automatedMaxFreq(false)
    , automatedResonance(false)
    , automatedMix(false)
    , automatedQuality(false)
{
    initializeFilters();
}

AutoWahEffect::~AutoWahEffect() {
    shutdown();
}

bool AutoWahEffect::initialize() {
    initializeFilters();
    return true;
}

void AutoWahEffect::shutdown() {
    // Keine spezifische Aufräumarbeit nötig
}

void AutoWahEffect::initializeFilters() {
    // Initialisiere Filterzustände
    leftFilter = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    rightFilter = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    
    // Berechne Hüllkurven-Koeffizienten
    float attackTime = attack / 1000.0f; // ms zu s
    float releaseTime = release / 1000.0f; // ms zu s
    
    leftFilter.attackCoeff = std::exp(-1.0f / (attackTime * 44100.0f));
    rightFilter.attackCoeff = leftFilter.attackCoeff;
    leftFilter.releaseCoeff = std::exp(-1.0f / (releaseTime * 44100.0f));
    rightFilter.releaseCoeff = leftFilter.releaseCoeff;
    
    // Initialisiere Filter mit mittlerer Frequenz
    float midFreq = (minFreq + maxFreq) * 0.5f;
    calculateFilterCoefficients(leftFilter, midFreq);
    calculateFilterCoefficients(rightFilter, midFreq);
}

void AutoWahEffect::updateFilters() {
    // Aktualisiere Hüllkurven-Koeffizienten
    float attackTime = attack / 1000.0f;
    float releaseTime = release / 1000.0f;
    
    leftFilter.attackCoeff = std::exp(-1.0f / (attackTime * 44100.0f));
    rightFilter.attackCoeff = leftFilter.attackCoeff;
    leftFilter.releaseCoeff = std::exp(-1.0f / (releaseTime * 44100.0f));
    rightFilter.releaseCoeff = leftFilter.releaseCoeff;
}

void AutoWahEffect::updateEnvelope(FilterState& filter, float input) {
    // Berechne Eingangspegel
    float inputLevel = std::abs(input);
    
    // Update Hüllkurve
    if (inputLevel > filter.envelope) {
        filter.envelope = inputLevel + (filter.envelope - inputLevel) * filter.attackCoeff;
    } else {
        filter.envelope = inputLevel + (filter.envelope - inputLevel) * filter.releaseCoeff;
    }
}

void AutoWahEffect::calculateFilterCoefficients(FilterState& filter, float frequency) {
    // Berechne Filterkoeffizienten für einen Bandpass-Filter
    float w0 = 2.0f * M_PI * frequency / 44100.0f;
    float alpha = std::sin(w0) / (2.0f * resonance);
    
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * std::cos(w0);
    float a2 = 1.0f - alpha;
    float b0 = alpha;
    float b1 = 0.0f;
    float b2 = -alpha;
    
    // Normalisiere Koeffizienten
    filter.b0 = b0 / a0;
    filter.b1 = b1 / a0;
    filter.b2 = b2 / a0;
    filter.a1 = a1 / a0;
    filter.a2 = a2 / a0;
}

float AutoWahEffect::processFilter(FilterState& filter, float input) {
    // Update Hüllkurve
    updateEnvelope(filter, input);
    
    // Berechne aktuelle Filterfrequenz basierend auf Hüllkurve
    float freq = minFreq + (maxFreq - minFreq) * filter.envelope * sensitivity;
    calculateFilterCoefficients(filter, freq);
    
    // Verarbeite Filter
    float output = filter.b0 * input + filter.b1 * filter.x1 + filter.b2 * filter.x2
                  - filter.a1 * filter.y1 - filter.a2 * filter.y2;
    
    // Update Filterzustand
    filter.x2 = filter.x1;
    filter.x1 = input;
    filter.y2 = filter.y1;
    filter.y1 = output;
    
    return output;
}

void AutoWahEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer * 2; i += 2) {
        float leftInput = buffer[i];
        float rightInput = buffer[i + 1];
        
        // Verarbeite Filter
        float leftOutput = processFilter(leftFilter, leftInput);
        float rightOutput = processFilter(rightFilter, rightInput);
        
        // Mix
        buffer[i] = leftInput + (leftOutput - leftInput) * mix;
        buffer[i + 1] = rightInput + (rightOutput - rightInput) * mix;
    }
}

std::vector<PluginParameter> AutoWahEffect::getParameters() const {
    return {
        {"sensitivity", sensitivity, 0.0f, 1.0f, ""},
        {"attack", attack, 0.1f, 100.0f, "ms"},
        {"release", release, 10.0f, 1000.0f, "ms"},
        {"minFreq", minFreq, 100.0f, 1000.0f, "Hz"},
        {"maxFreq", maxFreq, 1000.0f, 10000.0f, "Hz"},
        {"resonance", resonance, 0.1f, 10.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void AutoWahEffect::setParameter(const std::string& name, float value) {
    if (name == "sensitivity") sensitivity = value;
    else if (name == "attack") {
        attack = value;
        updateFilters();
    }
    else if (name == "release") {
        release = value;
        updateFilters();
    }
    else if (name == "minFreq") minFreq = value;
    else if (name == "maxFreq") maxFreq = value;
    else if (name == "resonance") resonance = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
}

float AutoWahEffect::getParameter(const std::string& name) const {
    if (name == "sensitivity") return sensitivity;
    if (name == "attack") return attack;
    if (name == "release") return release;
    if (name == "minFreq") return minFreq;
    if (name == "maxFreq") return maxFreq;
    if (name == "resonance") return resonance;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
    return 0.0f;
}

void AutoWahEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "sensitivity") automatedSensitivity = automated;
    else if (name == "attack") automatedAttack = automated;
    else if (name == "release") automatedRelease = automated;
    else if (name == "minFreq") automatedMinFreq = automated;
    else if (name == "maxFreq") automatedMaxFreq = automated;
    else if (name == "resonance") automatedResonance = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool AutoWahEffect::isParameterAutomated(const std::string& name) const {
    if (name == "sensitivity") return automatedSensitivity;
    if (name == "attack") return automatedAttack;
    if (name == "release") return automatedRelease;
    if (name == "minFreq") return automatedMinFreq;
    if (name == "maxFreq") return automatedMaxFreq;
    if (name == "resonance") return automatedResonance;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    return false;
}

void AutoWahEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        sensitivity = 0.5f;
        attack = 10.0f;
        release = 100.0f;
        minFreq = 500.0f;
        maxFreq = 3000.0f;
        resonance = 2.0f;
        mix = 0.5f;
        quality = 1.0f;
    }
    else if (presetName == "Heavy") {
        sensitivity = 0.8f;
        attack = 5.0f;
        release = 200.0f;
        minFreq = 300.0f;
        maxFreq = 5000.0f;
        resonance = 4.0f;
        mix = 0.7f;
        quality = 1.0f;
    }
    else if (presetName == "Subtle") {
        sensitivity = 0.3f;
        attack = 20.0f;
        release = 50.0f;
        minFreq = 800.0f;
        maxFreq = 2000.0f;
        resonance = 1.5f;
        mix = 0.3f;
        quality = 1.0f;
    }
    
    updateFilters();
}

void AutoWahEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> AutoWahEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 