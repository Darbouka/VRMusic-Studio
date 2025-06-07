#include "ConvolutionEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

ConvolutionEffect::ConvolutionEffect()
    : mix(0.5f)
    , stereoWidth(0.5f)
    , gain(1.0f)
    , predelay(0.0f)
    , damping(0.5f)
    , size(0.5f)
    , diffusion(0.5f)
    , automatedMix(false)
    , automatedStereoWidth(false)
    , automatedGain(false)
    , automatedPredelay(false)
    , automatedDamping(false)
    , automatedSize(false)
    , automatedDiffusion(false)
    , irLength(0)
    , blockSize(2048)
    , bufferPos(0)
{
    inputBuffer.resize(blockSize * 2, 0.0f);   // Stereo-Buffer
    outputBuffer.resize(blockSize * 2, 0.0f);  // Stereo-Buffer
}

ConvolutionEffect::~ConvolutionEffect() {
    impulseResponse.clear();
    inputBuffer.clear();
    outputBuffer.clear();
}

bool ConvolutionEffect::initialize() {
    // Standard-Impulsantwort laden
    loadImpulseResponse("Standard");
    return true;
}

void ConvolutionEffect::shutdown() {
    impulseResponse.clear();
    inputBuffer.clear();
    outputBuffer.clear();
}

void ConvolutionEffect::loadImpulseResponse(const std::string& name) {
    // Hier würde das Laden der Impulsantwort implementiert werden
    // Für dieses Beispiel erstellen wir eine einfache Impulsantwort
    
    irLength = static_cast<size_t>(44100.0f * size);  // 1 Sekunde bei 44.1kHz
    impulseResponse.resize(irLength * 2, 0.0f);  // Stereo-Impulsantwort
    
    // Einfache Impulsantwort generieren
    for (size_t i = 0; i < irLength; i++) {
        float t = static_cast<float>(i) / irLength;
        float decay = std::exp(-t * 10.0f * (1.0f - damping));
        float pan = std::sin(t * M_PI * 2.0f) * diffusion;
        
        impulseResponse[i * 2] = decay * (1.0f - pan);      // Links
        impulseResponse[i * 2 + 1] = decay * (1.0f + pan);  // Rechts
    }
}

std::vector<PluginParameter> ConvolutionEffect::getParameters() const {
    return {
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"stereoWidth", "Stereo Width", PluginParameter::Type::Float, 0.0f, 1.0f, stereoWidth},
        {"gain", "Gain", PluginParameter::Type::Float, 0.0f, 2.0f, gain},
        {"predelay", "Pre-delay", PluginParameter::Type::Float, 0.0f, 1.0f, predelay},
        {"damping", "Damping", PluginParameter::Type::Float, 0.0f, 1.0f, damping},
        {"size", "Size", PluginParameter::Type::Float, 0.0f, 1.0f, size},
        {"diffusion", "Diffusion", PluginParameter::Type::Float, 0.0f, 1.0f, diffusion}
    };
}

void ConvolutionEffect::setParameter(const std::string& name, float value) {
    if (name == "mix") mix = value;
    else if (name == "stereoWidth") stereoWidth = value;
    else if (name == "gain") gain = value;
    else if (name == "predelay") predelay = value;
    else if (name == "damping") {
        damping = value;
        loadImpulseResponse("Standard");
    }
    else if (name == "size") {
        size = value;
        loadImpulseResponse("Standard");
    }
    else if (name == "diffusion") {
        diffusion = value;
        loadImpulseResponse("Standard");
    }
}

float ConvolutionEffect::getParameter(const std::string& name) const {
    if (name == "mix") return mix;
    if (name == "stereoWidth") return stereoWidth;
    if (name == "gain") return gain;
    if (name == "predelay") return predelay;
    if (name == "damping") return damping;
    if (name == "size") return size;
    if (name == "diffusion") return diffusion;
    return 0.0f;
}

void ConvolutionEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "mix") automatedMix = automated;
    else if (name == "stereoWidth") automatedStereoWidth = automated;
    else if (name == "gain") automatedGain = automated;
    else if (name == "predelay") automatedPredelay = automated;
    else if (name == "damping") automatedDamping = automated;
    else if (name == "size") automatedSize = automated;
    else if (name == "diffusion") automatedDiffusion = automated;
}

bool ConvolutionEffect::isParameterAutomated(const std::string& name) const {
    if (name == "mix") return automatedMix;
    if (name == "stereoWidth") return automatedStereoWidth;
    if (name == "gain") return automatedGain;
    if (name == "predelay") return automatedPredelay;
    if (name == "damping") return automatedDamping;
    if (name == "size") return automatedSize;
    if (name == "diffusion") return automatedDiffusion;
    return false;
}

void ConvolutionEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    // Input-Buffer füllen
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        inputBuffer[bufferPos] = buffer[i];
        inputBuffer[bufferPos + 1] = buffer[i + 1];
        bufferPos += 2;
        
        if (bufferPos >= blockSize * 2) {
            // Block verarbeiten
            processBlock(inputBuffer.data(), outputBuffer.data(), blockSize);
            
            // Output-Buffer zurückschreiben
            for (size_t j = 0; j < blockSize * 2; j += 2) {
                buffer[i - (blockSize * 2 - j)] = outputBuffer[j];
                buffer[i - (blockSize * 2 - j) + 1] = outputBuffer[j + 1];
            }
            
            // Buffer zurücksetzen
            bufferPos = 0;
        }
    }
}

void ConvolutionEffect::processBlock(float* input, float* output, size_t length) {
    // Output-Buffer löschen
    std::fill(output, output + length * 2, 0.0f);
    
    // Faltung durchführen
    for (size_t i = 0; i < length; i++) {
        for (size_t j = 0; j < irLength; j++) {
            if (i + j < length) {
                // Links
                output[i * 2] += input[j * 2] * impulseResponse[j * 2] * gain;
                // Rechts
                output[i * 2 + 1] += input[j * 2 + 1] * impulseResponse[j * 2 + 1] * gain;
            }
        }
    }
    
    // Stereo-Width
    for (size_t i = 0; i < length; i++) {
        float mid = (output[i * 2] + output[i * 2 + 1]) * 0.5f;
        float side = (output[i * 2] - output[i * 2 + 1]) * 0.5f;
        output[i * 2] = mid + side * stereoWidth;
        output[i * 2 + 1] = mid - side * stereoWidth;
    }
    
    // Mix
    for (size_t i = 0; i < length; i++) {
        output[i * 2] = output[i * 2] * mix;
        output[i * 2 + 1] = output[i * 2 + 1] * mix;
    }
}

void ConvolutionEffect::updateBuffers() {
    // Buffer-Größen anpassen
    size_t newBlockSize = static_cast<size_t>(blockSize * size);
    if (newBlockSize != blockSize) {
        blockSize = newBlockSize;
        inputBuffer.resize(blockSize * 2, 0.0f);
        outputBuffer.resize(blockSize * 2, 0.0f);
        bufferPos = 0;
    }
}

void ConvolutionEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        mix = 0.5f;
        stereoWidth = 0.5f;
        gain = 1.0f;
        predelay = 0.0f;
        damping = 0.5f;
        size = 0.5f;
        diffusion = 0.5f;
    }
    else if (presetName == "Large") {
        mix = 0.7f;
        stereoWidth = 0.7f;
        gain = 1.2f;
        predelay = 0.1f;
        damping = 0.3f;
        size = 0.8f;
        diffusion = 0.7f;
    }
    else if (presetName == "Small") {
        mix = 0.3f;
        stereoWidth = 0.3f;
        gain = 0.8f;
        predelay = 0.0f;
        damping = 0.7f;
        size = 0.3f;
        diffusion = 0.3f;
    }
    
    loadImpulseResponse(presetName);
    updateBuffers();
}

void ConvolutionEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> ConvolutionEffect::getAvailablePresets() const {
    return {"Standard", "Large", "Small"};
}

} // namespace VR_DAW 