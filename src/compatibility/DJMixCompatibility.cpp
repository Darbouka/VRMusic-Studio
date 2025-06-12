#include "DJMixCompatibility.hpp"
#include <spdlog/spdlog.h>

namespace VRMusicStudio {

struct DJMixCompatibility::Impl {
    DJMixConfig currentConfig;
    std::vector<DJMixConfig> availableDJMixes;
    DJMixChangeCallback djMixChangeCallback;
    
    Impl() {
        // Initialisiere verfügbare DJ-Mix-Programme
        availableDJMixes = {
            {"Serato DJ", true, "2.5", "USB"},
            {"Traktor Pro", true, "3.0", "USB"},
            {"Virtual DJ", true, "2022", "USB"},
            {"Rekordbox", true, "6.0", "USB"},
            {"Mixxx", true, "2.3", "USB"},
            {"Algoriddim djay", true, "2022", "USB"},
            {"Cross DJ", true, "3.0", "USB"},
            {"Mixvibes Cross", true, "3.0", "USB"},
            {"PCDJ DEX", true, "3.0", "USB"},
            {"Mixmeister", true, "7.0", "USB"}
        };
    }
};

DJMixCompatibility::DJMixCompatibility() : pImpl(std::make_unique<Impl>()) {}
DJMixCompatibility::~DJMixCompatibility() = default;

void DJMixCompatibility::setDJMixConfig(const DJMixConfig& config) {
    pImpl->currentConfig = config;
    
    // Benachrichtige Callback
    if (pImpl->djMixChangeCallback) {
        pImpl->djMixChangeCallback(config.name, config);
    }
}

DJMixCompatibility::DJMixConfig DJMixCompatibility::getDJMixConfig() const {
    return pImpl->currentConfig;
}

std::vector<DJMixCompatibility::DJMixConfig> DJMixCompatibility::getAvailableDJMixes() const {
    return pImpl->availableDJMixes;
}

bool DJMixCompatibility::isDJMixAvailable(const std::string& djMixName) const {
    return std::find_if(pImpl->availableDJMixes.begin(), pImpl->availableDJMixes.end(),
        [&djMixName](const DJMixConfig& config) { return config.name == djMixName; }) != pImpl->availableDJMixes.end();
}

void DJMixCompatibility::setDJMixChangeCallback(DJMixChangeCallback callback) {
    pImpl->djMixChangeCallback = callback;
}

} // namespace VRMusicStudio 