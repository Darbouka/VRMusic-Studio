#include "MultiTapDelayEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

MultiTapDelayEffect::MultiTapDelayEffect()
    : time1(250.0f)
    , time2(500.0f)
    , time3(750.0f)
    , time4(1000.0f)
    , feedback(0.3f)
    , mix(0.5f)
    , quality(0.5f)
    , automatedTime1(false)
    , automatedTime2(false)
    , automatedTime3(false)
    , automatedTime4(false)
    , automatedFeedback(false)
    , automatedMix(false)
    , automatedQuality(false)
    , bufferSize(44100 * 2) // 2 Sekunden Buffer
{
    initializeTaps();
}

MultiTapDelayEffect::~MultiTapDelayEffect() {
}

bool MultiTapDelayEffect::initialize() {
    initializeTaps();
    return true;
}

void MultiTapDelayEffect::shutdown() {
    taps.clear();
}

std::vector<PluginParameter> MultiTapDelayEffect::getParameters() const {
    return {
        {"time1", "Time 1", PluginParameter::Type::Float, 0.0f, 2000.0f, time1},
        {"time2", "Time 2", PluginParameter::Type::Float, 0.0f, 2000.0f, time2},
        {"time3", "Time 3", PluginParameter::Type::Float, 0.0f, 2000.0f, time3},
        {"time4", "Time 4", PluginParameter::Type::Float, 0.0f, 2000.0f, time4},
        {"feedback", "Feedback", PluginParameter::Type::Float, 0.0f, 0.9f, feedback},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"quality", "Quality", PluginParameter::Type::Float, 0.0f, 1.0f, quality}
    };
}

void MultiTapDelayEffect::setParameter(const std::string& name, float value) {
    if (name == "time1") time1 = value;
    else if (name == "time2") time2 = value;
    else if (name == "time3") time3 = value;
    else if (name == "time4") time4 = value;
    else if (name == "feedback") feedback = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
    
    updateTapTimes();
}

float MultiTapDelayEffect::getParameter(const std::string& name) const {
    if (name == "time1") return time1;
    if (name == "time2") return time2;
    if (name == "time3") return time3;
    if (name == "time4") return time4;
    if (name == "feedback") return feedback;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
    return 0.0f;
}

void MultiTapDelayEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "time1") automatedTime1 = automated;
    else if (name == "time2") automatedTime2 = automated;
    else if (name == "time3") automatedTime3 = automated;
    else if (name == "time4") automatedTime4 = automated;
    else if (name == "feedback") automatedFeedback = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool MultiTapDelayEffect::isParameterAutomated(const std::string& name) const {
    if (name == "time1") return automatedTime1;
    if (name == "time2") return automatedTime2;
    if (name == "time3") return automatedTime3;
    if (name == "time4") return automatedTime4;
    if (name == "feedback") return automatedFeedback;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    return false;
}

void MultiTapDelayEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        float input = (left + right) * 0.5f;
        
        // Taps verarbeiten
        float output = 0.0f;
        for (auto& tap : taps) {
            output += processTap(tap, input);
        }
        
        // Feedback
        if (feedback > 0.0f) {
            for (auto& tap : taps) {
                tap.buffer[tap.writePos] += output * feedback;
            }
        }
        
        // Mix
        buffer[i] = left * (1.0f - mix) + output * mix;
        buffer[i + 1] = right * (1.0f - mix) + output * mix;
    }
}

void MultiTapDelayEffect::initializeTaps() {
    taps.clear();
    
    // Tap 1
    Tap tap1;
    tap1.buffer.resize(bufferSize, 0.0f);
    tap1.writePos = 0;
    tap1.readPos = 0;
    tap1.time = time1;
    tap1.level = 1.0f;
    taps.push_back(tap1);
    
    // Tap 2
    Tap tap2;
    tap2.buffer.resize(bufferSize, 0.0f);
    tap2.writePos = 0;
    tap2.readPos = 0;
    tap2.time = time2;
    tap2.level = 0.8f;
    taps.push_back(tap2);
    
    // Tap 3
    Tap tap3;
    tap3.buffer.resize(bufferSize, 0.0f);
    tap3.writePos = 0;
    tap3.readPos = 0;
    tap3.time = time3;
    tap3.level = 0.6f;
    taps.push_back(tap3);
    
    // Tap 4
    Tap tap4;
    tap4.buffer.resize(bufferSize, 0.0f);
    tap4.writePos = 0;
    tap4.readPos = 0;
    tap4.time = time4;
    tap4.level = 0.4f;
    taps.push_back(tap4);
}

void MultiTapDelayEffect::updateTaps() {
    for (auto& tap : taps) {
        tap.buffer[tap.writePos] = 0.0f;
        tap.writePos = (tap.writePos + 1) % bufferSize;
    }
}

float MultiTapDelayEffect::processTap(const Tap& tap, float input) {
    // Input in Buffer schreiben
    tap.buffer[tap.writePos] = input;
    
    // Verzögerungszeit in Samples
    unsigned long delaySamples = static_cast<unsigned long>(tap.time * 44.1f);
    
    // Leseposition berechnen
    unsigned long readPos = (tap.writePos + bufferSize - delaySamples) % bufferSize;
    
    // Interpolation
    float readPosFloat = static_cast<float>(readPos);
    unsigned long pos1 = static_cast<unsigned long>(readPosFloat);
    unsigned long pos2 = (pos1 + 1) % bufferSize;
    float frac = readPosFloat - pos1;
    
    float sample = tap.buffer[pos1] * (1.0f - frac) + tap.buffer[pos2] * frac;
    
    return sample * tap.level;
}

void MultiTapDelayEffect::updateTapTimes() {
    taps[0].time = time1;
    taps[1].time = time2;
    taps[2].time = time3;
    taps[3].time = time4;
}

void MultiTapDelayEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        time1 = 250.0f;
        time2 = 500.0f;
        time3 = 750.0f;
        time4 = 1000.0f;
        feedback = 0.3f;
        mix = 0.5f;
        quality = 0.5f;
    }
    else if (presetName == "Heavy") {
        time1 = 500.0f;
        time2 = 1000.0f;
        time3 = 1500.0f;
        time4 = 2000.0f;
        feedback = 0.5f;
        mix = 0.7f;
        quality = 0.7f;
    }
    else if (presetName == "Subtle") {
        time1 = 125.0f;
        time2 = 250.0f;
        time3 = 375.0f;
        time4 = 500.0f;
        feedback = 0.2f;
        mix = 0.3f;
        quality = 0.3f;
    }
    
    initializeTaps();
}

void MultiTapDelayEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> MultiTapDelayEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 