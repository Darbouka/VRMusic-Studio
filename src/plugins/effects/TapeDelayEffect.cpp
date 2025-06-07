#include "TapeDelayEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

TapeDelayEffect::TapeDelayEffect()
    : time(500.0f)
    , feedback(0.3f)
    , mix(0.5f)
    , wow(0.2f)
    , flutter(0.1f)
    , saturation(0.3f)
    , quality(0.5f)
    , automatedTime(false)
    , automatedFeedback(false)
    , automatedMix(false)
    , automatedWow(false)
    , automatedFlutter(false)
    , automatedSaturation(false)
    , automatedQuality(false)
    , bufferSize(44100 * 2) // 2 Sekunden Buffer
    , writePos(0)
    , readPos(0)
    , wowPhase(0.0f)
    , flutterPhase(0.0f)
    , lastDelayTime(500.0f)
    , currentDelayTime(500.0f)
{
    initializeBuffer();
}

TapeDelayEffect::~TapeDelayEffect() {
}

bool TapeDelayEffect::initialize() {
    initializeBuffer();
    return true;
}

void TapeDelayEffect::shutdown() {
    buffer.clear();
}

std::vector<PluginParameter> TapeDelayEffect::getParameters() const {
    return {
        {"time", "Time", PluginParameter::Type::Float, 0.0f, 2000.0f, time},
        {"feedback", "Feedback", PluginParameter::Type::Float, 0.0f, 0.9f, feedback},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"wow", "Wow", PluginParameter::Type::Float, 0.0f, 1.0f, wow},
        {"flutter", "Flutter", PluginParameter::Type::Float, 0.0f, 1.0f, flutter},
        {"saturation", "Saturation", PluginParameter::Type::Float, 0.0f, 1.0f, saturation},
        {"quality", "Quality", PluginParameter::Type::Float, 0.0f, 1.0f, quality}
    };
}

void TapeDelayEffect::setParameter(const std::string& name, float value) {
    if (name == "time") {
        time = value;
        lastDelayTime = currentDelayTime;
    }
    else if (name == "feedback") feedback = value;
    else if (name == "mix") mix = value;
    else if (name == "wow") wow = value;
    else if (name == "flutter") flutter = value;
    else if (name == "saturation") saturation = value;
    else if (name == "quality") quality = value;
}

float TapeDelayEffect::getParameter(const std::string& name) const {
    if (name == "time") return time;
    if (name == "feedback") return feedback;
    if (name == "mix") return mix;
    if (name == "wow") return wow;
    if (name == "flutter") return flutter;
    if (name == "saturation") return saturation;
    if (name == "quality") return quality;
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
    if (name == "feedback") return automatedFeedback;
    if (name == "mix") return automatedMix;
    if (name == "wow") return automatedWow;
    if (name == "flutter") return automatedFlutter;
    if (name == "saturation") return automatedSaturation;
    if (name == "quality") return automatedQuality;
    return false;
}

void TapeDelayEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        float input = (left + right) * 0.5f;
        
        // Delay-Zeit aktualisieren
        updateDelayTime();
        
        // Input in Buffer schreiben
        this->buffer[writePos] = input;
        writePos = (writePos + 1) % bufferSize;
        
        // Delay verarbeiten
        float output = processSample(input);
        
        // Sättigung
        output = processSaturation(output);
        
        // Feedback
        if (feedback > 0.0f) {
            this->buffer[writePos] += output * feedback;
        }
        
        // Mix
        buffer[i] = left * (1.0f - mix) + output * mix;
        buffer[i + 1] = right * (1.0f - mix) + output * mix;
    }
}

void TapeDelayEffect::initializeBuffer() {
    buffer.resize(bufferSize, 0.0f);
    writePos = 0;
    readPos = 0;
    wowPhase = 0.0f;
    flutterPhase = 0.0f;
    lastDelayTime = time;
    currentDelayTime = time;
}

void TapeDelayEffect::updateBuffer() {
    // Buffer aktualisieren (z.B. für Feedback)
    if (feedback > 0.0f) {
        for (unsigned long i = 0; i < bufferSize; ++i) {
            buffer[i] *= feedback;
        }
    }
}

float TapeDelayEffect::processSample(float input) {
    // Wow und Flutter berechnen
    float wowValue = processWow();
    float flutterValue = processFlutter();
    
    // Verzögerungszeit mit Modulation
    float delayTime = currentDelayTime * (1.0f + wowValue + flutterValue);
    unsigned long delaySamples = static_cast<unsigned long>(delayTime * 44.1f);
    
    // Leseposition berechnen
    readPos = (writePos + bufferSize - delaySamples) % bufferSize;
    
    // Interpolation
    float readPosFloat = static_cast<float>(readPos);
    unsigned long pos1 = static_cast<unsigned long>(readPosFloat);
    unsigned long pos2 = (pos1 + 1) % bufferSize;
    float frac = readPosFloat - pos1;
    
    float sample = buffer[pos1] * (1.0f - frac) + buffer[pos2] * frac;
    
    return sample;
}

void TapeDelayEffect::updateDelayTime() {
    // Sanfte Übergänge für die Verzögerungszeit
    currentDelayTime = lastDelayTime + (time - lastDelayTime) * 0.01f;
    lastDelayTime = currentDelayTime;
}

float TapeDelayEffect::processWow() {
    // Wow-Effekt (langsame Modulation)
    wowPhase += 0.1f / 44100.0f;
    if (wowPhase >= 1.0f) wowPhase -= 1.0f;
    
    return std::sin(2.0f * M_PI * wowPhase) * wow;
}

float TapeDelayEffect::processFlutter() {
    // Flutter-Effekt (schnelle Modulation)
    flutterPhase += 10.0f / 44100.0f;
    if (flutterPhase >= 1.0f) flutterPhase -= 1.0f;
    
    return std::sin(2.0f * M_PI * flutterPhase) * flutter;
}

float TapeDelayEffect::processSaturation(float input) {
    // Tape-Sättigung
    float threshold = 0.8f;
    float knee = 0.1f;
    
    if (std::abs(input) < threshold - knee) {
        return input;
    }
    else if (std::abs(input) > threshold + knee) {
        return std::tanh(input * (1.0f + saturation)) * (1.0f - saturation * 0.5f);
    }
    else {
        float t = (std::abs(input) - (threshold - knee)) / (2.0f * knee);
        float linear = input;
        float saturated = std::tanh(input * (1.0f + saturation)) * (1.0f - saturation * 0.5f);
        return linear + (saturated - linear) * t;
    }
}

void TapeDelayEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        time = 500.0f;
        feedback = 0.3f;
        mix = 0.5f;
        wow = 0.2f;
        flutter = 0.1f;
        saturation = 0.3f;
        quality = 0.5f;
    }
    else if (presetName == "Heavy") {
        time = 1000.0f;
        feedback = 0.5f;
        mix = 0.7f;
        wow = 0.4f;
        flutter = 0.2f;
        saturation = 0.5f;
        quality = 0.7f;
    }
    else if (presetName == "Subtle") {
        time = 250.0f;
        feedback = 0.2f;
        mix = 0.3f;
        wow = 0.1f;
        flutter = 0.05f;
        saturation = 0.2f;
        quality = 0.3f;
    }
    
    initializeBuffer();
}

void TapeDelayEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> TapeDelayEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 