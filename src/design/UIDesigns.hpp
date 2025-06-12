#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace VRMusicStudio {

class UIDesigns {
public:
    struct DesignConfig {
        std::string name;
        bool isEnabled;
        std::string version;
    };

    UIDesigns();
    ~UIDesigns();

    // Design-Konfiguration
    void setDesignConfig(const DesignConfig& config);
    DesignConfig getDesignConfig() const;

    // Design-Liste
    std::vector<DesignConfig> getAvailableDesigns() const;
    bool isDesignAvailable(const std::string& designName) const;

    // Callback-Funktionen
    using DesignChangeCallback = std::function<void(const std::string&, const DesignConfig&)>;
    void setDesignChangeCallback(DesignChangeCallback callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace VRMusicStudio 