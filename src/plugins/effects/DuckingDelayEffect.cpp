#include "DuckingDelayEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

DuckingDelayEffect::DuckingDelayEffect()
    : time(500.0f)
    , feedback(0.3f)
    , mix(0.5f)
    , threshold(-20.0f)
    , ratio(4.0f)
    , attack(10.0f)
    , release(100.0f)
    , quality(1.0f)
    , automatedTime(false)
    , automatedFeedback(false)
    , automatedMix(false)
    , automatedThreshold(false)
    , automatedRatio(false)
    , automatedAttack(false)
    , automatedRelease(false)
    , automatedQuality(false)
    , bufferSize(0)
{
    initializeDelayLines();
}

DuckingDelayEffect::~DuckingDelayEffect() {
    shutdown();
}

bool DuckingDelayEffect::initialize() {
    initializeDelayLines();
    return true;
}

void DuckingDelayEffect::shutdown() {
    leftDelay.buffer.clear();
    rightDelay.buffer.clear();
}

void DuckingDelayEffect::initializeDelayLines() {
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
    leftDelay.envelope = 0.0f;
    rightDelay.envelope = 0.0f;
    
    // Berechne Hüllkurven-Koeffizienten
    updateEnvelopeCoefficients();
}

void DuckingDelayEffect::updateEnvelopeCoefficients() {
    // Berechne Attack- und Release-Koeffizienten
    float attackTime = attack / 1000.0f; // ms zu s
    float releaseTime = release / 1000.0f; // ms zu s
    
    leftDelay.attackCoeff = std::exp(-1.0f / (attackTime * 44100.0f));
    rightDelay.attackCoeff = leftDelay.attackCoeff;
    leftDelay.releaseCoeff = std::exp(-1.0f / (releaseTime * 44100.0f));
    rightDelay.releaseCoeff = leftDelay.releaseCoeff;
}

void DuckingDelayEffect::updateDelayLines() {
    // Aktualisiere Verzögerungszeiten
    updateDelayTimes();
    
    // Aktualisiere Hüllkurven-Koeffizienten
    updateEnvelopeCoefficients();
}

void DuckingDelayEffect::updateDelayTimes() {
    // Berechne neue Lesepositionen
    unsigned long delaySamples = static_cast<unsigned long>(time * 44.1f);
    
    leftDelay.readPos = (leftDelay.writePos + bufferSize - delaySamples) % bufferSize;
    rightDelay.readPos = (rightDelay.writePos + bufferSize - delaySamples) % bufferSize;
}

void DuckingDelayEffect::updateEnvelope(DelayLine& delay, float input) {
    // Berechne Eingangspegel
    float inputLevel = std::abs(input);
    
    // Update Hüllkurve
    if (inputLevel > delay.envelope) {
        delay.envelope = inputLevel + (delay.envelope - inputLevel) * delay.attackCoeff;
    } else {
        delay.envelope = inputLevel + (delay.envelope - inputLevel) * delay.releaseCoeff;
    }
}

float DuckingDelayEffect::calculateGain(float envelope) {
    // Konvertiere Hüllkurve zu dB
    float envelopeDB = 20.0f * std::log10(envelope + 1e-6f);
    
    // Berechne Kompression
    if (envelopeDB > threshold) {
        float gainReduction = (envelopeDB - threshold) * (1.0f - 1.0f/ratio);
        return std::pow(10.0f, -gainReduction / 20.0f);
    }
    
    return 1.0f;
}

float DuckingDelayEffect::processDelayLine(DelayLine& delay, float input) {
    // Update Hüllkurve
    updateEnvelope(delay, input);
    
    // Berechne Verstärkung
    float gain = calculateGain(delay.envelope);
    
    // Schreibe in Puffer
    delay.buffer[delay.writePos] = input;
    delay.writePos = (delay.writePos + 1) % bufferSize;
    
    // Lese aus Puffer
    float output = delay.buffer[delay.readPos];
    delay.readPos = (delay.readPos + 1) % bufferSize;
    
    // Wende Verstärkung an
    return output * gain;
}

void DuckingDelayEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer * 2; i += 2) {
        float leftInput = buffer[i];
        float rightInput = buffer[i + 1];
        
        // Verarbeite Verzögerung
        float leftDelay = processDelayLine(leftDelay, leftInput);
        float rightDelay = processDelayLine(rightDelay, rightInput);
        
        // Feedback
        leftDelay *= feedback;
        rightDelay *= feedback;
        
        // Mix
        buffer[i] = leftInput + leftDelay * mix;
        buffer[i + 1] = rightInput + rightDelay * mix;
    }
}

std::vector<PluginParameter> DuckingDelayEffect::getParameters() const {
    return {
        {"time", time, 0.0f, 2000.0f, "ms"},
        {"feedback", feedback, 0.0f, 0.9f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"threshold", threshold, -60.0f, 0.0f, "dB"},
        {"ratio", ratio, 1.0f, 20.0f, ""},
        {"attack", attack, 0.1f, 100.0f, "ms"},
        {"release", release, 10.0f, 1000.0f, "ms"},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void DuckingDelayEffect::setParameter(const std::string& name, float value) {
    if (name == "time") {
        time = value;
        updateDelayTimes();
    }
    else if (name == "feedback") feedback = value;
    else if (name == "mix") mix = value;
    else if (name == "threshold") threshold = value;
    else if (name == "ratio") ratio = value;
    else if (name == "attack") {
        attack = value;
        updateEnvelopeCoefficients();
    }
    else if (name == "release") {
        release = value;
        updateEnvelopeCoefficients();
    }
    else if (name == "quality") quality = value;
}

float DuckingDelayEffect::getParameter(const std::string& name) const {
    if (name == "time") return time;
    if (name == "feedback") return feedback;
    if (name == "mix") return mix;
    if (name == "threshold") return threshold;
    if (name == "ratio") return ratio;
    if (name == "attack") return attack;
    if (name == "release") return release;
    if (name == "quality") return quality;
    return 0.0f;
}

void DuckingDelayEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "time") automatedTime = automated;
    else if (name == "feedback") automatedFeedback = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "threshold") automatedThreshold = automated;
    else if (name == "ratio") automatedRatio = automated;
    else if (name == "attack") automatedAttack = automated;
    else if (name == "release") automatedRelease = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool DuckingDelayEffect::isParameterAutomated(const std::string& name) const {
    if (name == "time") return automatedTime;
    if (name == "feedback") return automatedFeedback;
    if (name == "mix") return automatedMix;
    if (name == "threshold") return automatedThreshold;
    if (name == "ratio") return automatedRatio;
    if (name == "attack") return automatedAttack;
    if (name == "release") return automatedRelease;
    if (name == "quality") return automatedQuality;
    return false;
}

void DuckingDelayEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        time = 500.0f;
        feedback = 0.3f;
        mix = 0.5f;
        threshold = -20.0f;
        ratio = 4.0f;
        attack = 10.0f;
        release = 100.0f;
        quality = 1.0f;
    }
    else if (presetName == "Heavy") {
        time = 750.0f;
        feedback = 0.6f;
        mix = 0.7f;
        threshold = -30.0f;
        ratio = 8.0f;
        attack = 5.0f;
        release = 200.0f;
        quality = 1.0f;
    }
    else if (presetName == "Subtle") {
        time = 250.0f;
        feedback = 0.2f;
        mix = 0.3f;
        threshold = -10.0f;
        ratio = 2.0f;
        attack = 20.0f;
        release = 50.0f;
        quality = 1.0f;
    }
    
    updateDelayLines();
}

void DuckingDelayEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> DuckingDelayEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 