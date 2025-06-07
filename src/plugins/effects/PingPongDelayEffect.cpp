#include "PingPongDelayEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

PingPongDelayEffect::PingPongDelayEffect()
    : time(500.0f)
    , feedback(0.3f)
    , mix(0.5f)
    , crossfeed(0.5f)
    , stereoWidth(0.5f)
    , quality(0.5f)
    , automatedTime(false)
    , automatedFeedback(false)
    , automatedMix(false)
    , automatedCrossfeed(false)
    , automatedStereoWidth(false)
    , automatedQuality(false)
    , bufferSize(44100 * 2) // 2 Sekunden Buffer
{
    initializeDelayLines();
}

PingPongDelayEffect::~PingPongDelayEffect() {
}

bool PingPongDelayEffect::initialize() {
    initializeDelayLines();
    return true;
}

void PingPongDelayEffect::shutdown() {
    leftDelay.buffer.clear();
    rightDelay.buffer.clear();
}

std::vector<PluginParameter> PingPongDelayEffect::getParameters() const {
    return {
        {"time", "Time", PluginParameter::Type::Float, 0.0f, 2000.0f, time},
        {"feedback", "Feedback", PluginParameter::Type::Float, 0.0f, 0.9f, feedback},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"crossfeed", "Crossfeed", PluginParameter::Type::Float, 0.0f, 1.0f, crossfeed},
        {"stereoWidth", "Stereo Width", PluginParameter::Type::Float, 0.0f, 1.0f, stereoWidth},
        {"quality", "Quality", PluginParameter::Type::Float, 0.0f, 1.0f, quality}
    };
}

void PingPongDelayEffect::setParameter(const std::string& name, float value) {
    if (name == "time") {
        time = value;
        updateDelayTimes();
    }
    else if (name == "feedback") feedback = value;
    else if (name == "mix") mix = value;
    else if (name == "crossfeed") crossfeed = value;
    else if (name == "stereoWidth") stereoWidth = value;
    else if (name == "quality") quality = value;
}

float PingPongDelayEffect::getParameter(const std::string& name) const {
    if (name == "time") return time;
    if (name == "feedback") return feedback;
    if (name == "mix") return mix;
    if (name == "crossfeed") return crossfeed;
    if (name == "stereoWidth") return stereoWidth;
    if (name == "quality") return quality;
    return 0.0f;
}

void PingPongDelayEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "time") automatedTime = automated;
    else if (name == "feedback") automatedFeedback = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "crossfeed") automatedCrossfeed = automated;
    else if (name == "stereoWidth") automatedStereoWidth = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool PingPongDelayEffect::isParameterAutomated(const std::string& name) const {
    if (name == "time") return automatedTime;
    if (name == "feedback") return automatedFeedback;
    if (name == "mix") return automatedMix;
    if (name == "crossfeed") return automatedCrossfeed;
    if (name == "stereoWidth") return automatedStereoWidth;
    if (name == "quality") return automatedQuality;
    return false;
}

void PingPongDelayEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // Delay-Lines verarbeiten
        float leftDelayOutput = processDelayLine(leftDelay, left);
        float rightDelayOutput = processDelayLine(rightDelay, right);
        
        // Crossfeed
        float leftCrossfeed = rightDelayOutput * crossfeed;
        float rightCrossfeed = leftDelayOutput * crossfeed;
        
        // Feedback
        if (feedback > 0.0f) {
            leftDelay.buffer[leftDelay.writePos] += rightCrossfeed * feedback;
            rightDelay.buffer[rightDelay.writePos] += leftCrossfeed * feedback;
        }
        
        // Stereo-Width
        float mid = (leftDelayOutput + rightDelayOutput) * 0.5f;
        float side = (leftDelayOutput - rightDelayOutput) * 0.5f;
        
        leftDelayOutput = mid + side * stereoWidth;
        rightDelayOutput = mid - side * stereoWidth;
        
        // Mix
        buffer[i] = left * (1.0f - mix) + leftDelayOutput * mix;
        buffer[i + 1] = right * (1.0f - mix) + rightDelayOutput * mix;
    }
}

void PingPongDelayEffect::initializeDelayLines() {
    // Linke Delay-Line
    leftDelay.buffer.resize(bufferSize, 0.0f);
    leftDelay.writePos = 0;
    leftDelay.readPos = 0;
    leftDelay.time = time;
    leftDelay.level = 1.0f;
    
    // Rechte Delay-Line
    rightDelay.buffer.resize(bufferSize, 0.0f);
    rightDelay.writePos = 0;
    rightDelay.readPos = 0;
    rightDelay.time = time;
    rightDelay.level = 1.0f;
}

void PingPongDelayEffect::updateDelayLines() {
    leftDelay.buffer[leftDelay.writePos] = 0.0f;
    rightDelay.buffer[rightDelay.writePos] = 0.0f;
    
    leftDelay.writePos = (leftDelay.writePos + 1) % bufferSize;
    rightDelay.writePos = (rightDelay.writePos + 1) % bufferSize;
}

float PingPongDelayEffect::processDelayLine(DelayLine& delay, float input) {
    // Input in Buffer schreiben
    delay.buffer[delay.writePos] = input;
    
    // Verzögerungszeit in Samples
    unsigned long delaySamples = static_cast<unsigned long>(delay.time * 44.1f);
    
    // Leseposition berechnen
    delay.readPos = (delay.writePos + bufferSize - delaySamples) % bufferSize;
    
    // Interpolation
    float readPosFloat = static_cast<float>(delay.readPos);
    unsigned long pos1 = static_cast<unsigned long>(readPosFloat);
    unsigned long pos2 = (pos1 + 1) % bufferSize;
    float frac = readPosFloat - pos1;
    
    float sample = delay.buffer[pos1] * (1.0f - frac) + delay.buffer[pos2] * frac;
    
    return sample * delay.level;
}

void PingPongDelayEffect::updateDelayTimes() {
    leftDelay.time = time;
    rightDelay.time = time;
}

void PingPongDelayEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        time = 500.0f;
        feedback = 0.3f;
        mix = 0.5f;
        crossfeed = 0.5f;
        stereoWidth = 0.5f;
        quality = 0.5f;
    }
    else if (presetName == "Heavy") {
        time = 1000.0f;
        feedback = 0.5f;
        mix = 0.7f;
        crossfeed = 0.7f;
        stereoWidth = 0.8f;
        quality = 0.7f;
    }
    else if (presetName == "Subtle") {
        time = 250.0f;
        feedback = 0.2f;
        mix = 0.3f;
        crossfeed = 0.3f;
        stereoWidth = 0.3f;
        quality = 0.3f;
    }
    
    initializeDelayLines();
}

void PingPongDelayEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> PingPongDelayEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 