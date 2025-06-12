#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace VRMusicStudio {

class LayoutManager {
public:
    struct LayoutConfig {
        std::string name;
        bool isEnabled;
        std::string version;
    };

    LayoutManager();
    ~LayoutManager();

    // Layout-Konfiguration
    void setLayoutConfig(const LayoutConfig& config);
    LayoutConfig getLayoutConfig() const;

    // Layout-Liste
    std::vector<LayoutConfig> getAvailableLayouts() const;
    bool isLayoutAvailable(const std::string& layoutName) const;

    // Callback-Funktionen
    using LayoutChangeCallback = std::function<void(const std::string&, const LayoutConfig&)>;
    void setLayoutChangeCallback(LayoutChangeCallback callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace VRMusicStudio 