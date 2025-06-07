#include "EQEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

EQEffect::EQEffect() {
    // 10 Bands initialisieren
    bands.resize(10);
    
    // Frequenzen logarithmisch verteilen
    float minFreq = 20.0f;
    float maxFreq = 20000.0f;
    float freqStep = std::pow(maxFreq / minFreq, 1.0f / 9.0f);
    
    for (size_t i = 0; i < bands.size(); ++i) {
        bands[i].frequency = minFreq * std::pow(freqStep, static_cast<float>(i));
        bands[i].gain = 0.0f;
        bands[i].q = 1.0f;
        bands[i].enabled = true;
        bands[i].automated = false;
    }
    
    // Filter-Koeffizienten und Zustände initialisieren
    coeffs.resize(bands.size());
    states.resize(bands.size());
    
    // Koeffizienten für alle Bands aktualisieren
    for (size_t i = 0; i < bands.size(); ++i) {
        updateFilterCoefficients(i);
    }
}

EQEffect::~EQEffect() {
}

bool EQEffect::initialize() {
    return true;
}

void EQEffect::shutdown() {
}

std::vector<PluginParameter> EQEffect::getParameters() const {
    std::vector<PluginParameter> params;
    
    // Parameter für jeden Band
    for (size_t i = 0; i < bands.size(); ++i) {
        std::string prefix = "band" + std::to_string(i + 1) + "_";
        
        params.push_back({
            prefix + "frequency",
            "Band " + std::to_string(i + 1) + " Frequency",
            PluginParameter::Type::Float,
            20.0f,
            20000.0f,
            bands[i].frequency
        });
        
        params.push_back({
            prefix + "gain",
            "Band " + std::to_string(i + 1) + " Gain",
            PluginParameter::Type::Float,
            -12.0f,
            12.0f,
            bands[i].gain
        });
        
        params.push_back({
            prefix + "q",
            "Band " + std::to_string(i + 1) + " Q",
            PluginParameter::Type::Float,
            0.1f,
            10.0f,
            bands[i].q
        });
        
        params.push_back({
            prefix + "enabled",
            "Band " + std::to_string(i + 1) + " Enabled",
            PluginParameter::Type::Bool,
            0.0f,
            1.0f,
            bands[i].enabled ? 1.0f : 0.0f
        });
    }
    
    return params;
}

void EQEffect::setParameter(const std::string& name, float value) {
    // Band-Parameter parsen
    size_t bandIndex;
    std::string paramType;
    
    if (name.substr(0, 4) == "band" && name[5] == '_') {
        bandIndex = name[4] - '1';
        paramType = name.substr(6);
        
        if (bandIndex < bands.size()) {
            if (paramType == "frequency") {
                bands[bandIndex].frequency = value;
                updateFilterCoefficients(bandIndex);
            }
            else if (paramType == "gain") {
                bands[bandIndex].gain = value;
                updateFilterCoefficients(bandIndex);
            }
            else if (paramType == "q") {
                bands[bandIndex].q = value;
                updateFilterCoefficients(bandIndex);
            }
            else if (paramType == "enabled") {
                bands[bandIndex].enabled = value > 0.5f;
            }
        }
    }
}

float EQEffect::getParameter(const std::string& name) const {
    // Band-Parameter parsen
    size_t bandIndex;
    std::string paramType;
    
    if (name.substr(0, 4) == "band" && name[5] == '_') {
        bandIndex = name[4] - '1';
        paramType = name.substr(6);
        
        if (bandIndex < bands.size()) {
            if (paramType == "frequency") return bands[bandIndex].frequency;
            if (paramType == "gain") return bands[bandIndex].gain;
            if (paramType == "q") return bands[bandIndex].q;
            if (paramType == "enabled") return bands[bandIndex].enabled ? 1.0f : 0.0f;
        }
    }
    
    return 0.0f;
}

void EQEffect::setParameterAutomated(const std::string& name, bool automated) {
    // Band-Parameter parsen
    size_t bandIndex;
    std::string paramType;
    
    if (name.substr(0, 4) == "band" && name[5] == '_') {
        bandIndex = name[4] - '1';
        paramType = name.substr(6);
        
        if (bandIndex < bands.size()) {
            if (paramType == "gain") {
                bands[bandIndex].automated = automated;
            }
        }
    }
}

bool EQEffect::isParameterAutomated(const std::string& name) const {
    // Band-Parameter parsen
    size_t bandIndex;
    std::string paramType;
    
    if (name.substr(0, 4) == "band" && name[5] == '_') {
        bandIndex = name[4] - '1';
        paramType = name.substr(6);
        
        if (bandIndex < bands.size()) {
            if (paramType == "gain") {
                return bands[bandIndex].automated;
            }
        }
    }
    
    return false;
}

void EQEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // Jeden Band verarbeiten
        for (size_t band = 0; band < bands.size(); ++band) {
            if (bands[band].enabled) {
                left = processFilter(left, band);
                right = processFilter(right, band);
            }
        }
        
        // Ausgabe
        buffer[i] = left;
        buffer[i + 1] = right;
    }
}

void EQEffect::updateFilterCoefficients(size_t bandIndex) {
    float w0 = 2.0f * M_PI * bands[bandIndex].frequency / 44100.0f;
    float alpha = std::sin(w0) / (2.0f * bands[bandIndex].q);
    float A = std::pow(10.0f, bands[bandIndex].gain / 40.0f);
    
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * std::cos(w0);
    float a2 = 1.0f - alpha;
    float b0 = 1.0f + alpha * A;
    float b1 = a1;
    float b2 = 1.0f - alpha * A;
    
    // Normalisieren
    float scale = 1.0f / a0;
    coeffs[bandIndex].a0 = b0 * scale;
    coeffs[bandIndex].a1 = b1 * scale;
    coeffs[bandIndex].a2 = b2 * scale;
    coeffs[bandIndex].b1 = a1 * scale;
    coeffs[bandIndex].b2 = a2 * scale;
}

float EQEffect::processFilter(float input, size_t bandIndex) {
    float output = coeffs[bandIndex].a0 * input
                 + coeffs[bandIndex].a1 * states[bandIndex].x1
                 + coeffs[bandIndex].a2 * states[bandIndex].x2
                 - coeffs[bandIndex].b1 * states[bandIndex].y1
                 - coeffs[bandIndex].b2 * states[bandIndex].y2;
    
    // Zustand aktualisieren
    states[bandIndex].x2 = states[bandIndex].x1;
    states[bandIndex].x1 = input;
    states[bandIndex].y2 = states[bandIndex].y1;
    states[bandIndex].y1 = output;
    
    return output;
}

float EQEffect::dbToLinear(float db) {
    return std::pow(10.0f, db * 0.05f);
}

void EQEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Flat") {
        for (auto& band : bands) {
            band.gain = 0.0f;
            band.q = 1.0f;
            band.enabled = true;
        }
    }
    else if (presetName == "Bass Boost") {
        // Bass-Boost Preset
        bands[0].frequency = 60.0f;
        bands[0].gain = 6.0f;
        bands[0].q = 1.0f;
        bands[0].enabled = true;
        
        bands[1].frequency = 120.0f;
        bands[1].gain = 3.0f;
        bands[1].q = 1.0f;
        bands[1].enabled = true;
        
        // Andere Bands zurücksetzen
        for (size_t i = 2; i < bands.size(); ++i) {
            bands[i].gain = 0.0f;
            bands[i].q = 1.0f;
            bands[i].enabled = true;
        }
    }
    else if (presetName == "Treble Boost") {
        // Treble-Boost Preset
        bands[8].frequency = 8000.0f;
        bands[8].gain = 6.0f;
        bands[8].q = 1.0f;
        bands[8].enabled = true;
        
        bands[9].frequency = 16000.0f;
        bands[9].gain = 3.0f;
        bands[9].q = 1.0f;
        bands[9].enabled = true;
        
        // Andere Bands zurücksetzen
        for (size_t i = 0; i < 8; ++i) {
            bands[i].gain = 0.0f;
            bands[i].q = 1.0f;
            bands[i].enabled = true;
        }
    }
    
    // Koeffizienten für alle Bands aktualisieren
    for (size_t i = 0; i < bands.size(); ++i) {
        updateFilterCoefficients(i);
    }
}

void EQEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> EQEffect::getAvailablePresets() const {
    return {"Flat", "Bass Boost", "Treble Boost"};
}

} // namespace VR_DAW 