#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace VRMusicStudio {

class DJMixCompatibility {
public:
    struct DJMixConfig {
        std::string name;
        bool isEnabled;
        std::string version;
        std::string connectionType;
    };

    DJMixCompatibility();
    ~DJMixCompatibility();

    // DJ-Mix-Konfiguration
    void setDJMixConfig(const DJMixConfig& config);
    DJMixConfig getDJMixConfig() const;

    // DJ-Mix-Liste
    std::vector<DJMixConfig> getAvailableDJMixes() const;
    bool isDJMixAvailable(const std::string& djMixName) const;

    // Callback-Funktionen
    using DJMixChangeCallback = std::function<void(const std::string&, const DJMixConfig&)>;
    void setDJMixChangeCallback(DJMixChangeCallback callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace VRMusicStudio 