#include "Config.hpp"
#include "Logger.hpp"
#include <fstream>
#include <filesystem>

namespace VR_DAW {

Config::Config() {
    reset();
}

bool Config::load(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!parseJsonFile(filePath)) {
        Logger("Config").error("Fehler beim Laden der Konfigurationsdatei: {}", filePath);
        return false;
    }

    currentFilePath = filePath;
    return true;
}

bool Config::save(const std::string& filePath) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!writeJsonFile(filePath)) {
        Logger("Config").error("Fehler beim Speichern der Konfigurationsdatei: {}", filePath);
        return false;
    }

    return true;
}

void Config::setValue(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex);
    config[key] = value;
}

void Config::setValue(const std::string& key, int value) {
    std::lock_guard<std::mutex> lock(mutex);
    config[key] = value;
}

void Config::setValue(const std::string& key, float value) {
    std::lock_guard<std::mutex> lock(mutex);
    config[key] = value;
}

void Config::setValue(const std::string& key, bool value) {
    std::lock_guard<std::mutex> lock(mutex);
    config[key] = value;
}

void Config::setValue(const std::string& key, const nlohmann::json& value) {
    std::lock_guard<std::mutex> lock(mutex);
    config[key] = value;
}

std::string Config::getValue(const std::string& key, const std::string& defaultValue) const {
    std::lock_guard<std::mutex> lock(mutex);
    return config.value(key, defaultValue);
}

int Config::getValue(const std::string& key, int defaultValue) const {
    std::lock_guard<std::mutex> lock(mutex);
    return config.value(key, defaultValue);
}

float Config::getValue(const std::string& key, float defaultValue) const {
    std::lock_guard<std::mutex> lock(mutex);
    return config.value(key, defaultValue);
}

bool Config::getValue(const std::string& key, bool defaultValue) const {
    std::lock_guard<std::mutex> lock(mutex);
    return config.value(key, defaultValue);
}

nlohmann::json Config::getValue(const std::string& key, const nlohmann::json& defaultValue) const {
    std::lock_guard<std::mutex> lock(mutex);
    return config.value(key, defaultValue);
}

void Config::reset() {
    std::lock_guard<std::mutex> lock(mutex);
    config = nlohmann::json::object();
}

bool Config::hasValue(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex);
    return config.contains(key);
}

bool Config::parseJsonFile(const std::string& filePath) {
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }

        config = nlohmann::json::parse(file);
        return true;
    }
    catch (const std::exception& e) {
        Logger("Config").error("Fehler beim Parsen der JSON-Datei: {}", e.what());
        return false;
    }
}

bool Config::writeJsonFile(const std::string& filePath) const {
    try {
        // Stelle sicher, dass das Verzeichnis existiert
        std::filesystem::path path(filePath);
        std::filesystem::create_directories(path.parent_path());

        std::ofstream file(filePath);
        if (!file.is_open()) {
            return false;
        }

        file << std::setw(4) << config << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        Logger("Config").error("Fehler beim Schreiben der JSON-Datei: {}", e.what());
        return false;
    }
}

} // namespace VR_DAW 