#include "LooperEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

LooperEffect::LooperEffect()
    : loopLength(4.0f)
    , feedback(0.0f)
    , mix(0.5f)
    , speed(1.0f)
    , reverse(0.0f)
    , quality(0.5f)
    , automatedLoopLength(false)
    , automatedFeedback(false)
    , automatedMix(false)
    , automatedSpeed(false)
    , automatedReverse(false)
    , automatedQuality(false)
    , bufferSize(44100 * 60) // 60 Sekunden Buffer
    , writePos(0)
    , readPos(0)
    , isRecording(false)
    , isPlaying(false)
    , currentSpeed(1.0f)
    , currentReverse(0.0f)
{
    initializeBuffer();
}

LooperEffect::~LooperEffect() {
}

bool LooperEffect::initialize() {
    initializeBuffer();
    return true;
}

void LooperEffect::shutdown() {
    buffer.clear();
}

std::vector<PluginParameter> LooperEffect::getParameters() const {
    return {
        {"loopLength", "Loop Length", PluginParameter::Type::Float, 0.1f, 60.0f, loopLength},
        {"feedback", "Feedback", PluginParameter::Type::Float, 0.0f, 0.9f, feedback},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"speed", "Speed", PluginParameter::Type::Float, 0.25f, 4.0f, speed},
        {"reverse", "Reverse", PluginParameter::Type::Float, 0.0f, 1.0f, reverse},
        {"quality", "Quality", PluginParameter::Type::Float, 0.0f, 1.0f, quality}
    };
}

void LooperEffect::setParameter(const std::string& name, float value) {
    if (name == "loopLength") {
        loopLength = value;
        if (isRecording) {
            stopRecording();
            startRecording();
        }
    }
    else if (name == "feedback") feedback = value;
    else if (name == "mix") mix = value;
    else if (name == "speed") {
        speed = value;
        currentSpeed = speed;
    }
    else if (name == "reverse") {
        reverse = value;
        currentReverse = reverse;
    }
    else if (name == "quality") quality = value;
}

float LooperEffect::getParameter(const std::string& name) const {
    if (name == "loopLength") return loopLength;
    if (name == "feedback") return feedback;
    if (name == "mix") return mix;
    if (name == "speed") return speed;
    if (name == "reverse") return reverse;
    if (name == "quality") return quality;
    return 0.0f;
}

void LooperEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "loopLength") automatedLoopLength = automated;
    else if (name == "feedback") automatedFeedback = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "speed") automatedSpeed = automated;
    else if (name == "reverse") automatedReverse = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool LooperEffect::isParameterAutomated(const std::string& name) const {
    if (name == "loopLength") return automatedLoopLength;
    if (name == "feedback") return automatedFeedback;
    if (name == "mix") return automatedMix;
    if (name == "speed") return automatedSpeed;
    if (name == "reverse") return automatedReverse;
    if (name == "quality") return automatedQuality;
    return false;
}

void LooperEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        float input = (left + right) * 0.5f;
        
        // Recording
        if (isRecording) {
            this->buffer[writePos] = input;
            writePos = (writePos + 1) % bufferSize;
            
            // Loop-Länge erreicht?
            if (writePos >= static_cast<unsigned long>(loopLength * 44100)) {
                stopRecording();
                startPlayback();
            }
        }
        
        // Playback
        float output = 0.0f;
        if (isPlaying) {
            output = processSample(input);
        }
        
        // Mix
        buffer[i] = left * (1.0f - mix) + output * mix;
        buffer[i + 1] = right * (1.0f - mix) + output * mix;
    }
}

void LooperEffect::initializeBuffer() {
    buffer.resize(bufferSize, 0.0f);
    writePos = 0;
    readPos = 0;
    isRecording = false;
    isPlaying = false;
}

void LooperEffect::updateBuffer() {
    // Buffer aktualisieren (z.B. für Feedback)
    if (isPlaying && feedback > 0.0f) {
        for (unsigned long i = 0; i < bufferSize; ++i) {
            buffer[i] *= feedback;
        }
    }
}

float LooperEffect::processSample(float input) {
    // Leseposition aktualisieren
    float readPosFloat = static_cast<float>(readPos);
    readPosFloat += currentSpeed * (1.0f - 2.0f * currentReverse);
    
    // Loop-Bereich
    unsigned long loopSize = static_cast<unsigned long>(loopLength * 44100);
    
    // Position im Loop
    while (readPosFloat >= loopSize) readPosFloat -= loopSize;
    while (readPosFloat < 0) readPosFloat += loopSize;
    
    // Interpolation
    unsigned long pos1 = static_cast<unsigned long>(readPosFloat);
    unsigned long pos2 = (pos1 + 1) % loopSize;
    float frac = readPosFloat - pos1;
    
    float sample = buffer[pos1] * (1.0f - frac) + buffer[pos2] * frac;
    
    // Leseposition aktualisieren
    readPos = static_cast<unsigned long>(readPosFloat);
    
    return sample;
}

void LooperEffect::startRecording() {
    isRecording = true;
    isPlaying = false;
    writePos = 0;
    readPos = 0;
}

void LooperEffect::stopRecording() {
    isRecording = false;
}

void LooperEffect::startPlayback() {
    isPlaying = true;
    readPos = 0;
}

void LooperEffect::stopPlayback() {
    isPlaying = false;
}

void LooperEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        loopLength = 4.0f;
        feedback = 0.0f;
        mix = 0.5f;
        speed = 1.0f;
        reverse = 0.0f;
        quality = 0.5f;
    }
    else if (presetName == "Heavy") {
        loopLength = 2.0f;
        feedback = 0.3f;
        mix = 0.7f;
        speed = 0.5f;
        reverse = 0.5f;
        quality = 0.7f;
    }
    else if (presetName == "Subtle") {
        loopLength = 8.0f;
        feedback = 0.0f;
        mix = 0.3f;
        speed = 1.0f;
        reverse = 0.0f;
        quality = 0.3f;
    }
    
    initializeBuffer();
}

void LooperEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> LooperEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 