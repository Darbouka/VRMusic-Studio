#include "VocoderEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

VocoderEffect::VocoderEffect()
    : bands(16.0f)
    , q(1.0f)
    , attack(10.0f)
    , release(100.0f)
    , mix(0.5f)
    , level(0.5f)
    , quality(0.5f)
    , automatedBands(false)
    , automatedQ(false)
    , automatedAttack(false)
    , automatedRelease(false)
    , automatedMix(false)
    , automatedLevel(false)
    , automatedQuality(false)
{
    initializeBands();
}

VocoderEffect::~VocoderEffect() {
}

bool VocoderEffect::initialize() {
    initializeBands();
    updateBandCoefficients();
    return true;
}

void VocoderEffect::shutdown() {
}

std::vector<PluginParameter> VocoderEffect::getParameters() const {
    return {
        {"bands", "Bands", PluginParameter::Type::Float, 4.0f, 32.0f, bands},
        {"q", "Q", PluginParameter::Type::Float, 0.1f, 10.0f, q},
        {"attack", "Attack", PluginParameter::Type::Float, 0.1f, 100.0f, attack},
        {"release", "Release", PluginParameter::Type::Float, 10.0f, 1000.0f, release},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"level", "Level", PluginParameter::Type::Float, 0.0f, 1.0f, level},
        {"quality", "Quality", PluginParameter::Type::Float, 0.0f, 1.0f, quality}
    };
}

void VocoderEffect::setParameter(const std::string& name, float value) {
    if (name == "bands") {
        bands = value;
        initializeBands();
    }
    else if (name == "q") {
        q = value;
        updateBandCoefficients();
    }
    else if (name == "attack") {
        attack = value;
        updateBandCoefficients();
    }
    else if (name == "release") {
        release = value;
        updateBandCoefficients();
    }
    else if (name == "mix") mix = value;
    else if (name == "level") level = value;
    else if (name == "quality") quality = value;
}

float VocoderEffect::getParameter(const std::string& name) const {
    if (name == "bands") return bands;
    if (name == "q") return q;
    if (name == "attack") return attack;
    if (name == "release") return release;
    if (name == "mix") return mix;
    if (name == "level") return level;
    if (name == "quality") return quality;
    return 0.0f;
}

void VocoderEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "bands") automatedBands = automated;
    else if (name == "q") automatedQ = automated;
    else if (name == "attack") automatedAttack = automated;
    else if (name == "release") automatedRelease = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "level") automatedLevel = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool VocoderEffect::isParameterAutomated(const std::string& name) const {
    if (name == "bands") return automatedBands;
    if (name == "q") return automatedQ;
    if (name == "attack") return automatedAttack;
    if (name == "release") return automatedRelease;
    if (name == "mix") return automatedMix;
    if (name == "level") return automatedLevel;
    if (name == "quality") return automatedQuality;
    return false;
}

void VocoderEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // Carrier-Signal (hier einfach das Eingangssignal)
        float carrier = (left + right) * 0.5f;
        
        // Vocoder-Verarbeitung für jeden Band
        float processedLeft = 0.0f;
        float processedRight = 0.0f;
        
        for (auto& band : bands) {
            float bandOutput = processBand(band, left, carrier);
            processedLeft += bandOutput;
            processedRight += bandOutput;
        }
        
        // Level und Mix anwenden
        processedLeft *= level;
        processedRight *= level;
        
        buffer[i] = left * (1.0f - mix) + processedLeft * mix;
        buffer[i + 1] = right * (1.0f - mix) + processedRight * mix;
    }
}

void VocoderEffect::initializeBands() {
    bands.clear();
    
    // Band-Frequenzen logarithmisch verteilen
    float minFreq = 100.0f;
    float maxFreq = 8000.0f;
    int numBands = static_cast<int>(bands);
    
    for (int i = 0; i < numBands; ++i) {
        float t = static_cast<float>(i) / (numBands - 1);
        float centerFreq = minFreq * std::pow(maxFreq / minFreq, t);
        
        Band band;
        band.centerFreq = centerFreq;
        band.bandwidth = centerFreq / q;
        band.envelope = 0.0f;
        band.carrierEnvelope = 0.0f;
        band.modulatorEnvelope = 0.0f;
        band.filterState.resize(4, 0.0f);
        
        bands.push_back(band);
    }
    
    updateBandCoefficients();
}

void VocoderEffect::updateBandCoefficients() {
    for (auto& band : bands) {
        // Attack- und Release-Koeffizienten berechnen
        band.attackCoeff = std::exp(-1.0f / (attack * 0.001f * 44100.0f));
        band.releaseCoeff = std::exp(-1.0f / (release * 0.001f * 44100.0f));
    }
}

float VocoderEffect::processBand(const Band& band, float input, float carrier) {
    // Bandpass-Filter für Modulator und Carrier
    float modulatorOutput = input; // Hier würde die eigentliche Filterung stattfinden
    float carrierOutput = carrier; // Hier würde die eigentliche Filterung stattfinden
    
    // Hüllkurven berechnen
    updateEnvelopes(const_cast<Band&>(band), modulatorOutput, carrierOutput);
    
    // Modulator-Hüllkurve auf Carrier anwenden
    return carrierOutput * band.envelope;
}

void VocoderEffect::updateEnvelopes(Band& band, float input, float carrier) {
    // Modulator-Hüllkurve
    float modulatorLevel = std::abs(input);
    if (modulatorLevel > band.modulatorEnvelope) {
        band.modulatorEnvelope = modulatorLevel;
    } else {
        band.modulatorEnvelope = modulatorLevel + (band.modulatorEnvelope - modulatorLevel) * band.releaseCoeff;
    }
    
    // Carrier-Hüllkurve
    float carrierLevel = std::abs(carrier);
    if (carrierLevel > band.carrierEnvelope) {
        band.carrierEnvelope = carrierLevel;
    } else {
        band.carrierEnvelope = carrierLevel + (band.carrierEnvelope - carrierLevel) * band.releaseCoeff;
    }
    
    // Gesamt-Hüllkurve
    band.envelope = band.modulatorEnvelope * band.carrierEnvelope;
}

void VocoderEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        bands = 16.0f;
        q = 1.0f;
        attack = 10.0f;
        release = 100.0f;
        mix = 0.5f;
        level = 0.5f;
        quality = 0.5f;
    }
    else if (presetName == "Heavy") {
        bands = 24.0f;
        q = 2.0f;
        attack = 5.0f;
        release = 50.0f;
        mix = 0.7f;
        level = 0.7f;
        quality = 0.7f;
    }
    else if (presetName == "Subtle") {
        bands = 8.0f;
        q = 0.5f;
        attack = 20.0f;
        release = 200.0f;
        mix = 0.3f;
        level = 0.3f;
        quality = 0.3f;
    }
    
    initializeBands();
}

void VocoderEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> VocoderEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 