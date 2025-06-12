#include "MelodyToVocals.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>

namespace VRMusicStudio {

struct MelodyToVocals::Impl {
    // Plugin-Parameter
    Parameters params;
    VocalMode currentMode;
    std::string currentModel;
    std::vector<std::string> currentLyrics;
    std::string currentEmotion;

    // Audio-Parameter
    double sampleRate;
    int bufferSize;
    int numChannels;

    // KI-Modelle
    torch::jit::script::Module vocalModel;
    torch::jit::script::Module emotionModel;
    torch::jit::script::Module articulationModel;

    // Zustandsvariablen
    bool isInitialized;
    float currentPitch;
    float currentFormant;
    float currentBreath;
    float currentVibrato;
    float currentResonance;

    // Puffer
    std::vector<float> inputBuffer;
    std::vector<float> outputBuffer;
    std::vector<float> formantBuffer;
    std::vector<float> emotionBuffer;

    Impl() : isInitialized(false), currentPitch(0.0f), currentFormant(0.0f),
             currentBreath(0.0f), currentVibrato(0.0f), currentResonance(0.0f) {
        // Initialisiere Parameter
        params = {
            0.5f,  // vocalIntensity
            0.3f,  // breathiness
            0.2f,  // vibrato
            0.0f,  // formantShift
            0.5f,  // pitchCorrection
            0.5f,  // emotion
            0.5f,  // articulation
            0.5f   // resonance
        };
        currentMode = VocalMode::Natural;
    }
};

MelodyToVocals::MelodyToVocals() : pImpl(std::make_unique<Impl>()) {
    try {
        // Lade KI-Modelle
        pImpl->vocalModel = torch::jit::load("models/vocal_transformer.pt");
        pImpl->emotionModel = torch::jit::load("models/emotion_transformer.pt");
        pImpl->articulationModel = torch::jit::load("models/articulation_transformer.pt");
    } catch (const c10::Error& e) {
        spdlog::error("Fehler beim Laden der KI-Modelle: {}", e.what());
    }
}

MelodyToVocals::~MelodyToVocals() = default;

void MelodyToVocals::prepareToPlay(double sampleRate, int samplesPerBlock) {
    pImpl->sampleRate = sampleRate;
    pImpl->bufferSize = samplesPerBlock;
    pImpl->numChannels = 2;

    // Initialisiere Puffer
    pImpl->inputBuffer.resize(samplesPerBlock * 2);
    pImpl->outputBuffer.resize(samplesPerBlock * 2);
    pImpl->formantBuffer.resize(samplesPerBlock * 2);
    pImpl->emotionBuffer.resize(samplesPerBlock * 2);

    pImpl->isInitialized = true;
}

void MelodyToVocals::releaseResources() {
    pImpl->inputBuffer.clear();
    pImpl->outputBuffer.clear();
    pImpl->formantBuffer.clear();
    pImpl->emotionBuffer.clear();
    pImpl->isInitialized = false;
}

void MelodyToVocals::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    if (!pImpl->isInitialized) return;

    // Kopiere Input in Puffer
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        std::copy(buffer.getReadPointer(channel),
                 buffer.getReadPointer(channel) + buffer.getNumSamples(),
                 pImpl->inputBuffer.begin() + channel * buffer.getNumSamples());
    }

    // Verarbeite Vokalisierung
    processVocalization(buffer);

    // Kopiere Output zurück
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        std::copy(pImpl->outputBuffer.begin() + channel * buffer.getNumSamples(),
                 pImpl->outputBuffer.begin() + (channel + 1) * buffer.getNumSamples(),
                 buffer.getWritePointer(channel));
    }
}

void MelodyToVocals::processVocalization(juce::AudioBuffer<float>& buffer) {
    try {
        // Erstelle Input-Tensor
        std::vector<torch::jit::IValue> inputs;
        auto inputTensor = torch::from_blob(pImpl->inputBuffer.data(),
                                          {1, buffer.getNumSamples(), 2},
                                          torch::kFloat32);
        inputs.push_back(inputTensor);

        // Füge Parameter hinzu
        auto params = torch::tensor({
            pImpl->params.vocalIntensity,
            pImpl->params.breathiness,
            pImpl->params.vibrato,
            pImpl->params.formantShift,
            pImpl->params.pitchCorrection,
            pImpl->params.emotion,
            pImpl->params.articulation,
            pImpl->params.resonance
        });
        inputs.push_back(params);

        // Führe Vokalisierung durch
        auto output = pImpl->vocalModel.forward(inputs).toTensor();
        
        // Kopiere Ergebnis in Output-Puffer
        std::memcpy(pImpl->outputBuffer.data(),
                   output.data_ptr<float>(),
                   buffer.getNumSamples() * 2 * sizeof(float));

        // Wende Emotion an
        applyEmotion(pImpl->outputBuffer.data(), buffer.getNumSamples());

        // Wende Artikulation an
        applyArticulation(pImpl->outputBuffer.data(), buffer.getNumSamples());

    } catch (const c10::Error& e) {
        spdlog::error("Fehler bei der Vokalisierung: {}", e.what());
    }
}

void MelodyToVocals::applyEmotion(float* buffer, int numSamples) {
    try {
        // Erstelle Input-Tensor für Emotion
        std::vector<torch::jit::IValue> inputs;
        auto inputTensor = torch::from_blob(buffer,
                                          {1, numSamples, 2},
                                          torch::kFloat32);
        inputs.push_back(inputTensor);

        // Füge Emotions-Parameter hinzu
        auto emotionParams = torch::tensor({
            pImpl->params.emotion,
            static_cast<float>(pImpl->currentMode)
        });
        inputs.push_back(emotionParams);

        // Führe Emotions-Transformation durch
        auto output = pImpl->emotionModel.forward(inputs).toTensor();
        
        // Kopiere Ergebnis zurück
        std::memcpy(buffer,
                   output.data_ptr<float>(),
                   numSamples * 2 * sizeof(float));

    } catch (const c10::Error& e) {
        spdlog::error("Fehler bei der Emotions-Verarbeitung: {}", e.what());
    }
}

void MelodyToVocals::applyArticulation(float* buffer, int numSamples) {
    try {
        // Erstelle Input-Tensor für Artikulation
        std::vector<torch::jit::IValue> inputs;
        auto inputTensor = torch::from_blob(buffer,
                                          {1, numSamples, 2},
                                          torch::kFloat32);
        inputs.push_back(inputTensor);

        // Füge Artikulations-Parameter hinzu
        auto articulationParams = torch::tensor({
            pImpl->params.articulation,
            pImpl->currentFormant,
            pImpl->currentResonance
        });
        inputs.push_back(articulationParams);

        // Führe Artikulations-Transformation durch
        auto output = pImpl->articulationModel.forward(inputs).toTensor();
        
        // Kopiere Ergebnis zurück
        std::memcpy(buffer,
                   output.data_ptr<float>(),
                   numSamples * 2 * sizeof(float));

    } catch (const c10::Error& e) {
        spdlog::error("Fehler bei der Artikulations-Verarbeitung: {}", e.what());
    }
}

void MelodyToVocals::setParameters(const Parameters& params) {
    pImpl->params = params;
}

void MelodyToVocals::setVocalMode(VocalMode mode) {
    pImpl->currentMode = mode;
}

void MelodyToVocals::setVocalModel(const std::string& modelPath) {
    try {
        pImpl->vocalModel = torch::jit::load(modelPath);
        pImpl->currentModel = modelPath;
    } catch (const c10::Error& e) {
        spdlog::error("Fehler beim Laden des Vokal-Modells: {}", e.what());
    }
}

void MelodyToVocals::setLyrics(const std::vector<std::string>& lyrics) {
    pImpl->currentLyrics = lyrics;
}

void MelodyToVocals::setEmotion(const std::string& emotion) {
    pImpl->currentEmotion = emotion;
}

juce::AudioProcessorEditor* MelodyToVocals::createEditor() {
    return new juce::GenericAudioProcessorEditor(*this);
}

void MelodyToVocals::getStateInformation(juce::MemoryBlock& destData) {
    // Speichere Plugin-Zustand
    juce::ValueTree state("MelodyToVocals");
    state.setProperty("vocalIntensity", pImpl->params.vocalIntensity, nullptr);
    state.setProperty("breathiness", pImpl->params.breathiness, nullptr);
    state.setProperty("vibrato", pImpl->params.vibrato, nullptr);
    state.setProperty("formantShift", pImpl->params.formantShift, nullptr);
    state.setProperty("pitchCorrection", pImpl->params.pitchCorrection, nullptr);
    state.setProperty("emotion", pImpl->params.emotion, nullptr);
    state.setProperty("articulation", pImpl->params.articulation, nullptr);
    state.setProperty("resonance", pImpl->params.resonance, nullptr);
    state.setProperty("mode", static_cast<int>(pImpl->currentMode), nullptr);
    state.setProperty("model", pImpl->currentModel, nullptr);
    state.setProperty("emotion", pImpl->currentEmotion, nullptr);

    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MelodyToVocals::setStateInformation(const void* data, int sizeInBytes) {
    // Lade Plugin-Zustand
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr) {
        juce::ValueTree state = juce::ValueTree::fromXml(*xmlState);
        pImpl->params.vocalIntensity = state.getProperty("vocalIntensity");
        pImpl->params.breathiness = state.getProperty("breathiness");
        pImpl->params.vibrato = state.getProperty("vibrato");
        pImpl->params.formantShift = state.getProperty("formantShift");
        pImpl->params.pitchCorrection = state.getProperty("pitchCorrection");
        pImpl->params.emotion = state.getProperty("emotion");
        pImpl->params.articulation = state.getProperty("articulation");
        pImpl->params.resonance = state.getProperty("resonance");
        pImpl->currentMode = static_cast<VocalMode>(state.getProperty("mode"));
        pImpl->currentModel = state.getProperty("model").toString();
        pImpl->currentEmotion = state.getProperty("emotion").toString();
    }
}

} // namespace VRMusicStudio 