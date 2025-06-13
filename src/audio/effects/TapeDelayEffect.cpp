#include "TapeDelayEffect.hpp"
#include <cmath>

namespace VRMusicStudio {

TapeDelayEffect::TapeDelayEffect() :
    time(500.0f),
    feedback(0.5f),
    mix(0.5f),
    wow(0.1f),
    flutter(0.1f),
    saturation(0.5f),
    quality(1.0f),
    automatedTime(false),
    automatedFeedback(false),
    automatedMix(false),
    automatedWow(false),
    automatedFlutter(false),
    automatedSaturation(false),
    automatedQuality(false),
    bufferSize(0),
    writePos(0),
    readPos(0),
    wowPhase(0.0f),
    flutterPhase(0.0f),
    lastDelayTime(500.0f),
    currentTime(500.0f),
    currentFeedback(0.5f),
    currentMix(0.5f),
    currentWow(0.1f),
    currentFlutter(0.1f),
    currentSaturation(0.5f),
    currentQuality(1.0f)
{
}

TapeDelayEffect::~TapeDelayEffect() {
    shutdown();
}

bool TapeDelayEffect::initialize() {
    const float sampleRate = 44100.0f;
    bufferSize = static_cast<unsigned long>(sampleRate * 2.0f); // 2 Sekunden Buffer
    initializeBuffer();
    return true;
}

void TapeDelayEffect::shutdown() {
    buffer.clear();
}

std::vector<PluginParameter> TapeDelayEffect::getParameters() const {
    return {
        {"time", 500.0f, 0.0f, 2000.0f},
        {"feedback", 0.5f, 0.0f, 0.9f},
        {"mix", 0.5f, 0.0f, 1.0f},
        {"wow", 0.1f, 0.0f, 1.0f},
        {"flutter", 0.1f, 0.0f, 1.0f},
        {"saturation", 0.5f, 0.0f, 1.0f},
        {"quality", 1.0f, 0.0f, 1.0f}
    };
}

void TapeDelayEffect::setParameter(const std::string& name, float value) {
    if (name == "time") time = value;
    else if (name == "feedback") feedback = value;
    else if (name == "mix") mix = value;
    else if (name == "wow") wow = value;
    else if (name == "flutter") flutter = value;
    else if (name == "saturation") saturation = value;
    else if (name == "quality") quality = value;
}

float TapeDelayEffect::getParameter(const std::string& name) const {
    if (name == "time") return time;
    else if (name == "feedback") return feedback;
    else if (name == "mix") return mix;
    else if (name == "wow") return wow;
    else if (name == "flutter") return flutter;
    else if (name == "saturation") return saturation;
    else if (name == "quality") return quality;
    return 0.0f;
}

void TapeDelayEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "time") automatedTime = automated;
    else if (name == "feedback") automatedFeedback = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "wow") automatedWow = automated;
    else if (name == "flutter") automatedFlutter = automated;
    else if (name == "saturation") automatedSaturation = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool TapeDelayEffect::isParameterAutomated(const std::string& name) const {
    if (name == "time") return automatedTime;
    else if (name == "feedback") return automatedFeedback;
    else if (name == "mix") return automatedMix;
    else if (name == "wow") return automatedWow;
    else if (name == "flutter") return automatedFlutter;
    else if (name == "saturation") return automatedSaturation;
    else if (name == "quality") return automatedQuality;
    return false;
}

void TapeDelayEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateParameters();
    processDelay(buffer, framesPerBuffer);
    applyWow(buffer, framesPerBuffer);
    applyFlutter(buffer, framesPerBuffer);
    applySaturation(buffer, framesPerBuffer);
    applyQuality(buffer, framesPerBuffer);
}

void TapeDelayEffect::loadPreset(const std::string& presetName) {
    // TODO: Implement preset loading
}

void TapeDelayEffect::savePreset(const std::string& presetName) {
    // TODO: Implement preset saving
}

std::vector<std::string> TapeDelayEffect::getAvailablePresets() const {
    return {}; // TODO: Implement preset list
}

void TapeDelayEffect::updateParameters() {
    currentTime = time;
    currentFeedback = feedback;
    currentMix = mix;
    currentWow = wow;
    currentFlutter = flutter;
    currentSaturation = saturation;
    currentQuality = quality;
}

void TapeDelayEffect::initializeBuffer() {
    buffer.resize(bufferSize, 0.0f);
    writePos = 0;
    readPos = 0;
}

void TapeDelayEffect::processDelay(float* buffer, unsigned long framesPerBuffer) {
    const float sampleRate = 44100.0f;
    const float delaySamples = currentTime * sampleRate / 1000.0f;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        // Write to buffer
        buffer[writePos] = buffer[i];
        writePos = (writePos + 1) % bufferSize;
        
        // Read from buffer
        float delayedSample = buffer[readPos];
        
        // Apply feedback
        buffer[i] = buffer[i] * (1.0f - currentMix) + delayedSample * currentMix;
        buffer[writePos] = buffer[i] * currentFeedback;
        
        // Update read position
        readPos = (readPos + 1) % bufferSize;
    }
}

void TapeDelayEffect::applyWow(float* buffer, unsigned long framesPerBuffer) {
    if (currentWow <= 0.0f) return;
    
    const float sampleRate = 44100.0f;
    const float wowRate = 0.5f; // 0.5 Hz
    const float wowAmount = currentWow * 0.1f; // Max 10% pitch variation
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float modulation = calculateModulation(wowPhase, wowAmount);
        buffer[i] *= (1.0f + modulation);
        
        wowPhase += wowRate / sampleRate;
        if (wowPhase >= 1.0f) {
            wowPhase -= 1.0f;
        }
    }
}

void TapeDelayEffect::applyFlutter(float* buffer, unsigned long framesPerBuffer) {
    if (currentFlutter <= 0.0f) return;
    
    const float sampleRate = 44100.0f;
    const float flutterRate = 10.0f; // 10 Hz
    const float flutterAmount = currentFlutter * 0.05f; // Max 5% pitch variation
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float modulation = calculateModulation(flutterPhase, flutterAmount);
        buffer[i] *= (1.0f + modulation);
        
        flutterPhase += flutterRate / sampleRate;
        if (flutterPhase >= 1.0f) {
            flutterPhase -= 1.0f;
        }
    }
}

void TapeDelayEffect::applySaturation(float* buffer, unsigned long framesPerBuffer) {
    if (currentSaturation <= 0.0f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float sample = buffer[i];
        float saturated = std::tanh(sample * (1.0f + currentSaturation));
        buffer[i] = sample * (1.0f - currentSaturation) + saturated * currentSaturation;
    }
}

void TapeDelayEffect::applyQuality(float* buffer, unsigned long framesPerBuffer) {
    if (currentQuality >= 1.0f) return;
    
    // Simple low-pass filter based on quality
    float alpha = currentQuality;
    float prev = buffer[0];
    
    for (unsigned long i = 1; i < framesPerBuffer; ++i) {
        float current = buffer[i];
        buffer[i] = prev + alpha * (current - prev);
        prev = buffer[i];
    }
}

float TapeDelayEffect::calculateModulation(float phase, float amount) {
    return std::sin(phase * 2.0f * M_PI) * amount;
}

} // namespace VRMusicStudio 