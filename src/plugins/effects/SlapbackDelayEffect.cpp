#include "SlapbackDelayEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

SlapbackDelayEffect::SlapbackDelayEffect()
    : time(50.0f)
    , feedback(0.2f)
    , mix(0.5f)
    , tone(0.5f)
    , stereoWidth(0.5f)
    , quality(0.5f)
    , automatedTime(false)
    , automatedFeedback(false)
    , automatedMix(false)
    , automatedTone(false)
    , automatedStereoWidth(false)
    , automatedQuality(false)
    , bufferSize(44100) // 1 Sekunde Buffer
{
    initializeDelayLines();
}

SlapbackDelayEffect::~SlapbackDelayEffect() {
}

bool SlapbackDelayEffect::initialize() {
    initializeDelayLines();
    return true;
}

void SlapbackDelayEffect::shutdown() {
    leftDelay.buffer.clear();
    rightDelay.buffer.clear();
}

std::vector<PluginParameter> SlapbackDelayEffect::getParameters() const {
    return {
        {"time", "Time", PluginParameter::Type::Float, 0.0f, 200.0f, time},
        {"feedback", "Feedback", PluginParameter::Type::Float, 0.0f, 0.9f, feedback},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"tone", "Tone", PluginParameter::Type::Float, 0.0f, 1.0f, tone},
        {"stereoWidth", "Stereo Width", PluginParameter::Type::Float, 0.0f, 1.0f, stereoWidth},
        {"quality", "Quality", PluginParameter::Type::Float, 0.0f, 1.0f, quality}
    };
}

void SlapbackDelayEffect::setParameter(const std::string& name, float value) {
    if (name == "time") {
        time = value;
        updateDelayTimes();
    }
    else if (name == "feedback") feedback = value;
    else if (name == "mix") mix = value;
    else if (name == "tone") {
        tone = value;
        updateFilterCoefficients();
    }
    else if (name == "stereoWidth") stereoWidth = value;
    else if (name == "quality") quality = value;
}

float SlapbackDelayEffect::getParameter(const std::string& name) const {
    if (name == "time") return time;
    if (name == "feedback") return feedback;
    if (name == "mix") return mix;
    if (name == "tone") return tone;
    if (name == "stereoWidth") return stereoWidth;
    if (name == "quality") return quality;
    return 0.0f;
}

void SlapbackDelayEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "time") automatedTime = automated;
    else if (name == "feedback") automatedFeedback = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "tone") automatedTone = automated;
    else if (name == "stereoWidth") automatedStereoWidth = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool SlapbackDelayEffect::isParameterAutomated(const std::string& name) const {
    if (name == "time") return automatedTime;
    if (name == "feedback") return automatedFeedback;
    if (name == "mix") return automatedMix;
    if (name == "tone") return automatedTone;
    if (name == "stereoWidth") return automatedStereoWidth;
    if (name == "quality") return automatedQuality;
    return false;
}

void SlapbackDelayEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // Delay-Lines verarbeiten
        float leftDelayOutput = processDelayLine(leftDelay, left);
        float rightDelayOutput = processDelayLine(rightDelay, right);
        
        // Feedback
        if (feedback > 0.0f) {
            leftDelay.buffer[leftDelay.writePos] += leftDelayOutput * feedback;
            rightDelay.buffer[rightDelay.writePos] += rightDelayOutput * feedback;
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

void SlapbackDelayEffect::initializeDelayLines() {
    // Linke Delay-Line
    leftDelay.buffer.resize(bufferSize, 0.0f);
    leftDelay.writePos = 0;
    leftDelay.readPos = 0;
    leftDelay.time = time;
    leftDelay.level = 1.0f;
    leftDelay.lowpass = 0.0f;
    leftDelay.highpass = 0.0f;
    
    // Rechte Delay-Line
    rightDelay.buffer.resize(bufferSize, 0.0f);
    rightDelay.writePos = 0;
    rightDelay.readPos = 0;
    rightDelay.time = time;
    rightDelay.level = 1.0f;
    rightDelay.lowpass = 0.0f;
    rightDelay.highpass = 0.0f;
    
    updateFilterCoefficients();
}

void SlapbackDelayEffect::updateDelayLines() {
    leftDelay.buffer[leftDelay.writePos] = 0.0f;
    rightDelay.buffer[rightDelay.writePos] = 0.0f;
    
    leftDelay.writePos = (leftDelay.writePos + 1) % bufferSize;
    rightDelay.writePos = (rightDelay.writePos + 1) % bufferSize;
}

float SlapbackDelayEffect::processDelayLine(DelayLine& delay, float input) {
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
    
    // Filter anwenden
    sample = sample * (1.0f - delay.lowpass - delay.highpass) +
             sample * delay.lowpass * (1.0f - tone) +
             sample * delay.highpass * tone;
    
    return sample * delay.level;
}

void SlapbackDelayEffect::updateDelayTimes() {
    leftDelay.time = time;
    rightDelay.time = time;
}

void SlapbackDelayEffect::updateFilterCoefficients() {
    // Filter-Koeffizienten basierend auf Tone-Parameter
    float cutoff = 1000.0f + tone * 10000.0f;
    float q = 0.707f;
    
    float w0 = 2.0f * M_PI * cutoff / 44100.0f;
    float alpha = std::sin(w0) / (2.0f * q);
    
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * std::cos(w0);
    float a2 = 1.0f - alpha;
    float b0 = (1.0f - std::cos(w0)) / 2.0f;
    float b1 = 1.0f - std::cos(w0);
    float b2 = (1.0f - std::cos(w0)) / 2.0f;
    
    leftDelay.lowpass = b0 / a0;
    leftDelay.highpass = b2 / a0;
    rightDelay.lowpass = b0 / a0;
    rightDelay.highpass = b2 / a0;
}

void SlapbackDelayEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        time = 50.0f;
        feedback = 0.2f;
        mix = 0.5f;
        tone = 0.5f;
        stereoWidth = 0.5f;
        quality = 0.5f;
    }
    else if (presetName == "Heavy") {
        time = 100.0f;
        feedback = 0.4f;
        mix = 0.7f;
        tone = 0.7f;
        stereoWidth = 0.8f;
        quality = 0.7f;
    }
    else if (presetName == "Subtle") {
        time = 25.0f;
        feedback = 0.1f;
        mix = 0.3f;
        tone = 0.3f;
        stereoWidth = 0.3f;
        quality = 0.3f;
    }
    
    initializeDelayLines();
}

void SlapbackDelayEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> SlapbackDelayEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 