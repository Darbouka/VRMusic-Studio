#include "PitchShiftEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

PitchShiftEffect::PitchShiftEffect()
    : pitch(0.0f)
    , mix(0.5f)
    , formant(0.5f)
    , grainSize(0.02f)
    , overlap(0.5f)
    , window(0.5f)
    , quality(0.5f)
    , automatedPitch(false)
    , automatedMix(false)
    , automatedFormant(false)
    , automatedGrainSize(false)
    , automatedOverlap(false)
    , automatedWindow(false)
    , automatedQuality(false)
    , writePos(0)
    , readPos(0)
{
    // Buffer für 100ms bei 44.1kHz
    const size_t bufferSize = static_cast<size_t>(0.1f * 44100.0f);
    inputBuffer.resize(bufferSize, 0.0f);
    outputBuffer.resize(bufferSize, 0.0f);
}

PitchShiftEffect::~PitchShiftEffect() {
    inputBuffer.clear();
    outputBuffer.clear();
}

bool PitchShiftEffect::initialize() {
    writePos = 0;
    readPos = 0;
    std::fill(inputBuffer.begin(), inputBuffer.end(), 0.0f);
    std::fill(outputBuffer.begin(), outputBuffer.end(), 0.0f);
    return true;
}

void PitchShiftEffect::shutdown() {
    inputBuffer.clear();
    outputBuffer.clear();
}

std::vector<PluginParameter> PitchShiftEffect::getParameters() const {
    return {
        {"pitch", "Pitch", PluginParameter::Type::Float, -12.0f, 12.0f, pitch},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"formant", "Formant", PluginParameter::Type::Float, 0.0f, 1.0f, formant},
        {"grainSize", "Grain Size", PluginParameter::Type::Float, 0.001f, 0.1f, grainSize},
        {"overlap", "Overlap", PluginParameter::Type::Float, 0.0f, 0.9f, overlap},
        {"window", "Window", PluginParameter::Type::Float, 0.0f, 1.0f, window},
        {"quality", "Quality", PluginParameter::Type::Float, 0.0f, 1.0f, quality}
    };
}

void PitchShiftEffect::setParameter(const std::string& name, float value) {
    if (name == "pitch") pitch = value;
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

float PitchShiftEffect::getParameter(const std::string& name) const {
    if (name == "pitch") return pitch;
    if (name == "mix") return mix;
    if (name == "formant") return formant;
    if (name == "grainSize") return grainSize;
    if (name == "overlap") return overlap;
    if (name == "window") return window;
    if (name == "quality") return quality;
    return 0.0f;
}

void PitchShiftEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "pitch") automatedPitch = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "formant") automatedFormant = automated;
    else if (name == "grainSize") automatedGrainSize = automated;
    else if (name == "overlap") automatedOverlap = automated;
    else if (name == "window") automatedWindow = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool PitchShiftEffect::isParameterAutomated(const std::string& name) const {
    if (name == "pitch") return automatedPitch;
    if (name == "mix") return automatedMix;
    if (name == "formant") return automatedFormant;
    if (name == "grainSize") return automatedGrainSize;
    if (name == "overlap") return automatedOverlap;
    if (name == "window") return automatedWindow;
    if (name == "quality") return automatedQuality;
    return false;
}

void PitchShiftEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // Eingang in Buffer schreiben
        inputBuffer[writePos] = (left + right) * 0.5f;
        writePos = (writePos + 1) % inputBuffer.size();
        
        // Grain-Größe in Samples
        size_t grainSizeSamples = static_cast<size_t>(grainSize * 44100.0f);
        
        // Pitch-Shift-Faktor
        float shiftFactor = std::pow(2.0f, pitch / 12.0f);
        
        // Grain verarbeiten
        float processed = 0.0f;
        float windowSum = 0.0f;
        
        for (size_t j = 0; j < grainSizeSamples; ++j) {
            // Fenster-Funktion
            float windowValue = calculateWindow(static_cast<float>(j) / grainSizeSamples);
            windowSum += windowValue;
            
            // Eingang lesen
            size_t readIndex = (writePos + inputBuffer.size() - j) % inputBuffer.size();
            float input = inputBuffer[readIndex];
            
            // Ausgang lesen
            size_t writeIndex = (readPos + j) % outputBuffer.size();
            float output = outputBuffer[writeIndex];
            
            // Mix
            processed += (input * (1.0f - formant) + output * formant) * windowValue;
        }
        
        // Normalisierung
        if (windowSum > 0.0f) {
            processed /= windowSum;
        }
        
        // Ausgang in Buffer schreiben
        outputBuffer[readPos] = processed;
        readPos = (readPos + 1) % outputBuffer.size();
        
        // Mix
        buffer[i] = left * (1.0f - mix) + processed * mix;
        buffer[i + 1] = right * (1.0f - mix) + processed * mix;
    }
}

float PitchShiftEffect::calculateWindow(float position) {
    // Hanning-Fenster
    return 0.5f * (1.0f - std::cos(2.0f * M_PI * position));
}

void PitchShiftEffect::updateBuffers() {
    // Buffer-Größe basierend auf Grain-Größe aktualisieren
    size_t newSize = static_cast<size_t>(grainSize * 44100.0f * 2.0f);
    if (newSize != inputBuffer.size()) {
        inputBuffer.resize(newSize, 0.0f);
        outputBuffer.resize(newSize, 0.0f);
        writePos = 0;
        readPos = 0;
    }
}

void PitchShiftEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        pitch = 0.0f;
        mix = 0.5f;
        formant = 0.5f;
        grainSize = 0.02f;
        overlap = 0.5f;
        window = 0.5f;
        quality = 0.5f;
    }
    else if (presetName == "Octave Up") {
        pitch = 12.0f;
        mix = 0.5f;
        formant = 0.7f;
        grainSize = 0.01f;
        overlap = 0.7f;
        window = 0.5f;
        quality = 0.7f;
    }
    else if (presetName == "Octave Down") {
        pitch = -12.0f;
        mix = 0.5f;
        formant = 0.3f;
        grainSize = 0.03f;
        overlap = 0.3f;
        window = 0.5f;
        quality = 0.7f;
    }
    
    updateBuffers();
}

void PitchShiftEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> PitchShiftEffect::getAvailablePresets() const {
    return {"Standard", "Octave Up", "Octave Down"};
}

} // namespace VR_DAW 