#include "TremoloEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

TremoloEffect::TremoloEffect()
    : rate(5.0f)
    , depth(0.5f)
    , shape(0.0f)
    , mix(0.5f)
    , syncRate(0.0f)
    , phase(0.0f)
    , stereoPhase(0.0f)
    , automatedRate(false)
    , automatedDepth(false)
    , automatedShape(false)
    , automatedMix(false)
    , automatedSyncRate(false)
    , automatedPhase(false)
    , automatedStereoPhase(false)
    , lfoPhase(0.0f)
{
}

TremoloEffect::~TremoloEffect() {
}

bool TremoloEffect::initialize() {
    lfoPhase = 0.0f;
    return true;
}

void TremoloEffect::shutdown() {
}

std::vector<PluginParameter> TremoloEffect::getParameters() const {
    return {
        {"rate", "Rate", PluginParameter::Type::Float, 0.1f, 20.0f, rate},
        {"depth", "Depth", PluginParameter::Type::Float, 0.0f, 1.0f, depth},
        {"shape", "Shape", PluginParameter::Type::Float, 0.0f, 1.0f, shape},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"syncRate", "Sync Rate", PluginParameter::Type::Float, 0.0f, 1.0f, syncRate},
        {"phase", "Phase", PluginParameter::Type::Float, 0.0f, 1.0f, phase},
        {"stereoPhase", "Stereo Phase", PluginParameter::Type::Float, 0.0f, 1.0f, stereoPhase}
    };
}

void TremoloEffect::setParameter(const std::string& name, float value) {
    if (name == "rate") rate = value;
    else if (name == "depth") depth = value;
    else if (name == "shape") shape = value;
    else if (name == "mix") mix = value;
    else if (name == "syncRate") syncRate = value;
    else if (name == "phase") phase = value;
    else if (name == "stereoPhase") stereoPhase = value;
}

float TremoloEffect::getParameter(const std::string& name) const {
    if (name == "rate") return rate;
    if (name == "depth") return depth;
    if (name == "shape") return shape;
    if (name == "mix") return mix;
    if (name == "syncRate") return syncRate;
    if (name == "phase") return phase;
    if (name == "stereoPhase") return stereoPhase;
    return 0.0f;
}

void TremoloEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "rate") automatedRate = automated;
    else if (name == "depth") automatedDepth = automated;
    else if (name == "shape") automatedShape = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "syncRate") automatedSyncRate = automated;
    else if (name == "phase") automatedPhase = automated;
    else if (name == "stereoPhase") automatedStereoPhase = automated;
}

bool TremoloEffect::isParameterAutomated(const std::string& name) const {
    if (name == "rate") return automatedRate;
    if (name == "depth") return automatedDepth;
    if (name == "shape") return automatedShape;
    if (name == "mix") return automatedMix;
    if (name == "syncRate") return automatedSyncRate;
    if (name == "phase") return automatedPhase;
    if (name == "stereoPhase") return automatedStereoPhase;
    return false;
}

void TremoloEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // LFO für beide Kanäle berechnen
        float lfoLeft = processLFO();
        float lfoRight = processLFO() + stereoPhase;
        
        // Shape-Mischung
        float shapeLeft = shape * (lfoLeft > 0.5f ? 1.0f : 0.0f) + (1.0f - shape) * lfoLeft;
        float shapeRight = shape * (lfoRight > 0.5f ? 1.0f : 0.0f) + (1.0f - shape) * lfoRight;
        
        // Tiefe anwenden
        float modLeft = 1.0f - depth * shapeLeft;
        float modRight = 1.0f - depth * shapeRight;
        
        // Mix
        buffer[i] = left * (1.0f - mix) + left * modLeft * mix;
        buffer[i + 1] = right * (1.0f - mix) + right * modRight * mix;
    }
}

float TremoloEffect::processLFO() {
    // LFO-Phase aktualisieren
    float currentRate = syncRate > 0.0f ? calculateSyncTime() : rate;
    lfoPhase += currentRate / 44100.0f;
    if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
    
    // LFO-Wert berechnen
    return 0.5f + 0.5f * std::sin(2.0f * M_PI * (lfoPhase + phase));
}

float TremoloEffect::calculateSyncTime() {
    // Sync-Zeit basierend auf BPM berechnen
    const float bpm = 120.0f; // Feste BPM für jetzt
    float syncTime = 60.0f / bpm;
    
    // Sync-Rate auf verschiedene Notenlängen mappen
    if (syncRate < 0.25f) return syncTime * 4.0f;      // Ganze Note
    else if (syncRate < 0.5f) return syncTime * 2.0f;  // Halbe Note
    else if (syncRate < 0.75f) return syncTime;        // Viertel Note
    else return syncTime * 0.5f;                       // Achtel Note
}

void TremoloEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        rate = 5.0f;
        depth = 0.5f;
        shape = 0.0f;
        mix = 0.5f;
        syncRate = 0.0f;
        phase = 0.0f;
        stereoPhase = 0.0f;
    }
    else if (presetName == "Heavy") {
        rate = 3.0f;
        depth = 0.7f;
        shape = 0.5f;
        mix = 0.7f;
        syncRate = 0.0f;
        phase = 0.0f;
        stereoPhase = 0.5f;
    }
    else if (presetName == "Subtle") {
        rate = 8.0f;
        depth = 0.3f;
        shape = 0.0f;
        mix = 0.3f;
        syncRate = 0.0f;
        phase = 0.0f;
        stereoPhase = 0.25f;
    }
}

void TremoloEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> TremoloEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 