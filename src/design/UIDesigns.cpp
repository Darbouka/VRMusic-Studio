#include "UIDesigns.hpp"
#include <spdlog/spdlog.h>

namespace VRMusicStudio {

struct UIDesigns::Impl {
    DesignConfig currentConfig;
    std::vector<DesignConfig> availableDesigns;
    DesignChangeCallback designChangeCallback;
    
    Impl() {
        // Initialisiere verfügbare Designs
        availableDesigns = {
            {"Modern", true, "1.0"},
            {"Classic", true, "1.0"},
            {"Dark", true, "1.0"},
            {"Light", true, "1.0"},
            {"Custom", true, "1.0"}
        };
    }
};

UIDesigns::UIDesigns() : pImpl(std::make_unique<Impl>()) {}
UIDesigns::~UIDesigns() = default;

void UIDesigns::setDesignConfig(const DesignConfig& config) {
    pImpl->currentConfig = config;
    
    // Benachrichtige Callback
    if (pImpl->designChangeCallback) {
        pImpl->designChangeCallback(config.name, config);
    }
}

UIDesigns::DesignConfig UIDesigns::getDesignConfig() const {
    return pImpl->currentConfig;
}

std::vector<UIDesigns::DesignConfig> UIDesigns::getAvailableDesigns() const {
    return pImpl->availableDesigns;
}

bool UIDesigns::isDesignAvailable(const std::string& designName) const {
    return std::find_if(pImpl->availableDesigns.begin(), pImpl->availableDesigns.end(),
        [&designName](const DesignConfig& config) { return config.name == designName; }) != pImpl->availableDesigns.end();
}

void UIDesigns::setDesignChangeCallback(DesignChangeCallback callback) {
    pImpl->designChangeCallback = callback;
}

} // namespace VRMusicStudio 