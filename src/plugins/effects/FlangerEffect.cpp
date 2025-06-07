#include "FlangerEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

FlangerEffect::FlangerEffect()
    : rate(1.0f)
    , depth(0.5f)
    , feedback(0.3f)
    , mix(0.5f)
    , stereoWidth(0.5f)
    , syncRate(0.25f)
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
    // Verzögerungsbuffer initialisieren (max. 20ms bei 44.1kHz)
    size_t bufferSize = static_cast<size_t>(20.0f * 0.001f * 44100.0f) * 2;  // Stereo
    delayBuffer.resize(bufferSize, 0.0f);
}

FlangerEffect::~FlangerEffect() {
    delayBuffer.clear();
}

bool FlangerEffect::initialize() {
    return true;
}

void FlangerEffect::shutdown() {
    delayBuffer.clear();
}

std::vector<PluginParameter> FlangerEffect::getParameters() const {
    return {
        {"rate", "Rate", PluginParameter::Type::Float, 0.1f, 10.0f, rate},
        {"depth", "Depth", PluginParameter::Type::Float, 0.0f, 1.0f, depth},
        {"feedback", "Feedback", PluginParameter::Type::Float, 0.0f, 0.9f, feedback},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"stereoWidth", "Stereo Width", PluginParameter::Type::Float, 0.0f, 1.0f, stereoWidth},
        {"syncRate", "Sync Rate", PluginParameter::Type::Float, 0.0625f, 1.0f, syncRate},
        {"phase", "Phase", PluginParameter::Type::Float, 0.0f, 1.0f, phase}
    };
}

void FlangerEffect::setParameter(const std::string& name, float value) {
    if (name == "rate") rate = value;
    else if (name == "depth") depth = value;
    else if (name == "feedback") feedback = value;
    else if (name == "mix") mix = value;
    else if (name == "stereoWidth") stereoWidth = value;
    else if (name == "syncRate") syncRate = value;
    else if (name == "phase") phase = value;
}

float FlangerEffect::getParameter(const std::string& name) const {
    if (name == "rate") return rate;
    if (name == "depth") return depth;
    if (name == "feedback") return feedback;
    if (name == "mix") return mix;
    if (name == "stereoWidth") return stereoWidth;
    if (name == "syncRate") return syncRate;
    if (name == "phase") return phase;
    return 0.0f;
}

void FlangerEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "rate") automatedRate = automated;
    else if (name == "depth") automatedDepth = automated;
    else if (name == "feedback") automatedFeedback = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "stereoWidth") automatedStereoWidth = automated;
    else if (name == "syncRate") automatedSyncRate = automated;
    else if (name == "phase") automatedPhase = automated;
}

bool FlangerEffect::isParameterAutomated(const std::string& name) const {
    if (name == "rate") return automatedRate;
    if (name == "depth") return automatedDepth;
    if (name == "feedback") return automatedFeedback;
    if (name == "mix") return automatedMix;
    if (name == "stereoWidth") return automatedStereoWidth;
    if (name == "syncRate") return automatedSyncRate;
    if (name == "phase") return automatedPhase;
    return false;
}

void FlangerEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // LFO-Wert berechnen
        float lfoValue = processLFO();
        
        // Verzögerungszeit berechnen
        float delayTime = 1.0f + lfoValue * depth;  // 1-2ms
        size_t delaySamples = static_cast<size_t>(delayTime * 0.001f * 44100.0f) * 2;  // Stereo
        
        // Verzögerungsbuffer schreiben
        delayBuffer[writePos] = left;
        delayBuffer[writePos + 1] = right;
        
        // Verzögerungsbuffer lesen
        size_t readPos = (writePos - delaySamples + delayBuffer.size()) % delayBuffer.size();
        float delayedLeft = delayBuffer[readPos];
        float delayedRight = delayBuffer[readPos + 1];
        
        // Feedback
        delayBuffer[writePos] += delayedLeft * feedback;
        delayBuffer[writePos + 1] += delayedRight * feedback;
        
        // Stereo-Width
        float mid = (delayedLeft + delayedRight) * 0.5f;
        float side = (delayedLeft - delayedRight) * 0.5f;
        delayedLeft = mid + side * stereoWidth;
        delayedRight = mid - side * stereoWidth;
        
        // Mix
        buffer[i] = left * (1.0f - mix) + delayedLeft * mix;
        buffer[i + 1] = right * (1.0f - mix) + delayedRight * mix;
        
        // Buffer-Position aktualisieren
        writePos = (writePos + 2) % delayBuffer.size();
    }
}

float FlangerEffect::processLFO() {
    // LFO-Phase aktualisieren
    float lfoRate = rate;
    if (syncRate > 0.0f) {
        lfoRate = calculateSyncTime();
    }
    
    lfoPhase += lfoRate / 44100.0f;
    if (lfoPhase >= 1.0f) {
        lfoPhase -= 1.0f;
    }
    
    // Sinus-Welle mit Phase
    return 0.5f * (1.0f + std::sin(2.0f * M_PI * (lfoPhase + phase)));
}

float FlangerEffect::calculateSyncTime() {
    // Sync-Zeit basierend auf BPM berechnen (angenommen 120 BPM)
    return (60.0f / 120.0f) * syncRate;
}

void FlangerEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        rate = 1.0f;
        depth = 0.5f;
        feedback = 0.3f;
        mix = 0.5f;
        stereoWidth = 0.5f;
        syncRate = 0.25f;
        phase = 0.0f;
    }
    else if (presetName == "Jet") {
        rate = 2.0f;
        depth = 0.7f;
        feedback = 0.5f;
        mix = 0.7f;
        stereoWidth = 0.7f;
        syncRate = 0.5f;
        phase = 0.25f;
    }
    else if (presetName == "Subtle") {
        rate = 0.5f;
        depth = 0.3f;
        feedback = 0.2f;
        mix = 0.3f;
        stereoWidth = 0.3f;
        syncRate = 0.125f;
        phase = 0.0f;
    }
}

void FlangerEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> FlangerEffect::getAvailablePresets() const {
    return {"Standard", "Jet", "Subtle"};
}

} // namespace VR_DAW 
} // namespace VR_DAW 