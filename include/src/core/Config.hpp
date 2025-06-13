#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <nlohmann/json.hpp>

namespace VR_DAW {

class Config {
public:
    Config();
    ~Config() = default;

    // Konfigurationsdatei laden/speichern
    bool load(const std::string& filePath);
    bool save(const std::string& filePath) const;

    // Werte setzen/abrufen
    void setValue(const std::string& key, const std::string& value);
    void setValue(const std::string& key, int value);
    void setValue(const std::string& key, float value);
    void setValue(const std::string& key, bool value);
    void setValue(const std::string& key, const nlohmann::json& value);

    std::string getValue(const std::string& key, const std::string& defaultValue = "") const;
    int getValue(const std::string& key, int defaultValue = 0) const;
    float getValue(const std::string& key, float defaultValue = 0.0f) const;
    bool getValue(const std::string& key, bool defaultValue = false) const;
    nlohmann::json getValue(const std::string& key, const nlohmann::json& defaultValue = nlohmann::json()) const;

    // Konfiguration zurücksetzen
    void reset();

    // Konfiguration überprüfen
    bool hasValue(const std::string& key) const;

private:
    nlohmann::json config;
    mutable std::mutex mutex;
    std::string currentFilePath;

    bool parseJsonFile(const std::string& filePath);
    bool writeJsonFile(const std::string& filePath) const;
};

} // namespace VR_DAW 