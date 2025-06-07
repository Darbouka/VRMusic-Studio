#include "ChorusEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

ChorusEffect::ChorusEffect()
    : rate(2.0f)
    , depth(0.5f)
    , feedback(0.3f)
    , mix(0.5f)
    , stereoWidth(0.5f)
    , syncRate(0.0f)
    , phase(0.0f)
    , automatedRate(false)
    , automatedDepth(false)
    , automatedFeedback(false)
    , automatedMix(false)
    , automatedStereoWidth(false)
    , automatedSyncRate(false)
    , automatedPhase(false)
    , lfoPhase(0.0f)
    , writePos(0)
    , readPos(0)
{
    // Delay-Buffer für 20ms bei 44.1kHz
    const size_t maxDelay = static_cast<size_t>(0.02f * 44100.0f);
    delayBuffer.resize(maxDelay, 0.0f);
}

ChorusEffect::~ChorusEffect() {
    delayBuffer.clear();
}

bool ChorusEffect::initialize() {
    writePos = 0;
    readPos = 0;
    lfoPhase = 0.0f;
    return true;
}

void ChorusEffect::shutdown() {
    delayBuffer.clear();
}

std::vector<PluginParameter> ChorusEffect::getParameters() const {
    return {
        {"rate", "Rate", PluginParameter::Type::Float, 0.1f, 10.0f, rate},
        {"depth", "Depth", PluginParameter::Type::Float, 0.0f, 1.0f, depth},
        {"feedback", "Feedback", PluginParameter::Type::Float, 0.0f, 0.9f, feedback},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"stereoWidth", "Stereo Width", PluginParameter::Type::Float, 0.0f, 1.0f, stereoWidth},
        {"syncRate", "Sync Rate", PluginParameter::Type::Float, 0.0f, 1.0f, syncRate},
        {"phase", "Phase", PluginParameter::Type::Float, 0.0f, 1.0f, phase}
    };
}

void ChorusEffect::setParameter(const std::string& name, float value) {
    if (name == "rate") rate = value;
    else if (name == "depth") depth = value;
    else if (name == "feedback") feedback = value;
    else if (name == "mix") mix = value;
    else if (name == "stereoWidth") stereoWidth = value;
    else if (name == "syncRate") syncRate = value;
    else if (name == "phase") phase = value;
}

float ChorusEffect::getParameter(const std::string& name) const {
    if (name == "rate") return rate;
    if (name == "depth") return depth;
    if (name == "feedback") return feedback;
    if (name == "mix") return mix;
    if (name == "stereoWidth") return stereoWidth;
    if (name == "syncRate") return syncRate;
    if (name == "phase") return phase;
    return 0.0f;
}

void ChorusEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "rate") automatedRate = automated;
    else if (name == "depth") automatedDepth = automated;
    else if (name == "feedback") automatedFeedback = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "stereoWidth") automatedStereoWidth = automated;
    else if (name == "syncRate") automatedSyncRate = automated;
    else if (name == "phase") automatedPhase = automated;
}

bool ChorusEffect::isParameterAutomated(const std::string& name) const {
    if (name == "rate") return automatedRate;
    if (name == "depth") return automatedDepth;
    if (name == "feedback") return automatedFeedback;
    if (name == "mix") return automatedMix;
    if (name == "stereoWidth") return automatedStereoWidth;
    if (name == "syncRate") return automatedSyncRate;
    if (name == "phase") return automatedPhase;
    return false;
}

void ChorusEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // LFO berechnen
        float lfoValue = processLFO();
        
        // Delay-Zeit berechnen
        float delayTime = 0.005f + 0.015f * depth * (0.5f + 0.5f * lfoValue);
        size_t delaySamples = static_cast<size_t>(delayTime * 44100.0f);
        
        // Delay-Buffer aktualisieren
        delayBuffer[writePos] = (left + right) * 0.5f;
        writePos = (writePos + 1) % delayBuffer.size();
        
        // Verzögertes Signal lesen
        size_t readPos1 = (writePos + delayBuffer.size() - delaySamples) % delayBuffer.size();
        size_t readPos2 = (writePos + delayBuffer.size() - delaySamples - 1) % delayBuffer.size();
        
        float delayed1 = delayBuffer[readPos1];
        float delayed2 = delayBuffer[readPos2];
        
        // Interpolation
        float delayed = delayed1 + (delayed2 - delayed1) * (delayTime * 44100.0f - delaySamples);
        
        // Feedback
        delayed *= feedback;
        
        // Stereo-Width
        float mid = delayed;
        float side = delayed * stereoWidth;
        
        // Mix
        buffer[i] = left * (1.0f - mix) + (mid + side) * mix;
        buffer[i + 1] = right * (1.0f - mix) + (mid - side) * mix;
    }
}

float ChorusEffect::processLFO() {
    // LFO-Phase aktualisieren
    float currentRate = syncRate > 0.0f ? calculateSyncTime() : rate;
    lfoPhase += currentRate / 44100.0f;
    if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
    
    // LFO-Wert berechnen
    return 0.5f + 0.5f * std::sin(2.0f * M_PI * (lfoPhase + phase));
}

float ChorusEffect::calculateSyncTime() {
    // Sync-Zeit basierend auf BPM berechnen
    const float bpm = 120.0f; // Feste BPM für jetzt
    float syncTime = 60.0f / bpm;
    
    // Sync-Rate auf verschiedene Notenlängen mappen
    if (syncRate < 0.25f) return syncTime * 4.0f;      // Ganze Note
    else if (syncRate < 0.5f) return syncTime * 2.0f;  // Halbe Note
    else if (syncRate < 0.75f) return syncTime;        // Viertel Note
    else return syncTime * 0.5f;                       // Achtel Note
}

void ChorusEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        rate = 2.0f;
        depth = 0.5f;
        feedback = 0.3f;
        mix = 0.5f;
        stereoWidth = 0.5f;
        syncRate = 0.0f;
        phase = 0.0f;
    }
    else if (presetName == "Heavy") {
        rate = 1.5f;
        depth = 0.7f;
        feedback = 0.5f;
        mix = 0.7f;
        stereoWidth = 0.7f;
        syncRate = 0.0f;
        phase = 0.25f;
    }
    else if (presetName == "Subtle") {
        rate = 3.0f;
        depth = 0.3f;
        feedback = 0.2f;
        mix = 0.3f;
        stereoWidth = 0.3f;
        syncRate = 0.0f;
        phase = 0.0f;
    }
}

void ChorusEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> ChorusEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 