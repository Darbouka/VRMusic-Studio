#include "LayoutManager.hpp"
#include <spdlog/spdlog.h>

namespace VRMusicStudio {

struct LayoutManager::Impl {
    LayoutConfig currentConfig;
    std::vector<LayoutConfig> availableLayouts;
    LayoutChangeCallback layoutChangeCallback;
    
    Impl() {
        // Initialisiere verfügbare Layouts
        availableLayouts = {
            {"Default", true, "1.0"},
            {"Custom", true, "1.0"},
            {"Compact", true, "1.0"},
            {"Expanded", true, "1.0"},
            {"Minimal", true, "1.0"}
        };
    }
};

LayoutManager::LayoutManager() : pImpl(std::make_unique<Impl>()) {}
LayoutManager::~LayoutManager() = default;

void LayoutManager::setLayoutConfig(const LayoutConfig& config) {
    pImpl->currentConfig = config;
    
    // Benachrichtige Callback
    if (pImpl->layoutChangeCallback) {
        pImpl->layoutChangeCallback(config.name, config);
    }
}

LayoutManager::LayoutConfig LayoutManager::getLayoutConfig() const {
    return pImpl->currentConfig;
}

std::vector<LayoutManager::LayoutConfig> LayoutManager::getAvailableLayouts() const {
    return pImpl->availableLayouts;
}

bool LayoutManager::isLayoutAvailable(const std::string& layoutName) const {
    return std::find_if(pImpl->availableLayouts.begin(), pImpl->availableLayouts.end(),
        [&layoutName](const LayoutConfig& config) { return config.name == layoutName; }) != pImpl->availableLayouts.end();
}

void LayoutManager::setLayoutChangeCallback(LayoutChangeCallback callback) {
    pImpl->layoutChangeCallback = callback;
}

} // namespace VRMusicStudio 