#include "VoiceVocoderEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

VoiceVocoderEffect::VoiceVocoderEffect()
    : bands(16.0f)
    , mix(0.5f)
    , formant(1.0f)
    , robotize(0.0f)
    , quality(1.0f)
    , automatedBands(false)
    , automatedMix(false)
    , automatedFormant(false)
    , automatedRobotize(false)
    , automatedQuality(false)
    , currentVoice("Standard")
{
    // Initialisiere verfügbare Stimmen
    availableVoices = {
        "Standard",
        "Tom Hanks DE",
        "Morgan Freeman DE",
        "Scarlett Johansson DE",
        "Leonardo DiCaprio DE",
        "Meryl Streep DE",
        "Robert De Niro DE",
        "Emma Watson DE",
        "Brad Pitt DE",
        "Jennifer Lawrence DE"
    };
    
    initializeBands();
}

VoiceVocoderEffect::~VoiceVocoderEffect() {
    shutdown();
}

bool VoiceVocoderEffect::initialize() {
    initializeBands();
    return true;
}

void VoiceVocoderEffect::shutdown() {
    bandsState.clear();
}

void VoiceVocoderEffect::initializeBands() {
    // Initialisiere Filterbänder
    int numBands = static_cast<int>(bands);
    bandsState.resize(numBands);
    
    for (int i = 0; i < numBands; ++i) {
        Band band;
        band.carrier = 0.0f;
        band.modulator = 0.0f;
        band.envelope = 0.0f;
        bandsState[i] = band;
    }
}

void VoiceVocoderEffect::updateBands() {
    // Aktualisiere Filterbänder basierend auf aktuellen Einstellungen
    int numBands = static_cast<int>(bands);
    if (numBands != bandsState.size()) {
        initializeBands();
    }
}

void VoiceVocoderEffect::processBands(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float output = 0.0f;
        
        // Verarbeite jedes Band
        for (auto& band : bandsState) {
            // Berechne Hüllkurve
            float input = buffer[i];
            band.envelope = std::max(std::abs(input), band.envelope * 0.99f);
            
            // Wende Formant-Verschiebung an
            float formantShift = formant;
            if (robotize > 0.0f) {
                formantShift *= (1.0f + robotize);
            }
            
            // Moduliere Carrier mit Modulator
            float modulated = band.carrier * band.envelope * formantShift;
            output += modulated;
        }
        
        // Normalisiere Ausgang
        output /= bandsState.size();
        
        // Mix mit Original-Signal
        buffer[i] = buffer[i] * (1.0f - mix) + output * mix;
    }
}

void VoiceVocoderEffect::applyVoicePreset(float* buffer, unsigned long framesPerBuffer) {
    // Wende spezifische Einstellungen für die ausgewählte Stimme an
    if (currentVoice == "Tom Hanks DE") {
        formant = 1.1f;
        robotize = 0.2f;
    }
    else if (currentVoice == "Morgan Freeman DE") {
        formant = 0.9f;
        robotize = 0.1f;
    }
    else if (currentVoice == "Scarlett Johansson DE") {
        formant = 1.2f;
        robotize = 0.3f;
    }
    else if (currentVoice == "Leonardo DiCaprio DE") {
        formant = 1.0f;
        robotize = 0.2f;
    }
    else if (currentVoice == "Meryl Streep DE") {
        formant = 1.1f;
        robotize = 0.1f;
    }
    else if (currentVoice == "Robert De Niro DE") {
        formant = 0.9f;
        robotize = 0.2f;
    }
    else if (currentVoice == "Emma Watson DE") {
        formant = 1.2f;
        robotize = 0.3f;
    }
    else if (currentVoice == "Brad Pitt DE") {
        formant = 1.0f;
        robotize = 0.2f;
    }
    else if (currentVoice == "Jennifer Lawrence DE") {
        formant = 1.1f;
        robotize = 0.3f;
    }
    
    // Verarbeite Audio mit aktualisierten Einstellungen
    processBands(buffer, framesPerBuffer);
}

void VoiceVocoderEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateBands();
    applyVoicePreset(buffer, framesPerBuffer);
}

std::vector<PluginParameter> VoiceVocoderEffect::getParameters() const {
    return {
        {"bands", bands, 4.0f, 32.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"formant", formant, 0.5f, 2.0f, ""},
        {"robotize", robotize, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void VoiceVocoderEffect::setParameter(const std::string& name, float value) {
    if (name == "bands") {
        bands = value;
        updateBands();
    }
    else if (name == "mix") mix = value;
    else if (name == "formant") formant = value;
    else if (name == "robotize") robotize = value;
    else if (name == "quality") quality = value;
}

float VoiceVocoderEffect::getParameter(const std::string& name) const {
    if (name == "bands") return bands;
    if (name == "mix") return mix;
    if (name == "formant") return formant;
    if (name == "robotize") return robotize;
    if (name == "quality") return quality;
    return 0.0f;
}

void VoiceVocoderEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "bands") automatedBands = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "formant") automatedFormant = automated;
    else if (name == "robotize") automatedRobotize = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool VoiceVocoderEffect::isParameterAutomated(const std::string& name) const {
    if (name == "bands") return automatedBands;
    if (name == "mix") return automatedMix;
    if (name == "formant") return automatedFormant;
    if (name == "robotize") return automatedRobotize;
    if (name == "quality") return automatedQuality;
    return false;
}

void VoiceVocoderEffect::setVoicePreset(const std::string& voiceName) {
    if (std::find(availableVoices.begin(), availableVoices.end(), voiceName) != availableVoices.end()) {
        currentVoice = voiceName;
    }
}

std::string VoiceVocoderEffect::getVoicePreset() const {
    return currentVoice;
}

std::vector<std::string> VoiceVocoderEffect::getAvailableVoices() const {
    return availableVoices;
}

void VoiceVocoderEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        bands = 16.0f;
        mix = 0.5f;
        formant = 1.0f;
        robotize = 0.0f;
        quality = 1.0f;
        currentVoice = "Standard";
    }
    else if (presetName == "Heavy") {
        bands = 32.0f;
        mix = 0.7f;
        formant = 1.5f;
        robotize = 0.5f;
        quality = 1.0f;
        currentVoice = "Standard";
    }
    else if (presetName == "Subtle") {
        bands = 8.0f;
        mix = 0.3f;
        formant = 0.8f;
        robotize = 0.1f;
        quality = 1.0f;
        currentVoice = "Standard";
    }
    
    updateBands();
}

void VoiceVocoderEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> VoiceVocoderEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 