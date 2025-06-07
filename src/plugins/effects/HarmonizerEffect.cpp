#include "HarmonizerEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

HarmonizerEffect::HarmonizerEffect()
    : pitch1(4.0f)
    , pitch2(7.0f)
    , mix(0.5f)
    , formant(0.5f)
    , grainSize(0.02f)
    , overlap(0.5f)
    , window(0.5f)
    , quality(0.5f)
    , automatedPitch1(false)
    , automatedPitch2(false)
    , automatedMix(false)
    , automatedFormant(false)
    , automatedGrainSize(false)
    , automatedOverlap(false)
    , automatedWindow(false)
    , automatedQuality(false)
    , writePos(0)
    , readPos1(0)
    , readPos2(0)
{
    // Buffer für 100ms bei 44.1kHz
    const size_t bufferSize = static_cast<size_t>(0.1f * 44100.0f);
    inputBuffer.resize(bufferSize, 0.0f);
    outputBuffer1.resize(bufferSize, 0.0f);
    outputBuffer2.resize(bufferSize, 0.0f);
}

HarmonizerEffect::~HarmonizerEffect() {
    inputBuffer.clear();
    outputBuffer1.clear();
    outputBuffer2.clear();
}

bool HarmonizerEffect::initialize() {
    writePos = 0;
    readPos1 = 0;
    readPos2 = 0;
    std::fill(inputBuffer.begin(), inputBuffer.end(), 0.0f);
    std::fill(outputBuffer1.begin(), outputBuffer1.end(), 0.0f);
    std::fill(outputBuffer2.begin(), outputBuffer2.end(), 0.0f);
    return true;
}

void HarmonizerEffect::shutdown() {
    inputBuffer.clear();
    outputBuffer1.clear();
    outputBuffer2.clear();
}

std::vector<PluginParameter> HarmonizerEffect::getParameters() const {
    return {
        {"pitch1", "Pitch 1", PluginParameter::Type::Float, -12.0f, 12.0f, pitch1},
        {"pitch2", "Pitch 2", PluginParameter::Type::Float, -12.0f, 12.0f, pitch2},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"formant", "Formant", PluginParameter::Type::Float, 0.0f, 1.0f, formant},
        {"grainSize", "Grain Size", PluginParameter::Type::Float, 0.001f, 0.1f, grainSize},
        {"overlap", "Overlap", PluginParameter::Type::Float, 0.0f, 0.9f, overlap},
        {"window", "Window", PluginParameter::Type::Float, 0.0f, 1.0f, window},
        {"quality", "Quality", PluginParameter::Type::Float, 0.0f, 1.0f, quality}
    };
}

void HarmonizerEffect::setParameter(const std::string& name, float value) {
    if (name == "pitch1") pitch1 = value;
    else if (name == "pitch2") pitch2 = value;
    else if (name == "mix") mix = value;
    else if (name == "formant") formant = value;
    else if (name == "grainSize") {
        grainSize = value;
        updateBuffers();
    }
    else if (name == "overlap") overlap = value;
    else if (name == "window") window = value;
    else if (name == "quality") quality = value;
}

float HarmonizerEffect::getParameter(const std::string& name) const {
    if (name == "pitch1") return pitch1;
    if (name == "pitch2") return pitch2;
    if (name == "mix") return mix;
    if (name == "formant") return formant;
    if (name == "grainSize") return grainSize;
    if (name == "overlap") return overlap;
    if (name == "window") return window;
    if (name == "quality") return quality;
    return 0.0f;
}

void HarmonizerEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "pitch1") automatedPitch1 = automated;
    else if (name == "pitch2") automatedPitch2 = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "formant") automatedFormant = automated;
    else if (name == "grainSize") automatedGrainSize = automated;
    else if (name == "overlap") automatedOverlap = automated;
    else if (name == "window") automatedWindow = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool HarmonizerEffect::isParameterAutomated(const std::string& name) const {
    if (name == "pitch1") return automatedPitch1;
    if (name == "pitch2") return automatedPitch2;
    if (name == "mix") return automatedMix;
    if (name == "formant") return automatedFormant;
    if (name == "grainSize") return automatedGrainSize;
    if (name == "overlap") return automatedOverlap;
    if (name == "window") return automatedWindow;
    if (name == "quality") return automatedQuality;
    return false;
}

void HarmonizerEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // Eingang in Buffer schreiben
        inputBuffer[writePos] = (left + right) * 0.5f;
        writePos = (writePos + 1) % inputBuffer.size();
        
        // Grain-Größe in Samples
        size_t grainSizeSamples = static_cast<size_t>(grainSize * 44100.0f);
        
        // Pitch-Shift-Faktoren
        float shiftFactor1 = std::pow(2.0f, pitch1 / 12.0f);
        float shiftFactor2 = std::pow(2.0f, pitch2 / 12.0f);
        
        // Grains verarbeiten
        float processed1 = 0.0f;
        float processed2 = 0.0f;
        float windowSum = 0.0f;
        
        for (size_t j = 0; j < grainSizeSamples; ++j) {
            // Fenster-Funktion
            float windowValue = calculateWindow(static_cast<float>(j) / grainSizeSamples);
            windowSum += windowValue;
            
            // Eingang lesen
            size_t readIndex = (writePos + inputBuffer.size() - j) % inputBuffer.size();
            float input = inputBuffer[readIndex];
            
            // Ausgänge lesen
            size_t writeIndex1 = (readPos1 + j) % outputBuffer1.size();
            size_t writeIndex2 = (readPos2 + j) % outputBuffer2.size();
            float output1 = outputBuffer1[writeIndex1];
            float output2 = outputBuffer2[writeIndex2];
            
            // Mix
            processed1 += (input * (1.0f - formant) + output1 * formant) * windowValue;
            processed2 += (input * (1.0f - formant) + output2 * formant) * windowValue;
        }
        
        // Normalisierung
        if (windowSum > 0.0f) {
            processed1 /= windowSum;
            processed2 /= windowSum;
        }
        
        // Ausgänge in Buffer schreiben
        outputBuffer1[readPos1] = processed1;
        outputBuffer2[readPos2] = processed2;
        readPos1 = (readPos1 + 1) % outputBuffer1.size();
        readPos2 = (readPos2 + 1) % outputBuffer2.size();
        
        // Mix
        float mixed = (processed1 + processed2) * 0.5f;
        buffer[i] = left * (1.0f - mix) + mixed * mix;
        buffer[i + 1] = right * (1.0f - mix) + mixed * mix;
    }
}

float HarmonizerEffect::calculateWindow(float position) {
    // Hanning-Fenster
    return 0.5f * (1.0f - std::cos(2.0f * M_PI * position));
}

void HarmonizerEffect::updateBuffers() {
    // Buffer-Größe basierend auf Grain-Größe aktualisieren
    size_t newSize = static_cast<size_t>(grainSize * 44100.0f * 2.0f);
    if (newSize != inputBuffer.size()) {
        inputBuffer.resize(newSize, 0.0f);
        outputBuffer1.resize(newSize, 0.0f);
        outputBuffer2.resize(newSize, 0.0f);
        writePos = 0;
        readPos1 = 0;
        readPos2 = 0;
    }
}

void HarmonizerEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        pitch1 = 4.0f;
        pitch2 = 7.0f;
        mix = 0.5f;
        formant = 0.5f;
        grainSize = 0.02f;
        overlap = 0.5f;
        window = 0.5f;
        quality = 0.5f;
    }
    else if (presetName == "Major Third") {
        pitch1 = 4.0f;
        pitch2 = 7.0f;
        mix = 0.5f;
        formant = 0.7f;
        grainSize = 0.01f;
        overlap = 0.7f;
        window = 0.5f;
        quality = 0.7f;
    }
    else if (presetName == "Minor Third") {
        pitch1 = 3.0f;
        pitch2 = 7.0f;
        mix = 0.5f;
        formant = 0.3f;
        grainSize = 0.03f;
        overlap = 0.3f;
        window = 0.5f;
        quality = 0.7f;
    }
    
    updateBuffers();
}

void HarmonizerEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> HarmonizerEffect::getAvailablePresets() const {
    return {"Standard", "Major Third", "Minor Third"};
}

} // namespace VR_DAW 