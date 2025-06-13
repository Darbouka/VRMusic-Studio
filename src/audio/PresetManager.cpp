#include "PresetManager.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace VRMusicStudio {

PresetManager::PresetManager() {
    initialize();
}

PresetManager::~PresetManager() {
    saveAllPresets();
}

bool PresetManager::initialize() {
    // Verzeichnisse erstellen
    std::filesystem::create_directories(presetDirectory);
    std::filesystem::create_directories(sampleDirectory);
    std::filesystem::create_directories(drumkitDirectory);
    std::filesystem::create_directories(patternDirectory);

    // Presets laden
    loadAllPresets();
    return true;
}

void PresetManager::loadAllPresets() {
    // Drumkits laden
    for (const auto& entry : std::filesystem::directory_iterator(drumkitDirectory)) {
        if (entry.path().extension() == ".json") {
            loadDrumkit(entry.path().stem().string());
        }
    }

    // Patterns laden
    for (const auto& entry : std::filesystem::directory_iterator(patternDirectory)) {
        if (entry.path().extension() == ".json") {
            loadPattern(entry.path().stem().string());
        }
    }

    // Effekt-Presets laden
    for (const auto& entry : std::filesystem::directory_iterator(presetDirectory)) {
        if (entry.path().extension() == ".json") {
            loadEffectPreset(entry.path().stem().string());
        }
    }
}

void PresetManager::saveAllPresets() {
    // Drumkits speichern
    for (const auto& [name, kit] : drumkits) {
        saveDrumkit(name, kit);
    }

    // Patterns speichern
    for (const auto& [name, pattern] : patterns) {
        savePattern(name, pattern);
    }

    // Effekt-Presets speichern
    for (const auto& [name, preset] : effectPresets) {
        saveEffectPreset(name, preset);
    }
}

void PresetManager::loadDrumkit(const std::string& name) {
    std::filesystem::path path = drumkitDirectory / (name + ".json");
    if (!std::filesystem::exists(path)) return;

    try {
        std::ifstream file(path);
        nlohmann::json json;
        file >> json;

        Drumkit kit;
        kit.name = json["name"];
        kit.description = json["description"];
        
        for (const auto& pad : json["pads"]) {
            DrumPad drumPad;
            drumPad.name = pad["name"];
            drumPad.samplePath = pad["samplePath"];
            drumPad.volume = pad["volume"];
            drumPad.pan = pad["pan"];
            drumPad.pitch = pad["pitch"];
            drumPad.filter = pad["filter"];
            drumPad.resonance = pad["resonance"];
            drumPad.envelope = pad["envelope"];
            drumPad.lfo = pad["lfo"];
            drumPad.reverb = pad["reverb"];
            drumPad.delay = pad["delay"];
            drumPad.compression = pad["compression"];
            drumPad.eq = pad["eq"];
            drumPad.distortion = pad["distortion"];
            drumPad.granular = pad["granular"];
            drumPad.timeStretch = pad["timeStretch"];
            drumPad.reverse = pad["reverse"];
            drumPad.slices = pad["slices"].get<std::vector<float>>();
            
            kit.pads.push_back(drumPad);
        }

        drumkits[name] = kit;
    } catch (const std::exception& e) {
        // Fehlerbehandlung
    }
}

void PresetManager::saveDrumkit(const std::string& name, const Drumkit& kit) {
    std::filesystem::path path = drumkitDirectory / (name + ".json");
    
    try {
        nlohmann::json json;
        json["name"] = kit.name;
        json["description"] = kit.description;
        
        for (const auto& pad : kit.pads) {
            nlohmann::json padJson;
            padJson["name"] = pad.name;
            padJson["samplePath"] = pad.samplePath;
            padJson["volume"] = pad.volume;
            padJson["pan"] = pad.pan;
            padJson["pitch"] = pad.pitch;
            padJson["filter"] = pad.filter;
            padJson["resonance"] = pad.resonance;
            padJson["envelope"] = pad.envelope;
            padJson["lfo"] = pad.lfo;
            padJson["reverb"] = pad.reverb;
            padJson["delay"] = pad.delay;
            padJson["compression"] = pad.compression;
            padJson["eq"] = pad.eq;
            padJson["distortion"] = pad.distortion;
            padJson["granular"] = pad.granular;
            padJson["timeStretch"] = pad.timeStretch;
            padJson["reverse"] = pad.reverse;
            padJson["slices"] = pad.slices;
            
            json["pads"].push_back(padJson);
        }

        std::ofstream file(path);
        file << json.dump(4);
    } catch (const std::exception& e) {
        // Fehlerbehandlung
    }
}

void PresetManager::loadPattern(const std::string& name) {
    std::filesystem::path path = patternDirectory / (name + ".json");
    if (!std::filesystem::exists(path)) return;

    try {
        std::ifstream file(path);
        nlohmann::json json;
        file >> json;

        Pattern pattern;
        pattern.name = json["name"];
        pattern.description = json["description"];
        pattern.tempo = json["tempo"];
        pattern.timeSignature = json["timeSignature"];
        pattern.steps = json["steps"].get<std::vector<std::vector<int>>>();
        pattern.automation = json["automation"].get<std::map<std::string, std::vector<float>>>();
        
        patterns[name] = pattern;
    } catch (const std::exception& e) {
        // Fehlerbehandlung
    }
}

void PresetManager::savePattern(const std::string& name, const Pattern& pattern) {
    std::filesystem::path path = patternDirectory / (name + ".json");
    
    try {
        nlohmann::json json;
        json["name"] = pattern.name;
        json["description"] = pattern.description;
        json["tempo"] = pattern.tempo;
        json["timeSignature"] = pattern.timeSignature;
        json["steps"] = pattern.steps;
        json["automation"] = pattern.automation;

        std::ofstream file(path);
        file << json.dump(4);
    } catch (const std::exception& e) {
        // Fehlerbehandlung
    }
}

void PresetManager::loadEffectPreset(const std::string& name) {
    std::filesystem::path path = presetDirectory / (name + ".json");
    if (!std::filesystem::exists(path)) return;

    try {
        std::ifstream file(path);
        nlohmann::json json;
        file >> json;

        EffectPreset preset;
        preset.name = json["name"];
        preset.description = json["description"];
        preset.type = json["type"];
        preset.parameters = json["parameters"].get<std::map<std::string, float>>();
        preset.automation = json["automation"].get<std::map<std::string, std::vector<float>>>();
        
        effectPresets[name] = preset;
    } catch (const std::exception& e) {
        // Fehlerbehandlung
    }
}

void PresetManager::saveEffectPreset(const std::string& name, const EffectPreset& preset) {
    std::filesystem::path path = presetDirectory / (name + ".json");
    
    try {
        nlohmann::json json;
        json["name"] = preset.name;
        json["description"] = preset.description;
        json["type"] = preset.type;
        json["parameters"] = preset.parameters;
        json["automation"] = preset.automation;

        std::ofstream file(path);
        file << json.dump(4);
    } catch (const std::exception& e) {
        // Fehlerbehandlung
    }
}

const std::map<std::string, Drumkit>& PresetManager::getDrumkits() const {
    return drumkits;
}

const std::map<std::string, Pattern>& PresetManager::getPatterns() const {
    return patterns;
}

const std::map<std::string, EffectPreset>& PresetManager::getEffectPresets() const {
    return effectPresets;
}

} // namespace VRMusicStudio 