#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace VRMusicStudio {

class DAWCompatibility {
public:
    struct DAWConfig {
        std::string name;
        bool isEnabled;
        std::string version;
        std::string connectionType;
    };

    DAWCompatibility();
    ~DAWCompatibility();

    // DAW-Konfiguration
    void setDAWConfig(const DAWConfig& config);
    DAWConfig getDAWConfig() const;

    // DAW-Liste
    std::vector<DAWConfig> getAvailableDAWs() const;
    bool isDAWAvailable(const std::string& dawName) const;

    // Callback-Funktionen
    using DAWChangeCallback = std::function<void(const std::string&, const DAWConfig&)>;
    void setDAWChangeCallback(DAWChangeCallback callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace VRMusicStudio 