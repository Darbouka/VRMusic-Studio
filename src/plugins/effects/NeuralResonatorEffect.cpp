#include "NeuralResonatorEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

NeuralResonatorEffect::NeuralResonatorEffect()
    : learningRate(0.1f)
    , resonance(0.5f)
    , complexity(0.5f)
    , evolution(0.3f)
    , mutation(0.2f)
    , mix(0.5f)
    , quality(1.0f)
    , automatedLearningRate(false)
    , automatedResonance(false)
    , automatedComplexity(false)
    , automatedEvolution(false)
    , automatedMutation(false)
    , automatedMix(false)
    , automatedQuality(false)
    , bufferSize(44100 * 2) // 2 Sekunden bei 44.1kHz
{
    std::random_device rd;
    rng.seed(rd());
    initializeNeuralNetwork();
    initializeResonators();
}

NeuralResonatorEffect::~NeuralResonatorEffect() {
    shutdown();
}

bool NeuralResonatorEffect::initialize() {
    initializeNeuralNetwork();
    initializeResonators();
    return true;
}

void NeuralResonatorEffect::shutdown() {
    layers.clear();
    resonators.clear();
}

void NeuralResonatorEffect::initializeNeuralNetwork() {
    layers.clear();
    
    // Erstelle 3 Schichten für die KI-Verarbeitung
    for (int i = 0; i < 3; ++i) {
        NeuralLayer layer;
        layer.weights.resize(64, 0.0f);
        layer.biases.resize(64, 0.0f);
        layer.activations.resize(64, 0.0f);
        
        // Initialisiere Gewichte und Biases
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        for (auto& weight : layer.weights) {
            weight = dist(rng);
        }
        for (auto& bias : layer.biases) {
            bias = dist(rng);
        }
        
        layers.push_back(layer);
    }
}

void NeuralResonatorEffect::initializeResonators() {
    resonators.clear();
    
    // Erstelle 16 Resonatoren für komplexe Klanggenerierung
    for (int i = 0; i < 16; ++i) {
        Resonator resonator;
        resonator.buffer.fill(0.0f);
        resonator.writePos = 0;
        resonator.readPos = 0;
        resonator.frequency = 440.0f * std::pow(2.0f, i / 12.0f); // Harmonische Reihe
        resonator.amplitude = 1.0f;
        resonator.phase = 0.0f;
        resonator.active = true;
        resonators.push_back(resonator);
    }
}

void NeuralResonatorEffect::updateNeuralNetwork() {
    // Aktualisiere Gewichte basierend auf Learning Rate
    if (learningRate != currentLearningRate) {
        for (auto& layer : layers) {
            for (auto& weight : layer.weights) {
                weight *= (1.0f + learningRate * (std::rand() / RAND_MAX - 0.5f));
            }
            for (auto& bias : layer.biases) {
                bias *= (1.0f + learningRate * (std::rand() / RAND_MAX - 0.5f));
            }
        }
        currentLearningRate = learningRate;
    }
}

void NeuralResonatorEffect::updateResonators() {
    // Aktualisiere Resonatoren basierend auf Komplexität
    if (complexity != currentComplexity) {
        for (auto& resonator : resonators) {
            resonator.amplitude = complexity;
        }
        currentComplexity = complexity;
    }
    
    // Wende Evolution an
    if (evolution != currentEvolution) {
        evolveResonators();
        currentEvolution = evolution;
    }
    
    // Wende Mutation an
    if (mutation != currentMutation) {
        mutateResonators();
        currentMutation = mutation;
    }
}

void NeuralResonatorEffect::evolveResonators() {
    // Evolviere Resonatoren basierend auf Evolution-Parameter
    for (auto& resonator : resonators) {
        if (std::rand() / RAND_MAX < evolution) {
            resonator.frequency *= (1.0f + evolution * (std::rand() / RAND_MAX - 0.5f));
            resonator.amplitude *= (1.0f + evolution * (std::rand() / RAND_MAX - 0.5f));
        }
    }
}

void NeuralResonatorEffect::mutateResonators() {
    // Mutiere Resonatoren basierend auf Mutation-Parameter
    for (auto& resonator : resonators) {
        if (std::rand() / RAND_MAX < mutation) {
            resonator.frequency *= (1.0f + mutation * (std::rand() / RAND_MAX - 0.5f));
            resonator.amplitude *= (1.0f + mutation * (std::rand() / RAND_MAX - 0.5f));
            resonator.phase += mutation * (std::rand() / RAND_MAX - 0.5f);
        }
    }
}

float NeuralResonatorEffect::calculateResonance(float input) {
    // Berechne Resonanz basierend auf Eingangssignal
    float resonanceValue = 0.0f;
    for (const auto& layer : layers) {
        for (size_t i = 0; i < layer.weights.size(); ++i) {
            resonanceValue += layer.weights[i] * input + layer.biases[i];
        }
    }
    return std::tanh(resonanceValue * resonance);
}

void NeuralResonatorEffect::processResonator(Resonator& resonator, float* buffer, unsigned long framesPerBuffer) {
    if (!resonator.active) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        // Berechne Resonator-Output
        float phase = resonator.phase + 2.0f * M_PI * resonator.frequency / 44100.0f;
        float output = std::sin(phase) * resonator.amplitude;
        
        // Wende Resonanz an
        output = calculateResonance(output);
        
        // Schreibe in Buffer
        buffer[i] = output;
        
        // Aktualisiere Phase
        resonator.phase = std::fmod(phase, 2.0f * M_PI);
    }
}

void NeuralResonatorEffect::applyNeuralProcessing(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float input = buffer[i];
        
        // Verarbeite durch neuronales Netzwerk
        for (auto& layer : layers) {
            for (size_t j = 0; j < layer.weights.size(); ++j) {
                layer.activations[j] = std::tanh(layer.weights[j] * input + layer.biases[j]);
            }
            input = layer.activations[0]; // Verwende erste Aktivierung als Output
        }
        
        buffer[i] = input;
    }
}

void NeuralResonatorEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateNeuralNetwork();
    updateResonators();
    
    // Verarbeite jeden Resonator
    for (auto& resonator : resonators) {
        processResonator(resonator, buffer, framesPerBuffer);
    }
    
    // Wende neuronale Verarbeitung an
    applyNeuralProcessing(buffer, framesPerBuffer);
    
    // Wende Mix an
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = buffer[i] * mix;
    }
}

std::vector<PluginParameter> NeuralResonatorEffect::getParameters() const {
    return {
        {"learningRate", learningRate, 0.0f, 1.0f, ""},
        {"resonance", resonance, 0.0f, 1.0f, ""},
        {"complexity", complexity, 0.0f, 1.0f, ""},
        {"evolution", evolution, 0.0f, 1.0f, ""},
        {"mutation", mutation, 0.0f, 1.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void NeuralResonatorEffect::setParameter(const std::string& name, float value) {
    if (name == "learningRate") learningRate = value;
    else if (name == "resonance") resonance = value;
    else if (name == "complexity") complexity = value;
    else if (name == "evolution") evolution = value;
    else if (name == "mutation") mutation = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
}

float NeuralResonatorEffect::getParameter(const std::string& name) const {
    if (name == "learningRate") return learningRate;
    if (name == "resonance") return resonance;
    if (name == "complexity") return complexity;
    if (name == "evolution") return evolution;
    if (name == "mutation") return mutation;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
    return 0.0f;
}

void NeuralResonatorEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "learningRate") automatedLearningRate = automated;
    else if (name == "resonance") automatedResonance = automated;
    else if (name == "complexity") automatedComplexity = automated;
    else if (name == "evolution") automatedEvolution = automated;
    else if (name == "mutation") automatedMutation = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool NeuralResonatorEffect::isParameterAutomated(const std::string& name) const {
    if (name == "learningRate") return automatedLearningRate;
    if (name == "resonance") return automatedResonance;
    if (name == "complexity") return automatedComplexity;
    if (name == "evolution") return automatedEvolution;
    if (name == "mutation") return automatedMutation;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    return false;
}

void NeuralResonatorEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        learningRate = 0.1f;
        resonance = 0.5f;
        complexity = 0.5f;
        evolution = 0.3f;
        mutation = 0.2f;
        mix = 0.5f;
        quality = 1.0f;
    }
    else if (presetName == "Heavy") {
        learningRate = 0.3f;
        resonance = 0.7f;
        complexity = 0.8f;
        evolution = 0.5f;
        mutation = 0.4f;
        mix = 0.7f;
        quality = 1.0f;
    }
    else if (presetName == "Subtle") {
        learningRate = 0.05f;
        resonance = 0.3f;
        complexity = 0.3f;
        evolution = 0.1f;
        mutation = 0.1f;
        mix = 0.3f;
        quality = 1.0f;
    }
    
    updateNeuralNetwork();
    updateResonators();
}

void NeuralResonatorEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> NeuralResonatorEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 