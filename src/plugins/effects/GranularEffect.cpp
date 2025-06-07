#include "GranularEffect.hpp"
#include <cmath>
#include <random>
#include <algorithm>

namespace VR_DAW {

GranularEffect::GranularEffect()
    : grainSize(50.0f)
    , density(20.0f)
    , pitch(1.0f)
    , spread(0.5f)
    , mix(0.5f)
    , feedback(0.0f)
    , quality(0.5f)
    , automatedGrainSize(false)
    , automatedDensity(false)
    , automatedPitch(false)
    , automatedSpread(false)
    , automatedMix(false)
    , automatedFeedback(false)
    , automatedQuality(false)
    , bufferSize(44100 * 2) // 2 Sekunden Buffer
    , writePos(0)
    , timeSinceLastGrain(0.0f)
{
    initializeGrains();
}

GranularEffect::~GranularEffect() {
}

bool GranularEffect::initialize() {
    buffer.resize(bufferSize, 0.0f);
    writePos = 0;
    timeSinceLastGrain = 0.0f;
    initializeGrains();
    return true;
}

void GranularEffect::shutdown() {
    buffer.clear();
}

std::vector<PluginParameter> GranularEffect::getParameters() const {
    return {
        {"grainSize", "Grain Size", PluginParameter::Type::Float, 1.0f, 1000.0f, grainSize},
        {"density", "Density", PluginParameter::Type::Float, 1.0f, 100.0f, density},
        {"pitch", "Pitch", PluginParameter::Type::Float, 0.25f, 4.0f, pitch},
        {"spread", "Spread", PluginParameter::Type::Float, 0.0f, 1.0f, spread},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"feedback", "Feedback", PluginParameter::Type::Float, 0.0f, 0.9f, feedback},
        {"quality", "Quality", PluginParameter::Type::Float, 0.0f, 1.0f, quality}
    };
}

void GranularEffect::setParameter(const std::string& name, float value) {
    if (name == "grainSize") grainSize = value;
    else if (name == "density") density = value;
    else if (name == "pitch") pitch = value;
    else if (name == "spread") spread = value;
    else if (name == "mix") mix = value;
    else if (name == "feedback") feedback = value;
    else if (name == "quality") quality = value;
}

float GranularEffect::getParameter(const std::string& name) const {
    if (name == "grainSize") return grainSize;
    if (name == "density") return density;
    if (name == "pitch") return pitch;
    if (name == "spread") return spread;
    if (name == "mix") return mix;
    if (name == "feedback") return feedback;
    if (name == "quality") return quality;
    return 0.0f;
}

void GranularEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "grainSize") automatedGrainSize = automated;
    else if (name == "density") automatedDensity = automated;
    else if (name == "pitch") automatedPitch = automated;
    else if (name == "spread") automatedSpread = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "feedback") automatedFeedback = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool GranularEffect::isParameterAutomated(const std::string& name) const {
    if (name == "grainSize") return automatedGrainSize;
    if (name == "density") return automatedDensity;
    if (name == "pitch") return automatedPitch;
    if (name == "spread") return automatedSpread;
    if (name == "mix") return automatedMix;
    if (name == "feedback") return automatedFeedback;
    if (name == "quality") return automatedQuality;
    return false;
}

void GranularEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // Input in Buffer schreiben
        this->buffer[writePos] = (left + right) * 0.5f;
        writePos = (writePos + 1) % bufferSize;
        
        // Grains aktualisieren
        updateGrains();
        
        // Grain-Output berechnen
        float grainOutput = 0.0f;
        for (auto& grain : grains) {
            if (grain.active) {
                grainOutput += processGrain(grain);
            }
        }
        
        // Feedback
        if (feedback > 0.0f) {
            this->buffer[writePos] += grainOutput * feedback;
        }
        
        // Mix
        buffer[i] = left * (1.0f - mix) + grainOutput * mix;
        buffer[i + 1] = right * (1.0f - mix) + grainOutput * mix;
        
        // Zeit für nächsten Grain aktualisieren
        timeSinceLastGrain += 1.0f / 44100.0f;
        if (timeSinceLastGrain >= 1.0f / density) {
            addGrain();
            timeSinceLastGrain = 0.0f;
        }
    }
}

void GranularEffect::initializeGrains() {
    grains.clear();
    int numGrains = static_cast<int>(density * 2); // Doppelte Anzahl für Überlappung
    grains.resize(numGrains);
    
    for (auto& grain : grains) {
        grain.active = false;
        grain.position = 0.0f;
        grain.length = 0.0f;
        grain.pitch = 1.0f;
        grain.pan = 0.0f;
        grain.volume = 0.0f;
        grain.age = 0.0f;
    }
}

void GranularEffect::updateGrains() {
    for (auto& grain : grains) {
        if (grain.active) {
            grain.age += 1.0f / 44100.0f;
            if (grain.age >= grain.length) {
                grain.active = false;
            }
        }
    }
}

float GranularEffect::processGrain(const Grain& grain) {
    // Position im Buffer berechnen
    float readPos = grain.position;
    int pos1 = static_cast<int>(readPos);
    int pos2 = (pos1 + 1) % bufferSize;
    float frac = readPos - pos1;
    
    // Lineare Interpolation
    float sample = buffer[pos1] * (1.0f - frac) + buffer[pos2] * frac;
    
    // Fenster-Funktion
    float window = getWindowValue(grain.age / grain.length);
    
    return sample * window * grain.volume;
}

void GranularEffect::addGrain() {
    // Inaktiven Grain finden
    for (auto& grain : grains) {
        if (!grain.active) {
            // Zufällige Position im Buffer
            float position = static_cast<float>(writePos);
            position -= grainSize * 0.001f * 44100.0f * (1.0f + spread * (rand() / RAND_MAX - 0.5f));
            if (position < 0.0f) position += bufferSize;
            
            // Grain-Parameter setzen
            grain.position = position;
            grain.length = grainSize * 0.001f * 44100.0f;
            grain.pitch = pitch * (1.0f + spread * (rand() / RAND_MAX - 0.5f));
            grain.pan = spread * (rand() / RAND_MAX - 0.5f);
            grain.volume = 1.0f;
            grain.age = 0.0f;
            grain.active = true;
            
            break;
        }
    }
}

float GranularEffect::getWindowValue(float position) {
    // Hanning-Fenster
    return 0.5f * (1.0f - std::cos(2.0f * M_PI * position));
}

void GranularEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        grainSize = 50.0f;
        density = 20.0f;
        pitch = 1.0f;
        spread = 0.5f;
        mix = 0.5f;
        feedback = 0.0f;
        quality = 0.5f;
    }
    else if (presetName == "Heavy") {
        grainSize = 20.0f;
        density = 50.0f;
        pitch = 0.5f;
        spread = 0.8f;
        mix = 0.7f;
        feedback = 0.3f;
        quality = 0.7f;
    }
    else if (presetName == "Subtle") {
        grainSize = 100.0f;
        density = 10.0f;
        pitch = 1.0f;
        spread = 0.2f;
        mix = 0.3f;
        feedback = 0.0f;
        quality = 0.3f;
    }
    
    initializeGrains();
}

void GranularEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> GranularEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 