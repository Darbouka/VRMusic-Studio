#include "LimiterEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

LimiterEffect::LimiterEffect()
    : threshold(-1.0f)
    , attack(0.001f)
    , release(0.1f)
    , makeup(0.0f)
    , mix(0.5f)
    , stereoWidth(0.5f)
    , phase(0.0f)
    , lookahead(0.001f)
    , automatedThreshold(false)
    , automatedAttack(false)
    , automatedRelease(false)
    , automatedMakeup(false)
    , automatedMix(false)
    , automatedStereoWidth(false)
    , automatedPhase(false)
    , automatedLookahead(false)
    , envelope(0.0f)
    , gain(1.0f)
    , lastLeft(0.0f)
    , lastRight(0.0f)
    , writeIndex(0)
    , readIndex(0)
    , bufferSize(0)
{
}

LimiterEffect::~LimiterEffect() {}

bool LimiterEffect::initialize() {
    // Lookahead-Buffer initialisieren
    bufferSize = static_cast<unsigned long>(lookahead * 44100.0f);
    lookaheadBuffer.resize(bufferSize * 2, 0.0f);  // Stereo-Buffer
    writeIndex = 0;
    readIndex = 0;
    return true;
}

void LimiterEffect::shutdown() {
    lookaheadBuffer.clear();
}

std::vector<PluginParameter> LimiterEffect::getParameters() const {
    return {
        {"threshold", "Threshold", PluginParameter::Type::Float, -20.0f, 0.0f, threshold},
        {"attack", "Attack", PluginParameter::Type::Float, 0.0001f, 0.01f, attack},
        {"release", "Release", PluginParameter::Type::Float, 0.01f, 1.0f, release},
        {"makeup", "Makeup", PluginParameter::Type::Float, 0.0f, 24.0f, makeup},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"stereoWidth", "Stereo Width", PluginParameter::Type::Float, 0.0f, 1.0f, stereoWidth},
        {"phase", "Phase", PluginParameter::Type::Float, 0.0f, 1.0f, phase},
        {"lookahead", "Lookahead", PluginParameter::Type::Float, 0.0f, 0.01f, lookahead}
    };
}

void LimiterEffect::setParameter(const std::string& name, float value) {
    if (name == "threshold") threshold = value;
    else if (name == "attack") attack = value;
    else if (name == "release") release = value;
    else if (name == "makeup") makeup = value;
    else if (name == "mix") mix = value;
    else if (name == "stereoWidth") stereoWidth = value;
    else if (name == "phase") phase = value;
    else if (name == "lookahead") {
        lookahead = value;
        // Buffer neu initialisieren
        bufferSize = static_cast<unsigned long>(lookahead * 44100.0f);
        lookaheadBuffer.resize(bufferSize * 2, 0.0f);
        writeIndex = 0;
        readIndex = 0;
    }
}

float LimiterEffect::getParameter(const std::string& name) const {
    if (name == "threshold") return threshold;
    if (name == "attack") return attack;
    if (name == "release") return release;
    if (name == "makeup") return makeup;
    if (name == "mix") return mix;
    if (name == "stereoWidth") return stereoWidth;
    if (name == "phase") return phase;
    if (name == "lookahead") return lookahead;
    return 0.0f;
}

void LimiterEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "threshold") automatedThreshold = automated;
    else if (name == "attack") automatedAttack = automated;
    else if (name == "release") automatedRelease = automated;
    else if (name == "makeup") automatedMakeup = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "stereoWidth") automatedStereoWidth = automated;
    else if (name == "phase") automatedPhase = automated;
    else if (name == "lookahead") automatedLookahead = automated;
}

bool LimiterEffect::isParameterAutomated(const std::string& name) const {
    if (name == "threshold") return automatedThreshold;
    if (name == "attack") return automatedAttack;
    if (name == "release") return automatedRelease;
    if (name == "makeup") return automatedMakeup;
    if (name == "mix") return automatedMix;
    if (name == "stereoWidth") return automatedStereoWidth;
    if (name == "phase") return automatedPhase;
    if (name == "lookahead") return automatedLookahead;
    return false;
}

void LimiterEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        float left = buffer[i];
        float right = buffer[i + 1];

        // Lookahead-Buffer füllen
        lookaheadBuffer[writeIndex] = left;
        lookaheadBuffer[writeIndex + 1] = right;

        // Eingangspegel berechnen
        float inputLevel = std::max(std::abs(left), std::abs(right));
        inputLevel = 20.0f * std::log10(inputLevel + 1e-6f);

        // Hüllkurve berechnen
        envelope = processEnvelope(inputLevel);

        // Gain berechnen
        float newGain = calculateGain(envelope);
        gain = newGain;

        // Makeup-Gain anwenden
        float makeupGain = std::pow(10.0f, makeup / 20.0f);
        gain *= makeupGain;

        // Limiter auf Lookahead-Signal anwenden
        float limitedLeft = lookaheadBuffer[readIndex] * gain;
        float limitedRight = lookaheadBuffer[readIndex + 1] * gain;

        // Stereo-Width
        float mid = (limitedLeft + limitedRight) * 0.5f;
        float side = (limitedLeft - limitedRight) * 0.5f;
        limitedLeft = mid + side * stereoWidth;
        limitedRight = mid - side * stereoWidth;

        // Mix
        buffer[i] = left * (1.0f - mix) + limitedLeft * mix;
        buffer[i + 1] = right * (1.0f - mix) + limitedRight * mix;

        // Indizes aktualisieren
        writeIndex = (writeIndex + 2) % (bufferSize * 2);
        readIndex = (readIndex + 2) % (bufferSize * 2);
    }
}

float LimiterEffect::calculateGain(float input) {
    if (input <= threshold) {
        return 1.0f;
    }
    else {
        return std::pow(10.0f, (threshold - input) / 20.0f);
    }
}

float LimiterEffect::processEnvelope(float input) {
    float target = std::abs(input);
    float attackCoeff = std::exp(-1.0f / (attack * 44100.0f));
    float releaseCoeff = std::exp(-1.0f / (release * 44100.0f));

    if (target > envelope) {
        envelope = target + (envelope - target) * attackCoeff;
    }
    else {
        envelope = target + (envelope - target) * releaseCoeff;
    }

    return envelope;
}

float LimiterEffect::calculateSyncTime() {
    // Sync-Zeit basierend auf BPM berechnen (angenommen 120 BPM)
    return (60.0f / 120.0f) * 1000.0f;
}

void LimiterEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        threshold = -1.0f;
        attack = 0.001f;
        release = 0.1f;
        makeup = 0.0f;
        mix = 0.5f;
        stereoWidth = 0.5f;
        phase = 0.0f;
        lookahead = 0.001f;
    }
    else if (presetName == "Brickwall") {
        threshold = -0.1f;
        attack = 0.0001f;
        release = 0.01f;
        makeup = 6.0f;
        mix = 0.7f;
        stereoWidth = 0.8f;
        phase = 0.0f;
        lookahead = 0.001f;
    }
    else if (presetName == "Subtle") {
        threshold = -3.0f;
        attack = 0.005f;
        release = 0.2f;
        makeup = 2.0f;
        mix = 0.3f;
        stereoWidth = 0.3f;
        phase = 0.0f;
        lookahead = 0.0005f;
    }
}

void LimiterEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> LimiterEffect::getAvailablePresets() const {
    return {"Standard", "Brickwall", "Subtle"};
}

} // namespace VR_DAW 