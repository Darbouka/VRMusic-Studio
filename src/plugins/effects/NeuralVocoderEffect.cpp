#include "NeuralVocoderEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

NeuralVocoderEffect::NeuralVocoderEffect()
    : carrier(0.5f)
    , modulator(0.5f)
    , bands(0.5f)
    , learning(0.5f)
    , adaptation(0.5f)
    , mix(0.5f)
    , quality(1.0f)
    , automatedCarrier(false)
    , automatedModulator(false)
    , automatedBands(false)
    , automatedLearning(false)
    , automatedAdaptation(false)
    , automatedMix(false)
    , automatedQuality(false)
    , bufferSize(44100 * 2) // 2 Sekunden bei 44.1kHz
{
    std::random_device rd;
    rng.seed(rd());
    initializeStates();
}

NeuralVocoderEffect::~NeuralVocoderEffect() {
    shutdown();
}

bool NeuralVocoderEffect::initialize() {
    initializeStates();
    return true;
}

void NeuralVocoderEffect::shutdown() {
    states.clear();
}

void NeuralVocoderEffect::initializeStates() {
    states.clear();
    
    // Erstelle 4 Vocoder-Zustände
    for (int i = 0; i < 4; ++i) {
        VocoderState state;
        
        // Erstelle 16 Bänder pro Vocoder-Zustand
        for (int j = 0; j < 16; ++j) {
            BandState band;
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            
            band.frequency = 20.0f * std::pow(2.0f, j * 0.5f); // Exponentiell verteilte Frequenzen
            band.bandwidth = band.frequency * 0.1f;
            band.gain = dist(rng);
            band.phase = dist(rng) * 2.0f * M_PI;
            
            state.bands.push_back(band);
        }
        
        state.carrier = carrier;
        state.modulator = modulator;
        states.push_back(state);
    }
}

void NeuralVocoderEffect::updateStates() {
    // Aktualisiere Vocoder-Zustände
    for (auto& state : states) {
        state.carrier = carrier;
        state.modulator = modulator;
        
        // Aktualisiere Bänder
        for (auto& band : state.bands) {
            // Aktualisiere Gain basierend auf Lernen
            band.gain += learning * (std::rand() / RAND_MAX - 0.5f) * 0.01f;
            band.gain = std::max(0.0f, std::min(1.0f, band.gain));
            
            // Aktualisiere Phase
            band.phase += band.frequency * 0.0001f;
            band.phase = std::fmod(band.phase, 2.0f * M_PI);
            
            // Aktualisiere Bandbreite basierend auf Anpassung
            band.bandwidth *= (1.0f + adaptation * (std::rand() / RAND_MAX - 0.5f) * 0.01f);
            band.bandwidth = std::max(band.frequency * 0.01f, std::min(band.frequency * 0.5f, band.bandwidth));
        }
    }
}

void NeuralVocoderEffect::processState(VocoderState& state, float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float t = static_cast<float>(i) / framesPerBuffer;
        float output = buffer[i];
        
        // Verarbeite jedes Band
        for (auto& band : state.bands) {
            // Berechne Band-Wert
            float value = output * band.gain;
            value *= std::sin(2.0f * M_PI * band.frequency * t + band.phase);
            
            // Modifiziere Output
            output += value * bands;
        }
        
        // Normalisiere Output
        output /= (state.bands.size() + 1);
        
        buffer[i] = output;
    }
}

void NeuralVocoderEffect::applyCarrier(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float t = static_cast<float>(i) / framesPerBuffer;
        buffer[i] *= (1.0f + carrier * std::sin(2.0f * M_PI * t));
    }
}

void NeuralVocoderEffect::applyModulator(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float t = static_cast<float>(i) / framesPerBuffer;
        buffer[i] *= (1.0f + modulator * std::cos(2.0f * M_PI * t));
    }
}

void NeuralVocoderEffect::applyBands(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] *= bands;
    }
}

void NeuralVocoderEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateStates();
    
    // Verarbeite jeden Vocoder-Zustand
    for (auto& state : states) {
        processState(state, buffer, framesPerBuffer);
    }
    
    // Wende Effekte an
    applyCarrier(buffer, framesPerBuffer);
    applyModulator(buffer, framesPerBuffer);
    applyBands(buffer, framesPerBuffer);
    
    // Wende Mix an
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = buffer[i] * mix;
    }
}

std::vector<PluginParameter> NeuralVocoderEffect::getParameters() const {
    return {
        {"carrier", carrier, 0.0f, 1.0f, ""},
        {"modulator", modulator, 0.0f, 1.0f, ""},
        {"bands", bands, 0.0f, 1.0f, ""},
        {"learning", learning, 0.0f, 1.0f, ""},
        {"adaptation", adaptation, 0.0f, 1.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void NeuralVocoderEffect::setParameter(const std::string& name, float value) {
    if (name == "carrier") carrier = value;
    else if (name == "modulator") modulator = value;
    else if (name == "bands") bands = value;
    else if (name == "learning") learning = value;
    else if (name == "adaptation") adaptation = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
}

float NeuralVocoderEffect::getParameter(const std::string& name) const {
    if (name == "carrier") return carrier;
    if (name == "modulator") return modulator;
    if (name == "bands") return bands;
    if (name == "learning") return learning;
    if (name == "adaptation") return adaptation;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
    return 0.0f;
}

void NeuralVocoderEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "carrier") automatedCarrier = automated;
    else if (name == "modulator") automatedModulator = automated;
    else if (name == "bands") automatedBands = automated;
    else if (name == "learning") automatedLearning = automated;
    else if (name == "adaptation") automatedAdaptation = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool NeuralVocoderEffect::isParameterAutomated(const std::string& name) const {
    if (name == "carrier") return automatedCarrier;
    if (name == "modulator") return automatedModulator;
    if (name == "bands") return automatedBands;
    if (name == "learning") return automatedLearning;
    if (name == "adaptation") return automatedAdaptation;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    return false;
}

void NeuralVocoderEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        carrier = 0.5f;
        modulator = 0.5f;
        bands = 0.5f;
        learning = 0.5f;
        adaptation = 0.5f;
        mix = 0.5f;
        quality = 1.0f;
    }
    else if (presetName == "Robotic") {
        carrier = 0.8f;
        modulator = 0.7f;
        bands = 0.8f;
        learning = 0.7f;
        adaptation = 0.6f;
        mix = 0.7f;
        quality = 1.0f;
    }
    else if (presetName == "Natural") {
        carrier = 0.3f;
        modulator = 0.4f;
        bands = 0.3f;
        learning = 0.3f;
        adaptation = 0.4f;
        mix = 0.4f;
        quality = 1.0f;
    }
    
    updateStates();
}

void NeuralVocoderEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> NeuralVocoderEffect::getAvailablePresets() const {
    return {"Standard", "Robotic", "Natural"};
}

} // namespace VR_DAW 