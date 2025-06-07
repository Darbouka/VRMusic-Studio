#include "VoiceEditor.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

VoiceEditor::VoiceEditor()
    : pitch(1.0f)
    , formant(1.0f)
    , breathiness(0.0f)
    , clarity(1.0f)
    , warmth(0.5f)
    , presence(0.5f)
    , mix(0.5f)
    , quality(1.0f)
    , automatedPitch(false)
    , automatedFormant(false)
    , automatedBreathiness(false)
    , automatedClarity(false)
    , automatedWarmth(false)
    , automatedPresence(false)
    , automatedMix(false)
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
    
    initializeVoiceSettings();
}

VoiceEditor::~VoiceEditor() {
    shutdown();
}

bool VoiceEditor::initialize() {
    initializeVoiceSettings();
    return true;
}

void VoiceEditor::shutdown() {
    voiceSettings.clear();
}

void VoiceEditor::initializeVoiceSettings() {
    // Initialisiere Standard-Stimmeinstellungen
    std::map<std::string, float> standardSettings = {
        {"pitch", 1.0f},
        {"formant", 1.0f},
        {"breathiness", 0.0f},
        {"clarity", 1.0f},
        {"warmth", 0.5f},
        {"presence", 0.5f}
    };
    voiceSettings["Standard"] = standardSettings;
    
    // Tom Hanks DE
    std::map<std::string, float> tomHanksSettings = {
        {"pitch", 1.1f},
        {"formant", 1.1f},
        {"breathiness", 0.2f},
        {"clarity", 0.9f},
        {"warmth", 0.7f},
        {"presence", 0.6f}
    };
    voiceSettings["Tom Hanks DE"] = tomHanksSettings;
    
    // Morgan Freeman DE
    std::map<std::string, float> morganFreemanSettings = {
        {"pitch", 0.9f},
        {"formant", 0.9f},
        {"breathiness", 0.1f},
        {"clarity", 0.8f},
        {"warmth", 0.8f},
        {"presence", 0.7f}
    };
    voiceSettings["Morgan Freeman DE"] = morganFreemanSettings;
    
    // Scarlett Johansson DE
    std::map<std::string, float> scarlettSettings = {
        {"pitch", 1.2f},
        {"formant", 1.2f},
        {"breathiness", 0.3f},
        {"clarity", 1.0f},
        {"warmth", 0.6f},
        {"presence", 0.8f}
    };
    voiceSettings["Scarlett Johansson DE"] = scarlettSettings;
    
    // Leonardo DiCaprio DE
    std::map<std::string, float> leonardoSettings = {
        {"pitch", 1.0f},
        {"formant", 1.0f},
        {"breathiness", 0.2f},
        {"clarity", 0.9f},
        {"warmth", 0.7f},
        {"presence", 0.7f}
    };
    voiceSettings["Leonardo DiCaprio DE"] = leonardoSettings;
    
    // Meryl Streep DE
    std::map<std::string, float> merylSettings = {
        {"pitch", 1.1f},
        {"formant", 1.1f},
        {"breathiness", 0.1f},
        {"clarity", 0.9f},
        {"warmth", 0.7f},
        {"presence", 0.6f}
    };
    voiceSettings["Meryl Streep DE"] = merylSettings;
    
    // Robert De Niro DE
    std::map<std::string, float> robertSettings = {
        {"pitch", 0.9f},
        {"formant", 0.9f},
        {"breathiness", 0.2f},
        {"clarity", 0.8f},
        {"warmth", 0.8f},
        {"presence", 0.7f}
    };
    voiceSettings["Robert De Niro DE"] = robertSettings;
    
    // Emma Watson DE
    std::map<std::string, float> emmaSettings = {
        {"pitch", 1.2f},
        {"formant", 1.2f},
        {"breathiness", 0.3f},
        {"clarity", 1.0f},
        {"warmth", 0.6f},
        {"presence", 0.8f}
    };
    voiceSettings["Emma Watson DE"] = emmaSettings;
    
    // Brad Pitt DE
    std::map<std::string, float> bradSettings = {
        {"pitch", 1.0f},
        {"formant", 1.0f},
        {"breathiness", 0.2f},
        {"clarity", 0.9f},
        {"warmth", 0.7f},
        {"presence", 0.7f}
    };
    voiceSettings["Brad Pitt DE"] = bradSettings;
    
    // Jennifer Lawrence DE
    std::map<std::string, float> jenniferSettings = {
        {"pitch", 1.1f},
        {"formant", 1.1f},
        {"breathiness", 0.3f},
        {"clarity", 1.0f},
        {"warmth", 0.6f},
        {"presence", 0.8f}
    };
    voiceSettings["Jennifer Lawrence DE"] = jenniferSettings;
}

void VoiceEditor::updateVoiceSettings() {
    // Aktualisiere Einstellungen basierend auf aktueller Stimme
    if (voiceSettings.find(currentVoice) != voiceSettings.end()) {
        const auto& settings = voiceSettings[currentVoice];
        pitch = settings.at("pitch");
        formant = settings.at("formant");
        breathiness = settings.at("breathiness");
        clarity = settings.at("clarity");
        warmth = settings.at("warmth");
        presence = settings.at("presence");
    }
}

void VoiceEditor::processVoice(float* buffer, unsigned long framesPerBuffer) {
    // Wende Pitch-Shifting an
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] *= pitch;
    }
    
    // Wende Formant-Verschiebung an
    applyFormantSettings(buffer, framesPerBuffer);
    
    // Wende EQ-Einstellungen an
    applyEQSettings(buffer, framesPerBuffer);
}

void VoiceEditor::applyFormantSettings(float* buffer, unsigned long framesPerBuffer) {
    // Implementiere Formant-Verschiebung
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] *= formant;
    }
}

void VoiceEditor::applyEQSettings(float* buffer, unsigned long framesPerBuffer) {
    // Implementiere EQ-Einstellungen basierend auf clarity, warmth und presence
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float sample = buffer[i];
        
        // Wende Clarity an (Höhen)
        sample *= (1.0f + clarity * 0.5f);
        
        // Wende Warmth an (Mitten)
        sample *= (1.0f + warmth * 0.3f);
        
        // Wende Presence an (Präsenz)
        sample *= (1.0f + presence * 0.4f);
        
        // Wende Breathiness an
        if (breathiness > 0.0f) {
            float noise = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
            sample = sample * (1.0f - breathiness) + noise * breathiness;
        }
        
        buffer[i] = sample;
    }
}

void VoiceEditor::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateVoiceSettings();
    processVoice(buffer, framesPerBuffer);
}

std::vector<PluginParameter> VoiceEditor::getParameters() const {
    return {
        {"pitch", pitch, 0.5f, 2.0f, ""},
        {"formant", formant, 0.5f, 2.0f, ""},
        {"breathiness", breathiness, 0.0f, 1.0f, ""},
        {"clarity", clarity, 0.0f, 1.0f, ""},
        {"warmth", warmth, 0.0f, 1.0f, ""},
        {"presence", presence, 0.0f, 1.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void VoiceEditor::setParameter(const std::string& name, float value) {
    if (name == "pitch") pitch = value;
    else if (name == "formant") formant = value;
    else if (name == "breathiness") breathiness = value;
    else if (name == "clarity") clarity = value;
    else if (name == "warmth") warmth = value;
    else if (name == "presence") presence = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
}

float VoiceEditor::getParameter(const std::string& name) const {
    if (name == "pitch") return pitch;
    if (name == "formant") return formant;
    if (name == "breathiness") return breathiness;
    if (name == "clarity") return clarity;
    if (name == "warmth") return warmth;
    if (name == "presence") return presence;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
    return 0.0f;
}

void VoiceEditor::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "pitch") automatedPitch = automated;
    else if (name == "formant") automatedFormant = automated;
    else if (name == "breathiness") automatedBreathiness = automated;
    else if (name == "clarity") automatedClarity = automated;
    else if (name == "warmth") automatedWarmth = automated;
    else if (name == "presence") automatedPresence = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool VoiceEditor::isParameterAutomated(const std::string& name) const {
    if (name == "pitch") return automatedPitch;
    if (name == "formant") return automatedFormant;
    if (name == "breathiness") return automatedBreathiness;
    if (name == "clarity") return automatedClarity;
    if (name == "warmth") return automatedWarmth;
    if (name == "presence") return automatedPresence;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    return false;
}

void VoiceEditor::setVoice(const std::string& voiceName) {
    if (std::find(availableVoices.begin(), availableVoices.end(), voiceName) != availableVoices.end()) {
        currentVoice = voiceName;
        updateVoiceSettings();
    }
}

std::string VoiceEditor::getCurrentVoice() const {
    return currentVoice;
}

std::vector<std::string> VoiceEditor::getAvailableVoices() const {
    return availableVoices;
}

void VoiceEditor::applyVoiceSettings(const std::string& voiceName) {
    setVoice(voiceName);
}

void VoiceEditor::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        pitch = 1.0f;
        formant = 1.0f;
        breathiness = 0.0f;
        clarity = 1.0f;
        warmth = 0.5f;
        presence = 0.5f;
        mix = 0.5f;
        quality = 1.0f;
        currentVoice = "Standard";
    }
    else if (presetName == "Heavy") {
        pitch = 1.2f;
        formant = 1.2f;
        breathiness = 0.3f;
        clarity = 1.0f;
        warmth = 0.7f;
        presence = 0.8f;
        mix = 0.7f;
        quality = 1.0f;
        currentVoice = "Standard";
    }
    else if (presetName == "Subtle") {
        pitch = 0.9f;
        formant = 0.9f;
        breathiness = 0.1f;
        clarity = 0.8f;
        warmth = 0.6f;
        presence = 0.4f;
        mix = 0.3f;
        quality = 1.0f;
        currentVoice = "Standard";
    }
    
    updateVoiceSettings();
}

void VoiceEditor::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> VoiceEditor::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 