#include "ReverseDelayEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

ReverseDelayEffect::ReverseDelayEffect()
    : time(500.0f)
    , feedback(0.3f)
    , mix(0.5f)
    , reverse(0.5f)
    , stereoWidth(0.5f)
    , quality(1.0f)
    , automatedTime(false)
    , automatedFeedback(false)
    , automatedMix(false)
    , automatedReverse(false)
    , automatedStereoWidth(false)
    , automatedQuality(false)
    , bufferSize(0)
{
    initializeDelayLines();
}

ReverseDelayEffect::~ReverseDelayEffect() {
    shutdown();
}

bool ReverseDelayEffect::initialize() {
    initializeDelayLines();
    return true;
}

void ReverseDelayEffect::shutdown() {
    leftDelay.buffer.clear();
    rightDelay.buffer.clear();
}

void ReverseDelayEffect::initializeDelayLines() {
    // Berechne Puffergröße basierend auf der maximalen Verzögerungszeit
    bufferSize = static_cast<unsigned long>(2000.0f * 44.1f); // 2000ms bei 44.1kHz
    
    // Initialisiere Verzögerungslinien
    leftDelay.buffer.resize(bufferSize, 0.0f);
    rightDelay.buffer.resize(bufferSize, 0.0f);
    leftDelay.writePos = 0;
    rightDelay.writePos = 0;
    leftDelay.readPos = 0;
    rightDelay.readPos = 0;
    leftDelay.time = time;
    rightDelay.time = time;
    leftDelay.level = 1.0f;
    rightDelay.level = 1.0f;
    
    // Initialisiere Lesepositionen
    updateDelayTimes();
}

void ReverseDelayEffect::updateDelayLines() {
    // Aktualisiere Verzögerungszeiten
    updateDelayTimes();
}

void ReverseDelayEffect::updateDelayTimes() {
    // Berechne neue Lesepositionen
    unsigned long delaySamples = static_cast<unsigned long>(time * 44.1f);
    
    leftDelay.readPos = (leftDelay.writePos + bufferSize - delaySamples) % bufferSize;
    rightDelay.readPos = (rightDelay.writePos + bufferSize - delaySamples) % bufferSize;
}

float ReverseDelayEffect::processDelayLine(DelayLine& delay, float input) {
    // Schreibe in Puffer
    delay.buffer[delay.writePos] = input;
    delay.writePos = (delay.writePos + 1) % bufferSize;
    
    // Lese aus Puffer
    float output = delay.buffer[delay.readPos];
    delay.readPos = (delay.readPos + 1) % bufferSize;
    
    // Wende Reverse-Effekt an
    if (reverse > 0.0f) {
        // Berechne Reverse-Position
        unsigned long reversePos = (delay.writePos + bufferSize - 1) % bufferSize;
        float reverseSample = delay.buffer[reversePos];
        
        // Mix zwischen normalem und reversed Signal
        output = output * (1.0f - reverse) + reverseSample * reverse;
    }
    
    return output;
}

void ReverseDelayEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer * 2; i += 2) {
        float leftInput = buffer[i];
        float rightInput = buffer[i + 1];
        
        // Verarbeite Verzögerung
        float leftDelay = processDelayLine(this->leftDelay, leftInput);
        float rightDelay = processDelayLine(this->rightDelay, rightInput);
        
        // Feedback
        leftDelay *= feedback;
        rightDelay *= feedback;
        
        // Stereo-Width
        float mid = (leftDelay + rightDelay) * 0.5f;
        float side = (leftDelay - rightDelay) * 0.5f;
        leftDelay = mid + side * stereoWidth;
        rightDelay = mid - side * stereoWidth;
        
        // Mix
        buffer[i] = leftInput + leftDelay * mix;
        buffer[i + 1] = rightInput + rightDelay * mix;
    }
}

std::vector<PluginParameter> ReverseDelayEffect::getParameters() const {
    return {
        {"time", time, 0.0f, 2000.0f, "ms"},
        {"feedback", feedback, 0.0f, 0.9f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"reverse", reverse, 0.0f, 1.0f, ""},
        {"stereoWidth", stereoWidth, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void ReverseDelayEffect::setParameter(const std::string& name, float value) {
    if (name == "time") {
        time = value;
        updateDelayTimes();
    }
    else if (name == "feedback") feedback = value;
    else if (name == "mix") mix = value;
    else if (name == "reverse") reverse = value;
    else if (name == "stereoWidth") stereoWidth = value;
    else if (name == "quality") quality = value;
}

float ReverseDelayEffect::getParameter(const std::string& name) const {
    if (name == "time") return time;
    if (name == "feedback") return feedback;
    if (name == "mix") return mix;
    if (name == "reverse") return reverse;
    if (name == "stereoWidth") return stereoWidth;
    if (name == "quality") return quality;
    return 0.0f;
}

void ReverseDelayEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "time") automatedTime = automated;
    else if (name == "feedback") automatedFeedback = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "reverse") automatedReverse = automated;
    else if (name == "stereoWidth") automatedStereoWidth = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool ReverseDelayEffect::isParameterAutomated(const std::string& name) const {
    if (name == "time") return automatedTime;
    if (name == "feedback") return automatedFeedback;
    if (name == "mix") return automatedMix;
    if (name == "reverse") return automatedReverse;
    if (name == "stereoWidth") return automatedStereoWidth;
    if (name == "quality") return automatedQuality;
    return false;
}

void ReverseDelayEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        time = 500.0f;
        feedback = 0.3f;
        mix = 0.5f;
        reverse = 0.5f;
        stereoWidth = 0.5f;
        quality = 1.0f;
    }
    else if (presetName == "Heavy") {
        time = 750.0f;
        feedback = 0.6f;
        mix = 0.7f;
        reverse = 0.8f;
        stereoWidth = 0.8f;
        quality = 1.0f;
    }
    else if (presetName == "Subtle") {
        time = 250.0f;
        feedback = 0.2f;
        mix = 0.3f;
        reverse = 0.3f;
        stereoWidth = 0.3f;
        quality = 1.0f;
    }
    
    updateDelayLines();
}

void ReverseDelayEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> ReverseDelayEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 