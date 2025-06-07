#include "NeuralSaturatorEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

NeuralSaturatorEffect::NeuralSaturatorEffect()
    : drive(0.5f)
    , bias(0.0f)
    , learning(0.5f)
    , adaptation(0.5f)
    , complexity(0.5f)
    , mix(0.5f)
    , quality(1.0f)
    , automatedDrive(false)
    , automatedBias(false)
    , automatedLearning(false)
    , automatedAdaptation(false)
    , automatedComplexity(false)
    , automatedMix(false)
    , automatedQuality(false)
    , bufferSize(44100 * 2) // 2 Sekunden bei 44.1kHz
{
    std::random_device rd;
    rng.seed(rd());
    initializeLayers();
}

NeuralSaturatorEffect::~NeuralSaturatorEffect() {
    shutdown();
}

bool NeuralSaturatorEffect::initialize() {
    initializeLayers();
    return true;
}

void NeuralSaturatorEffect::shutdown() {
    layers.clear();
}

void NeuralSaturatorEffect::initializeLayers() {
    layers.clear();
    
    // Erstelle 4 Layer
    for (int i = 0; i < 4; ++i) {
        Layer layer;
        
        // Erstelle 16 Neuronen pro Layer
        for (int j = 0; j < 16; ++j) {
            Neuron neuron;
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            
            neuron.weight = dist(rng);
            neuron.bias = dist(rng) * 2.0f - 1.0f;
            neuron.activation = 0.0f;
            neuron.learning = learning;
            
            layer.neurons.push_back(neuron);
        }
        
        layer.complexity = complexity;
        layers.push_back(layer);
    }
}

void NeuralSaturatorEffect::updateLayers() {
    // Aktualisiere Layer
    for (auto& layer : layers) {
        layer.complexity = complexity;
        
        // Aktualisiere Neuronen
        for (auto& neuron : layer.neurons) {
            // Aktualisiere Gewicht basierend auf Lernen
            neuron.weight += neuron.learning * (std::rand() / RAND_MAX - 0.5f) * 0.01f;
            neuron.weight = std::max(0.0f, std::min(1.0f, neuron.weight));
            
            // Aktualisiere Bias
            neuron.bias += bias * (std::rand() / RAND_MAX - 0.5f) * 0.01f;
            neuron.bias = std::max(-1.0f, std::min(1.0f, neuron.bias));
            
            // Aktualisiere Aktivierung
            neuron.activation = std::tanh(neuron.weight * drive + neuron.bias);
        }
    }
}

void NeuralSaturatorEffect::processLayer(Layer& layer, float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float t = static_cast<float>(i) / framesPerBuffer;
        float output = buffer[i];
        
        // Verarbeite jedes Neuron
        for (auto& neuron : layer.neurons) {
            // Berechne Neuron-Wert
            float value = output * neuron.weight;
            value = std::tanh(value * drive + neuron.bias);
            
            // Modifiziere Output
            output += value * layer.complexity;
        }
        
        // Normalisiere Output
        output /= (layer.neurons.size() + 1);
        
        buffer[i] = output;
    }
}

void NeuralSaturatorEffect::applyDrive(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = std::tanh(buffer[i] * drive);
    }
}

void NeuralSaturatorEffect::applyBias(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] += bias;
    }
}

void NeuralSaturatorEffect::applyLearning(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float t = static_cast<float>(i) / framesPerBuffer;
        buffer[i] *= (1.0f + learning * std::sin(2.0f * M_PI * t));
    }
}

void NeuralSaturatorEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateLayers();
    
    // Verarbeite jeden Layer
    for (auto& layer : layers) {
        processLayer(layer, buffer, framesPerBuffer);
    }
    
    // Wende Effekte an
    applyDrive(buffer, framesPerBuffer);
    applyBias(buffer, framesPerBuffer);
    applyLearning(buffer, framesPerBuffer);
    
    // Wende Mix an
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = buffer[i] * mix;
    }
}

std::vector<PluginParameter> NeuralSaturatorEffect::getParameters() const {
    return {
        {"drive", drive, 0.0f, 1.0f, ""},
        {"bias", bias, -1.0f, 1.0f, ""},
        {"learning", learning, 0.0f, 1.0f, ""},
        {"adaptation", adaptation, 0.0f, 1.0f, ""},
        {"complexity", complexity, 0.0f, 1.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void NeuralSaturatorEffect::setParameter(const std::string& name, float value) {
    if (name == "drive") drive = value;
    else if (name == "bias") bias = value;
    else if (name == "learning") learning = value;
    else if (name == "adaptation") adaptation = value;
    else if (name == "complexity") complexity = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
}

float NeuralSaturatorEffect::getParameter(const std::string& name) const {
    if (name == "drive") return drive;
    if (name == "bias") return bias;
    if (name == "learning") return learning;
    if (name == "adaptation") return adaptation;
    if (name == "complexity") return complexity;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
    return 0.0f;
}

void NeuralSaturatorEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "drive") automatedDrive = automated;
    else if (name == "bias") automatedBias = automated;
    else if (name == "learning") automatedLearning = automated;
    else if (name == "adaptation") automatedAdaptation = automated;
    else if (name == "complexity") automatedComplexity = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool NeuralSaturatorEffect::isParameterAutomated(const std::string& name) const {
    if (name == "drive") return automatedDrive;
    if (name == "bias") return automatedBias;
    if (name == "learning") return automatedLearning;
    if (name == "adaptation") return automatedAdaptation;
    if (name == "complexity") return automatedComplexity;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    return false;
}

void NeuralSaturatorEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        drive = 0.5f;
        bias = 0.0f;
        learning = 0.5f;
        adaptation = 0.5f;
        complexity = 0.5f;
        mix = 0.5f;
        quality = 1.0f;
    }
    else if (presetName == "Aggressive") {
        drive = 0.8f;
        bias = 0.2f;
        learning = 0.7f;
        adaptation = 0.6f;
        complexity = 0.7f;
        mix = 0.7f;
        quality = 1.0f;
    }
    else if (presetName == "Subtle") {
        drive = 0.3f;
        bias = -0.1f;
        learning = 0.3f;
        adaptation = 0.4f;
        complexity = 0.3f;
        mix = 0.3f;
        quality = 1.0f;
    }
    
    updateLayers();
}

void NeuralSaturatorEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> NeuralSaturatorEffect::getAvailablePresets() const {
    return {"Standard", "Aggressive", "Subtle"};
}

} // namespace VR_DAW 