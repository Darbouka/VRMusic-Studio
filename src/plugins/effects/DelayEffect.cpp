#include "DelayEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

DelayEffect::DelayEffect()
    : time(0.5f)
    , feedback(0.3f)
    , mix(0.5f)
    , stereoWidth(0.5f)
    , syncRate(0.25f)
    , lowCut(100.0f)
    , highCut(10000.0f)
    , automatedTime(false)
    , automatedFeedback(false)
    , automatedMix(false)
    , automatedStereoWidth(false)
    , automatedSyncRate(false)
    , automatedLowCut(false)
    , automatedHighCut(false)
    , bufferSize(44100 * 2)  // 2 Sekunden bei 44.1kHz
    , writePos(0)
    , readPos(0)
{
    delayBuffer.resize(bufferSize * 2, 0.0f);  // Stereo-Buffer
    
    // Filter-Zustand initialisieren
    x1[0] = x1[1] = x2[0] = x2[1] = 0.0f;
    y1[0] = y1[1] = y2[0] = y2[1] = 0.0f;
    u1[0] = u1[1] = u2[0] = u2[1] = 0.0f;
    v1[0] = v1[1] = v2[0] = v2[1] = 0.0f;
    
    updateFilterCoefficients();
}

DelayEffect::~DelayEffect() {
    delayBuffer.clear();
}

bool DelayEffect::initialize() {
    return true;
}

void DelayEffect::shutdown() {
    delayBuffer.clear();
}

void DelayEffect::updateFilterCoefficients() {
    // Low-Cut Filter
    float w0 = 2.0f * M_PI * lowCut / 44100.0f;
    float alpha = std::sin(w0) / (2.0f * 0.707f);  // Q = 0.707
    
    a0 = (1.0f + alpha);
    a1 = -2.0f * std::cos(w0);
    a2 = (1.0f - alpha);
    b1 = a1;
    b2 = (1.0f - alpha) / (1.0f + alpha);
    
    // High-Cut Filter
    w0 = 2.0f * M_PI * highCut / 44100.0f;
    alpha = std::sin(w0) / (2.0f * 0.707f);  // Q = 0.707
    
    c0 = (1.0f - alpha);
    c1 = -2.0f * std::cos(w0);
    c2 = (1.0f + alpha);
    d1 = c1;
    d2 = (1.0f - alpha) / (1.0f + alpha);
    
    // Normalisieren
    float scale = 1.0f / a0;
    a0 *= scale;
    a1 *= scale;
    a2 *= scale;
    b1 *= scale;
    b2 *= scale;
    
    scale = 1.0f / c0;
    c0 *= scale;
    c1 *= scale;
    c2 *= scale;
    d1 *= scale;
    d2 *= scale;
}

std::vector<PluginParameter> DelayEffect::getParameters() const {
    return {
        {"time", "Time", PluginParameter::Type::Float, 0.0f, 2.0f, time},
        {"feedback", "Feedback", PluginParameter::Type::Float, 0.0f, 0.9f, feedback},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"stereoWidth", "Stereo Width", PluginParameter::Type::Float, 0.0f, 1.0f, stereoWidth},
        {"syncRate", "Sync Rate", PluginParameter::Type::Float, 0.0625f, 1.0f, syncRate},
        {"lowCut", "Low Cut", PluginParameter::Type::Float, 20.0f, 20000.0f, lowCut},
        {"highCut", "High Cut", PluginParameter::Type::Float, 20.0f, 20000.0f, highCut}
    };
}

void DelayEffect::setParameter(const std::string& name, float value) {
    if (name == "time") time = value;
    else if (name == "feedback") feedback = value;
    else if (name == "mix") mix = value;
    else if (name == "stereoWidth") stereoWidth = value;
    else if (name == "syncRate") syncRate = value;
    else if (name == "lowCut") {
        lowCut = value;
        updateFilterCoefficients();
    }
    else if (name == "highCut") {
        highCut = value;
        updateFilterCoefficients();
    }
}

float DelayEffect::getParameter(const std::string& name) const {
    if (name == "time") return time;
    if (name == "feedback") return feedback;
    if (name == "mix") return mix;
    if (name == "stereoWidth") return stereoWidth;
    if (name == "syncRate") return syncRate;
    if (name == "lowCut") return lowCut;
    if (name == "highCut") return highCut;
    return 0.0f;
}

void DelayEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "time") automatedTime = automated;
    else if (name == "feedback") automatedFeedback = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "stereoWidth") automatedStereoWidth = automated;
    else if (name == "syncRate") automatedSyncRate = automated;
    else if (name == "lowCut") automatedLowCut = automated;
    else if (name == "highCut") automatedHighCut = automated;
}

bool DelayEffect::isParameterAutomated(const std::string& name) const {
    if (name == "time") return automatedTime;
    if (name == "feedback") return automatedFeedback;
    if (name == "mix") return automatedMix;
    if (name == "stereoWidth") return automatedStereoWidth;
    if (name == "syncRate") return automatedSyncRate;
    if (name == "lowCut") return automatedLowCut;
    if (name == "highCut") return automatedHighCut;
    return false;
}

void DelayEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // Delay-Zeit berechnen
        float delayTime = time * 44100.0f;  // Sample-Rate: 44.1kHz
        if (syncRate > 0.0f) {
            delayTime = calculateSyncTime();
        }
        
        // Delay-Buffer lesen
        size_t readPos = (writePos - static_cast<size_t>(delayTime * 2) + bufferSize * 2) % (bufferSize * 2);
        float delayedLeft = delayBuffer[readPos];
        float delayedRight = delayBuffer[readPos + 1];
        
        // Filter anwenden
        delayedLeft = processFilter(delayedLeft, 0);
        delayedRight = processFilter(delayedRight, 1);
        
        // Feedback
        delayBuffer[writePos] = left + delayedLeft * feedback;
        delayBuffer[writePos + 1] = right + delayedRight * feedback;
        
        // Stereo-Width
        float mid = (delayedLeft + delayedRight) * 0.5f;
        float side = (delayedLeft - delayedRight) * 0.5f;
        delayedLeft = mid + side * stereoWidth;
        delayedRight = mid - side * stereoWidth;
        
        // Mix
        buffer[i] = left * (1.0f - mix) + delayedLeft * mix;
        buffer[i + 1] = right * (1.0f - mix) + delayedRight * mix;
        
        // Buffer-Position aktualisieren
        writePos = (writePos + 2) % (bufferSize * 2);
    }
}

float DelayEffect::processFilter(float input, int channel) {
    // Low-Cut Filter
    float output = a0 * input + a1 * x1[channel] + a2 * x2[channel]
                 - b1 * y1[channel] - b2 * y2[channel];
    
    x2[channel] = x1[channel];
    x1[channel] = input;
    y2[channel] = y1[channel];
    y1[channel] = output;
    
    // High-Cut Filter
    float filtered = c0 * output + c1 * u1[channel] + c2 * u2[channel]
                   - d1 * v1[channel] - d2 * v2[channel];
    
    u2[channel] = u1[channel];
    u1[channel] = output;
    v2[channel] = v1[channel];
    v1[channel] = filtered;
    
    return filtered;
}

float DelayEffect::calculateSyncTime() {
    // Sync-Zeit basierend auf BPM berechnen (angenommen 120 BPM)
    return (60.0f / 120.0f) * syncRate * 44100.0f;
}

void DelayEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        time = 0.5f;
        feedback = 0.3f;
        mix = 0.5f;
        stereoWidth = 0.5f;
        syncRate = 0.25f;
        lowCut = 100.0f;
        highCut = 10000.0f;
    }
    else if (presetName == "Long") {
        time = 1.0f;
        feedback = 0.5f;
        mix = 0.7f;
        stereoWidth = 0.7f;
        syncRate = 0.5f;
        lowCut = 200.0f;
        highCut = 8000.0f;
    }
    else if (presetName == "Short") {
        time = 0.25f;
        feedback = 0.2f;
        mix = 0.3f;
        stereoWidth = 0.3f;
        syncRate = 0.125f;
        lowCut = 50.0f;
        highCut = 15000.0f;
    }
    
    updateFilterCoefficients();
}

void DelayEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> DelayEffect::getAvailablePresets() const {
    return {"Standard", "Long", "Short"};
}

} // namespace VR_DAW 