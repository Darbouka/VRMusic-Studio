#include "LooperEffect.hpp"
#include <cmath>

namespace VRMusicStudio {

LooperEffect::LooperEffect() :
    mix(0.5f),
    feedback(0.5f),
    reverse(0.0f),
    speed(1.0f),
    quality(1.0f),
    automatedMix(false),
    automatedFeedback(false),
    automatedReverse(false),
    automatedSpeed(false),
    automatedQuality(false),
    bufferSize(0),
    writePos(0),
    readPos(0),
    recording(false),
    playing(false),
    loopLength(4.0f),
    currentSpeed(1.0f),
    currentReverse(0.0f),
    currentMix(0.5f),
    currentFeedback(0.5f),
    currentQuality(1.0f)
{
}

LooperEffect::~LooperEffect() {
    shutdown();
}

bool LooperEffect::initialize() {
    const float sampleRate = 44100.0f;
    resizeBuffer(loopLength);
    return true;
}

void LooperEffect::shutdown() {
    loopBuffer.clear();
}

std::vector<PluginParameter> LooperEffect::getParameters() const {
    return {
        {"mix", 0.5f, 0.0f, 1.0f},
        {"feedback", 0.5f, 0.0f, 0.9f},
        {"reverse", 0.0f, 0.0f, 1.0f},
        {"speed", 1.0f, 0.25f, 4.0f},
        {"quality", 1.0f, 0.0f, 1.0f}
    };
}

void LooperEffect::setParameter(const std::string& name, float value) {
    if (name == "mix") mix = value;
    else if (name == "feedback") feedback = value;
    else if (name == "reverse") reverse = value;
    else if (name == "speed") speed = value;
    else if (name == "quality") quality = value;
}

float LooperEffect::getParameter(const std::string& name) const {
    if (name == "mix") return mix;
    else if (name == "feedback") return feedback;
    else if (name == "reverse") return reverse;
    else if (name == "speed") return speed;
    else if (name == "quality") return quality;
    return 0.0f;
}

void LooperEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "mix") automatedMix = automated;
    else if (name == "feedback") automatedFeedback = automated;
    else if (name == "reverse") automatedReverse = automated;
    else if (name == "speed") automatedSpeed = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool LooperEffect::isParameterAutomated(const std::string& name) const {
    if (name == "mix") return automatedMix;
    else if (name == "feedback") return automatedFeedback;
    else if (name == "reverse") return automatedReverse;
    else if (name == "speed") return automatedSpeed;
    else if (name == "quality") return automatedQuality;
    return false;
}

void LooperEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateParameters();
    
    if (recording) {
        for (unsigned long i = 0; i < framesPerBuffer; ++i) {
            loopBuffer[writePos] = buffer[i];
            writePos = (writePos + 1) % bufferSize;
        }
    }
    
    if (playing) {
        processLoop(buffer, framesPerBuffer);
    }
    
    applySpeed(buffer, framesPerBuffer);
    applyReverse(buffer, framesPerBuffer);
    applyMix(buffer, framesPerBuffer);
    applyFeedback(buffer, framesPerBuffer);
    applyQuality(buffer, framesPerBuffer);
}

void LooperEffect::loadPreset(const std::string& presetName) {
    // TODO: Implement preset loading
}

void LooperEffect::savePreset(const std::string& presetName) {
    // TODO: Implement preset saving
}

std::vector<std::string> LooperEffect::getAvailablePresets() const {
    return {}; // TODO: Implement preset list
}

void LooperEffect::startRecording() {
    recording = true;
    playing = false;
    writePos = 0;
}

void LooperEffect::stopRecording() {
    recording = false;
    playing = true;
    readPos = 0;
}

void LooperEffect::startPlayback() {
    playing = true;
}

void LooperEffect::stopPlayback() {
    playing = false;
}

void LooperEffect::clearLoop() {
    loopBuffer.clear();
    loopBuffer.resize(bufferSize, 0.0f);
    recording = false;
    playing = false;
    writePos = 0;
    readPos = 0;
}

void LooperEffect::setLoopLength(float seconds) {
    loopLength = seconds;
    resizeBuffer(seconds);
}

float LooperEffect::getLoopLength() const {
    return loopLength;
}

bool LooperEffect::isRecording() const {
    return recording;
}

bool LooperEffect::isPlaying() const {
    return playing;
}

void LooperEffect::updateParameters() {
    currentSpeed = speed;
    currentReverse = reverse;
    currentMix = mix;
    currentFeedback = feedback;
    currentQuality = quality;
}

void LooperEffect::processLoop(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float loopSample = loopBuffer[readPos];
        buffer[i] = buffer[i] * (1.0f - currentMix) + loopSample * currentMix;
        loopBuffer[writePos] = buffer[i] * currentFeedback;
        
        writePos = (writePos + 1) % bufferSize;
        readPos = (readPos + 1) % bufferSize;
    }
}

void LooperEffect::applySpeed(float* buffer, unsigned long framesPerBuffer) {
    if (currentSpeed == 1.0f) return;
    
    std::vector<float> tempBuffer(buffer, buffer + framesPerBuffer);
    float position = 0.0f;
    float step = currentSpeed;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = interpolateSample(tempBuffer, position);
        position += step;
    }
}

void LooperEffect::applyReverse(float* buffer, unsigned long framesPerBuffer) {
    if (currentReverse <= 0.0f) return;
    
    std::vector<float> tempBuffer(buffer, buffer + framesPerBuffer);
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float forward = tempBuffer[i];
        float backward = tempBuffer[framesPerBuffer - 1 - i];
        buffer[i] = forward * (1.0f - currentReverse) + backward * currentReverse;
    }
}

void LooperEffect::applyMix(float* buffer, unsigned long framesPerBuffer) {
    if (currentMix == 0.5f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] *= currentMix;
    }
}

void LooperEffect::applyFeedback(float* buffer, unsigned long framesPerBuffer) {
    if (currentFeedback <= 0.0f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] *= (1.0f + currentFeedback);
    }
}

void LooperEffect::applyQuality(float* buffer, unsigned long framesPerBuffer) {
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

void LooperEffect::initializeBuffer() {
    loopBuffer.resize(bufferSize, 0.0f);
    writePos = 0;
    readPos = 0;
}

void LooperEffect::resizeBuffer(float seconds) {
    const float sampleRate = 44100.0f;
    bufferSize = static_cast<unsigned long>(seconds * sampleRate);
    loopBuffer.resize(bufferSize, 0.0f);
    writePos = 0;
    readPos = 0;
}

float LooperEffect::interpolateSample(const std::deque<float>& buffer, float position) {
    unsigned long index = static_cast<unsigned long>(position);
    float fraction = position - index;
    
    if (index >= buffer.size() - 1) {
        return buffer.back();
    }
    
    float sample1 = buffer[index];
    float sample2 = buffer[index + 1];
    return sample1 + fraction * (sample2 - sample1);
}

} // namespace VRMusicStudio 