#include "VibratoEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

VibratoEffect::VibratoEffect()
    : rate(2.0f)
    , depth(0.5f)
    , mix(0.5f)
    , stereoWidth(0.5f)
    , syncRate(0.25f)
    , shape(0.0f)
    , phase(0.0f)
    , automatedRate(false)
    , automatedDepth(false)
    , automatedMix(false)
    , automatedStereoWidth(false)
    , automatedSyncRate(false)
    , automatedShape(false)
    , automatedPhase(false)
    , lfoPhase(0.0f)
    , lastLeft(0.0f)
    , lastRight(0.0f)
    , bufferSize(44100)  // 1 Sekunde bei 44.1kHz
    , writePos(0)
    , readPos(0)
{
    delayBuffer.resize(bufferSize * 2, 0.0f);  // Stereo-Buffer
}

VibratoEffect::~VibratoEffect() {}

bool VibratoEffect::initialize() {
    return true;
}

void VibratoEffect::shutdown() {
    delayBuffer.clear();
}

std::vector<PluginParameter> VibratoEffect::getParameters() const {
    return {
        {"rate", "Rate", PluginParameter::Type::Float, 0.1f, 10.0f, rate},
        {"depth", "Depth", PluginParameter::Type::Float, 0.0f, 1.0f, depth},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"stereoWidth", "Stereo Width", PluginParameter::Type::Float, 0.0f, 1.0f, stereoWidth},
        {"syncRate", "Sync Rate", PluginParameter::Type::Float, 0.0625f, 1.0f, syncRate},
        {"shape", "Shape", PluginParameter::Type::Float, 0.0f, 1.0f, shape},
        {"phase", "Phase", PluginParameter::Type::Float, 0.0f, 1.0f, phase}
    };
}

void VibratoEffect::setParameter(const std::string& name, float value) {
    if (name == "rate") rate = value;
    else if (name == "depth") depth = value;
    else if (name == "mix") mix = value;
    else if (name == "stereoWidth") stereoWidth = value;
    else if (name == "syncRate") syncRate = value;
    else if (name == "shape") shape = value;
    else if (name == "phase") phase = value;
}

float VibratoEffect::getParameter(const std::string& name) const {
    if (name == "rate") return rate;
    if (name == "depth") return depth;
    if (name == "mix") return mix;
    if (name == "stereoWidth") return stereoWidth;
    if (name == "syncRate") return syncRate;
    if (name == "shape") return shape;
    if (name == "phase") return phase;
    return 0.0f;
}

void VibratoEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "rate") automatedRate = automated;
    else if (name == "depth") automatedDepth = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "stereoWidth") automatedStereoWidth = automated;
    else if (name == "syncRate") automatedSyncRate = automated;
    else if (name == "shape") automatedShape = automated;
    else if (name == "phase") automatedPhase = automated;
}

bool VibratoEffect::isParameterAutomated(const std::string& name) const {
    if (name == "rate") return automatedRate;
    if (name == "depth") return automatedDepth;
    if (name == "mix") return automatedMix;
    if (name == "stereoWidth") return automatedStereoWidth;
    if (name == "syncRate") return automatedSyncRate;
    if (name == "shape") return automatedShape;
    if (name == "phase") return automatedPhase;
    return false;
}

void VibratoEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        float time = static_cast<float>(i) / 44100.0f;  // Sample-Rate: 44.1kHz
        
        // LFO für Modulation
        float lfoLeft = processLFO(time);
        float lfoRight = processLFO(time + phase);
        
        // Delay-Zeit berechnen
        float delayTimeLeft = lfoLeft * depth * bufferSize;
        float delayTimeRight = lfoRight * depth * bufferSize;
        
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // Delay-Buffer schreiben
        delayBuffer[writePos] = left;
        delayBuffer[writePos + 1] = right;
        writePos = (writePos + 2) % (bufferSize * 2);
        
        // Delay-Buffer lesen
        float delayedLeft = interpolate(delayBuffer.data(), writePos - delayTimeLeft * 2);
        float delayedRight = interpolate(delayBuffer.data(), writePos - delayTimeRight * 2);
        
        // Stereo-Width
        float mid = (delayedLeft + delayedRight) * 0.5f;
        float side = (delayedLeft - delayedRight) * 0.5f;
        delayedLeft = mid + side * stereoWidth;
        delayedRight = mid - side * stereoWidth;
        
        // Mix
        buffer[i] = left * (1.0f - mix) + delayedLeft * mix;
        buffer[i + 1] = right * (1.0f - mix) + delayedRight * mix;
    }
}

float VibratoEffect::processLFO(float time) {
    lfoPhase += rate * (1.0f / 44100.0f);
    if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
    
    // LFO-Form basierend auf Shape-Parameter
    float lfo;
    if (shape < 0.5f) {
        // Sine zu Triangle
        lfo = std::sin(2.0f * M_PI * lfoPhase);
        float tri = 2.0f * std::abs(2.0f * lfoPhase - 1.0f) - 1.0f;
        lfo = lfo * (1.0f - shape * 2.0f) + tri * (shape * 2.0f);
    } else {
        // Triangle zu Square
        float tri = 2.0f * std::abs(2.0f * lfoPhase - 1.0f) - 1.0f;
        float square = (lfoPhase < 0.5f) ? 1.0f : -1.0f;
        lfo = tri * (2.0f - shape * 2.0f) + square * (shape * 2.0f - 1.0f);
    }
    
    return (lfo + 1.0f) * 0.5f;  // Normalisieren auf 0-1
}

float VibratoEffect::calculateSyncTime() {
    // Sync-Zeit basierend auf BPM berechnen (angenommen 120 BPM)
    return (60.0f / 120.0f) * syncRate * 1000.0f;
}

float VibratoEffect::interpolate(float* buffer, float position) {
    // Lineare Interpolation
    int pos1 = static_cast<int>(position);
    int pos2 = (pos1 + 1) % (bufferSize * 2);
    float frac = position - pos1;
    
    return buffer[pos1] * (1.0f - frac) + buffer[pos2] * frac;
}

void VibratoEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        rate = 2.0f;
        depth = 0.5f;
        mix = 0.5f;
        stereoWidth = 0.5f;
        syncRate = 0.25f;
        shape = 0.0f;
        phase = 0.0f;
    }
    else if (presetName == "Fast") {
        rate = 5.0f;
        depth = 0.7f;
        mix = 0.6f;
        stereoWidth = 0.7f;
        syncRate = 0.125f;
        shape = 0.3f;
        phase = 0.5f;
    }
    else if (presetName == "Slow") {
        rate = 0.5f;
        depth = 0.3f;
        mix = 0.4f;
        stereoWidth = 0.3f;
        syncRate = 0.5f;
        shape = 0.0f;
        phase = 0.0f;
    }
}

void VibratoEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> VibratoEffect::getAvailablePresets() const {
    return {"Standard", "Fast", "Slow"};
}

} // namespace VR_DAW 