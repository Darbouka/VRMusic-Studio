#include "ReverbEffect.hpp"
#include <spdlog/spdlog.h>
#include <random>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace VR_DAW {

ReverbEffect::ReverbEffect()
    : bypassed(false)
    , wetDryMix(0.5f)
    , roomSize(0.5f)
    , damping(0.5f)
    , width(0.5f)
    , stereoWidth(1.0f)
    , preDelay(0.0f)
    , lowCut(20.0f)
    , highCut(20000.0f)
    , diffusion(0.7f)
    , density(0.8f)
    , wetLevel(0.33f)
    , dryLevel(0.4f)
    , freeze(0.0f)
    , mix(0.5f)
    , automatedRoomSize(false)
    , automatedDamping(false)
    , automatedWidth(false)
    , automatedWetLevel(false)
    , automatedDryLevel(false)
    , automatedFreeze(false)
    , automatedMix(false)
{
    generateId();
    name = "Reverb";
    initializeFilters();
}

ReverbEffect::~ReverbEffect() {
    shutdown();
}

void ReverbEffect::generateId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    const char* hex = "0123456789abcdef";
    
    std::stringstream ss;
    for (int i = 0; i < 32; ++i) {
        ss << hex[dis(gen)];
    }
    id = ss.str();
}

bool ReverbEffect::initialize() {
    initializeFilters();
    return true;
}

void ReverbEffect::shutdown() {
    for (auto& filter : combFilters) {
        filter.buffer.clear();
    }
    for (auto& filter : allpassFilters) {
        filter.buffer.clear();
    }
}

void ReverbEffect::update() {
    updateReverbParameters();
}

std::vector<PluginParameter> ReverbEffect::getParameters() const {
    std::vector<PluginParameter> parameters;

    // Wet/Dry Mix
    parameters.push_back({
        "wetDryMix",
        "Wet/Dry Mix",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        0.5f,
        {},
        false,
        [this](float value) { wetDryMix = value; }
    });

    // Room Size
    parameters.push_back({
        "roomSize",
        "Room Size",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        roomSize,
        {},
        automatedRoomSize,
        [this](float value) { roomSize = value; updateBuffers(); }
    });

    // Damping
    parameters.push_back({
        "damping",
        "Damping",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        damping,
        {},
        automatedDamping,
        [this](float value) { damping = value; updateDamping(); }
    });

    // Width
    parameters.push_back({
        "width",
        "Width",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        width,
        {},
        automatedWidth,
        [this](float value) { width = value; }
    });

    // Stereo Width
    parameters.push_back({
        "stereoWidth",
        "Stereo Width",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        1.0f,
        {},
        false,
        [this](float value) { stereoWidth = value; }
    });

    // Pre-delay
    parameters.push_back({
        "preDelay",
        "Pre-delay",
        PluginParameter::Type::Float,
        0.0f,
        100.0f,
        0.0f,
        {},
        false,
        [this](float value) { preDelay = value; }
    });

    // Low Cut
    parameters.push_back({
        "lowCut",
        "Low Cut",
        PluginParameter::Type::Float,
        20.0f,
        1000.0f,
        20.0f,
        {},
        false,
        [this](float value) { lowCut = value; }
    });

    // High Cut
    parameters.push_back({
        "highCut",
        "High Cut",
        PluginParameter::Type::Float,
        1000.0f,
        20000.0f,
        20000.0f,
        {},
        false,
        [this](float value) { highCut = value; }
    });

    // Diffusion
    parameters.push_back({
        "diffusion",
        "Diffusion",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        0.7f,
        {},
        false,
        [this](float value) { diffusion = value; }
    });

    // Density
    parameters.push_back({
        "density",
        "Density",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        0.8f,
        {},
        false,
        [this](float value) { density = value; }
    });

    // Wet Level
    parameters.push_back({
        "wetLevel",
        "Wet Level",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        wetLevel,
        {},
        automatedWetLevel,
        [this](float value) { wetLevel = value; }
    });

    // Dry Level
    parameters.push_back({
        "dryLevel",
        "Dry Level",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        dryLevel,
        {},
        automatedDryLevel,
        [this](float value) { dryLevel = value; }
    });

    // Freeze
    parameters.push_back({
        "freeze",
        "Freeze",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        freeze,
        {},
        automatedFreeze,
        [this](float value) { freeze = value; }
    });

    // Mix
    parameters.push_back({
        "mix",
        "Mix",
        PluginParameter::Type::Float,
        0.0f,
        1.0f,
        mix,
        {},
        automatedMix,
        [this](float value) { mix = value; }
    });

    return parameters;
}

void ReverbEffect::setParameter(const std::string& name, float value) {
    if (name == "wetDryMix") wetDryMix = value;
    else if (name == "roomSize") {
        roomSize = value;
        updateBuffers();
    }
    else if (name == "damping") {
        damping = value;
        for (auto& filter : combFilters) {
            filter.damping = damping;
            filter.dampInverse = 1.0f - damping;
        }
    }
    else if (name == "width") width = value;
    else if (name == "stereoWidth") stereoWidth = value;
    else if (name == "preDelay") preDelay = value;
    else if (name == "lowCut") lowCut = value;
    else if (name == "highCut") highCut = value;
    else if (name == "diffusion") diffusion = value;
    else if (name == "density") density = value;
    else if (name == "wetLevel") wetLevel = value;
    else if (name == "dryLevel") dryLevel = value;
    else if (name == "freeze") freeze = value;
    else if (name == "mix") mix = value;
}

float ReverbEffect::getParameter(const std::string& name) const {
    if (name == "wetDryMix") return wetDryMix;
    if (name == "roomSize") return roomSize;
    if (name == "damping") return damping;
    if (name == "width") return width;
    if (name == "stereoWidth") return stereoWidth;
    if (name == "preDelay") return preDelay;
    if (name == "lowCut") return lowCut;
    if (name == "highCut") return highCut;
    if (name == "diffusion") return diffusion;
    if (name == "density") return density;
    if (name == "wetLevel") return wetLevel;
    if (name == "dryLevel") return dryLevel;
    if (name == "freeze") return freeze;
    if (name == "mix") return mix;
    return 0.0f;
}

void ReverbEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "roomSize") automatedRoomSize = automated;
    else if (name == "damping") automatedDamping = automated;
    else if (name == "width") automatedWidth = automated;
    else if (name == "wetLevel") automatedWetLevel = automated;
    else if (name == "dryLevel") automatedDryLevel = automated;
    else if (name == "freeze") automatedFreeze = automated;
    else if (name == "mix") automatedMix = automated;
}

bool ReverbEffect::isParameterAutomated(const std::string& name) const {
    if (name == "roomSize") return automatedRoomSize;
    if (name == "damping") return automatedDamping;
    if (name == "width") return automatedWidth;
    if (name == "wetLevel") return automatedWetLevel;
    if (name == "dryLevel") return automatedDryLevel;
    if (name == "freeze") return automatedFreeze;
    if (name == "mix") return automatedMix;
    return false;
}

void ReverbEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    if (bypassed) return;

    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // Comb-Filter-Verarbeitung
        float combSum = 0.0f;
        for (auto& filter : combFilters) {
            combSum += processComb(left, filter);
        }
        
        // Allpass-Filter-Verarbeitung
        float allpassSum = combSum;
        for (auto& filter : allpassFilters) {
            allpassSum = processAllpass(allpassSum, filter);
        }
        
        // Stereo-Width
        float mid = allpassSum;
        float side = allpassSum * width;
        
        // Mix
        float wetLeft = mid + side;
        float wetRight = mid - side;
        
        // Freeze-Modus
        if (freeze > 0.0f) {
            wetLeft *= (1.0f - freeze);
            wetRight *= (1.0f - freeze);
        }
        
        // Final Mix
        buffer[i] = left * dryLevel * (1.0f - mix) + wetLeft * wetLevel * mix;
        buffer[i + 1] = right * dryLevel * (1.0f - mix) + wetRight * wetLevel * mix;
    }
}

void ReverbEffect::processMidi(const std::vector<uint8_t>& midiData) {
    // MIDI-Verarbeitung für den Reverb-Effekt
}

void ReverbEffect::setAutomationPoint(const std::string& parameter, double time, float value) {
    // Hier würde ein Automation-Punkt für den Parameter gesetzt werden
}

void ReverbEffect::removeAutomationPoint(const std::string& parameter, double time) {
    // Hier würde ein Automation-Punkt für den Parameter entfernt werden
}

void ReverbEffect::clearAutomation(const std::string& parameter) {
    // Hier würde die Automation für den Parameter gelöscht werden
}

std::vector<std::pair<double, float>> ReverbEffect::getAutomationPoints(const std::string& parameter) const {
    // Hier würden die Automation-Punkte für den Parameter zurückgegeben werden
    return {};
}

void ReverbEffect::savePreset(const std::string& name) {
    // Hier würde das Preset gespeichert werden
}

void ReverbEffect::loadPreset(const std::string& name) {
    // Hier würde das Preset geladen werden
}

std::vector<std::string> ReverbEffect::getPresetList() const {
    // Hier würde die Liste der verfügbaren Presets zurückgegeben werden
    return {};
}

void ReverbEffect::setUIPosition(const glm::vec3& position) {
    // Hier würde die UI-Position gesetzt werden
}

void ReverbEffect::setUIOrientation(const glm::quat& orientation) {
    // Hier würde die UI-Ausrichtung gesetzt werden
}

void ReverbEffect::setUIScale(const glm::vec3& scale) {
    // Hier würde die UI-Skalierung gesetzt werden
}

void ReverbEffect::renderUI() {
    // Hier würde die UI gerendert werden
}

void ReverbEffect::setBypass(bool bypass) {
    bypassed = bypass;
}

bool ReverbEffect::isBypassed() const {
    return bypassed;
}

void ReverbEffect::setWetDryMix(float mix) {
    wetDryMix = std::max(0.0f, std::min(1.0f, mix));
}

float ReverbEffect::getWetDryMix() const {
    return wetDryMix;
}

void ReverbEffect::updateReverbParameters() {
    // Hier würden die Reverb-Parameter basierend auf den UI-Einstellungen aktualisiert werden
}

void ReverbEffect::updateBuffers() {
    // Comb-Filter-Längen basierend auf Room-Size aktualisieren
    const size_t baseLengths[] = {
        1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617
    };
    
    for (size_t i = 0; i < combFilters.size(); ++i) {
        size_t newLength = static_cast<size_t>(baseLengths[i] * (0.5f + roomSize));
        if (newLength != combFilters[i].buffer.size()) {
            combFilters[i].buffer.resize(newLength, 0.0f);
            combFilters[i].position = 0;
        }
    }
    
    for (auto& filter : allpassFilters) {
        filter.feedback = 0.5f * (1.0f + (roomSize * 0.5f));
    }
}

float ReverbEffect::processComb(float input, CombFilter& filter) {
    float output = filter.buffer[filter.position];
    
    // Damping
    filter.buffer[filter.position] = input + output * filter.feedback;
    filter.buffer[filter.position] = filter.buffer[filter.position] * filter.damping + 
                                   filter.buffer[filter.position] * filter.dampInverse;
    
    // Position aktualisieren
    filter.position = (filter.position + 1) % filter.buffer.size();
    
    return output;
}

float ReverbEffect::processAllpass(float input, AllpassFilter& filter) {
    float output = filter.buffer[filter.position];
    
    // Allpass-Verarbeitung
    filter.buffer[filter.position] = input + output * filter.feedback;
    
    // Position aktualisieren
    filter.position = (filter.position + 1) % filter.buffer.size();
    
    return output - input;
}

void ReverbEffect::updateDamping() {
    float damp = damping * 0.4f + 0.2f;  // 0.2 bis 0.6
    for (auto& filter : combFilters) {
        filter.damping = damp;
        filter.dampInverse = 1.0f - damp;
    }
}

void ReverbEffect::initializeFilters() {
    // Comb-Filter-Längen (in Samples bei 44.1kHz)
    const size_t combLengths[] = {
        1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617
    };
    
    // Allpass-Filter-Längen
    const size_t allpassLengths[] = {
        556, 441, 341, 225
    };
    
    // Comb-Filter initialisieren
    combFilters.clear();
    for (size_t length : combLengths) {
        CombFilter filter;
        filter.buffer.resize(length, 0.0f);
        filter.position = 0;
        filter.feedback = 0.5f;
        filter.damping = damping;
        filter.dampInverse = 1.0f - damping;
        combFilters.push_back(filter);
    }
    
    // Allpass-Filter initialisieren
    allpassFilters.clear();
    for (size_t length : allpassLengths) {
        AllpassFilter filter;
        filter.buffer.resize(length, 0.0f);
        filter.position = 0;
        filter.feedback = 0.5f;
        allpassFilters.push_back(filter);
    }
}

void ReverbEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        roomSize = 0.5f;
        damping = 0.5f;
        width = 0.5f;
        wetLevel = 0.33f;
        dryLevel = 0.4f;
        freeze = 0.0f;
        mix = 0.5f;
    }
    else if (presetName == "Hall") {
        roomSize = 0.8f;
        damping = 0.3f;
        width = 0.7f;
        wetLevel = 0.4f;
        dryLevel = 0.3f;
        freeze = 0.0f;
        mix = 0.7f;
    }
    else if (presetName == "Room") {
        roomSize = 0.3f;
        damping = 0.7f;
        width = 0.3f;
        wetLevel = 0.25f;
        dryLevel = 0.5f;
        freeze = 0.0f;
        mix = 0.3f;
    }
    
    // Filter aktualisieren
    for (auto& filter : combFilters) {
        filter.damping = damping;
        filter.dampInverse = 1.0f - damping;
    }
    updateBuffers();
}

void ReverbEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> ReverbEffect::getAvailablePresets() const {
    return {"Standard", "Hall", "Room"};
}

} // namespace VR_DAW 