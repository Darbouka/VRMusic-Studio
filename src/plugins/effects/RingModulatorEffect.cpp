#include "RingModulatorEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

RingModulatorEffect::RingModulatorEffect()
    : frequency(440.0f)
    , mix(0.5f)
    , depth(0.5f)
    , phase(0.0f)
    , syncRate(0.0f)
    , stereoWidth(0.5f)
    , quality(0.5f)
    , automatedFrequency(false)
    , automatedMix(false)
    , automatedDepth(false)
    , automatedPhase(false)
    , automatedSyncRate(false)
    , automatedStereoWidth(false)
    , automatedQuality(false)
    , lfoPhase(0.0f)
    , lastFrequency(440.0f)
    , lastPhase(0.0f)
{
}

RingModulatorEffect::~RingModulatorEffect() {}

bool RingModulatorEffect::initialize() {
    lfoPhase = 0.0f;
    lastFrequency = frequency;
    lastPhase = phase;
    return true;
}

void RingModulatorEffect::shutdown() {}

std::vector<PluginParameter> RingModulatorEffect::getParameters() const {
    return {
        {"frequency", "Frequency", PluginParameter::Type::Float, 0.0f, 2000.0f, frequency},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"depth", "Depth", PluginParameter::Type::Float, 0.0f, 1.0f, depth},
        {"phase", "Phase", PluginParameter::Type::Float, 0.0f, 360.0f, phase},
        {"syncRate", "Sync Rate", PluginParameter::Type::Float, 0.0f, 1.0f, syncRate},
        {"stereoWidth", "Stereo Width", PluginParameter::Type::Float, 0.0f, 1.0f, stereoWidth},
        {"quality", "Quality", PluginParameter::Type::Float, 0.0f, 1.0f, quality}
    };
}

void RingModulatorEffect::setParameter(const std::string& name, float value) {
    if (name == "frequency") frequency = value;
    else if (name == "mix") mix = value;
    else if (name == "depth") depth = value;
    else if (name == "phase") phase = value;
    else if (name == "syncRate") syncRate = value;
    else if (name == "stereoWidth") stereoWidth = value;
    else if (name == "quality") quality = value;
}

float RingModulatorEffect::getParameter(const std::string& name) const {
    if (name == "frequency") return frequency;
    if (name == "mix") return mix;
    if (name == "depth") return depth;
    if (name == "phase") return phase;
    if (name == "syncRate") return syncRate;
    if (name == "stereoWidth") return stereoWidth;
    if (name == "quality") return quality;
    return 0.0f;
}

void RingModulatorEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "frequency") automatedFrequency = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "depth") automatedDepth = automated;
    else if (name == "phase") automatedPhase = automated;
    else if (name == "syncRate") automatedSyncRate = automated;
    else if (name == "stereoWidth") automatedStereoWidth = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool RingModulatorEffect::isParameterAutomated(const std::string& name) const {
    if (name == "frequency") return automatedFrequency;
    if (name == "mix") return automatedMix;
    if (name == "depth") return automatedDepth;
    if (name == "phase") return automatedPhase;
    if (name == "syncRate") return automatedSyncRate;
    if (name == "stereoWidth") return automatedStereoWidth;
    if (name == "quality") return automatedQuality;
    return false;
}

void RingModulatorEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // LFO berechnen
        float lfoValue = processLFO();
        
        // Ring-Modulation anwenden
        float modulatedLeft = left * lfoValue;
        float modulatedRight = right * lfoValue;
        
        // Stereo-Width anwenden
        float mid = (modulatedLeft + modulatedRight) * 0.5f;
        float side = (modulatedLeft - modulatedRight) * 0.5f;
        
        modulatedLeft = mid + side * stereoWidth;
        modulatedRight = mid - side * stereoWidth;
        
        // Mix
        buffer[i] = left * (1.0f - mix) + modulatedLeft * mix;
        buffer[i + 1] = right * (1.0f - mix) + modulatedRight * mix;
    }
}

float RingModulatorEffect::processLFO() {
    // LFO-Phase aktualisieren
    float currentFreq = frequency;
    if (syncRate > 0.0f) {
        currentFreq = calculateSyncTime();
    }
    
    lfoPhase += currentFreq / 44100.0f;
    if (lfoPhase >= 1.0f) {
        lfoPhase -= 1.0f;
    }
    
    // LFO-Wert berechnen
    float phaseRad = (lfoPhase + phase / 360.0f) * 2.0f * M_PI;
    return std::sin(phaseRad) * depth;
}

float RingModulatorEffect::calculateSyncTime() {
    // Feste BPM von 120 annehmen
    const float bpm = 120.0f;
    const float beatsPerSecond = bpm / 60.0f;
    return beatsPerSecond * syncRate;
}

void RingModulatorEffect::updateOscillator() {
    // Hier könnte die Oszillator-Implementierung erweitert werden
    // z.B. für verschiedene Wellenformen oder Interpolation
}

void RingModulatorEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        frequency = 440.0f;
        mix = 0.5f;
        depth = 0.5f;
        phase = 0.0f;
        syncRate = 0.0f;
        stereoWidth = 0.5f;
        quality = 0.5f;
    }
    else if (presetName == "Heavy") {
        frequency = 880.0f;
        mix = 0.7f;
        depth = 0.8f;
        phase = 90.0f;
        syncRate = 0.5f;
        stereoWidth = 0.8f;
        quality = 0.7f;
    }
    else if (presetName == "Subtle") {
        frequency = 220.0f;
        mix = 0.3f;
        depth = 0.3f;
        phase = 0.0f;
        syncRate = 0.0f;
        stereoWidth = 0.3f;
        quality = 0.3f;
    }
}

void RingModulatorEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> RingModulatorEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 