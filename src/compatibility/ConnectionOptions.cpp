#include "ConnectionOptions.hpp"
#include <spdlog/spdlog.h>

namespace VRMusicStudio {

struct ConnectionOptions::Impl {
    ConnectionConfig currentConfig;
    std::vector<ConnectionConfig> availableConnections;
    ConnectionChangeCallback connectionChangeCallback;
    
    Impl() {
        // Initialisiere verfügbare Anschlussoptionen
        availableConnections = {
            {"USB", true, "3.0"},
            {"MIDI", true, "2.0"},
            {"Bluetooth", true, "5.0"},
            {"AirPlay", true, "2.0"},
            {"Ethernet", true, "1.0"},
            {"Thunderbolt", true, "3.0"},
            {"FireWire", true, "800"},
            {"HDMI", true, "2.0"},
            {"DisplayPort", true, "1.4"},
            {"Audio-Out", true, "3.5mm"}
        };
    }
};

ConnectionOptions::ConnectionOptions() : pImpl(std::make_unique<Impl>()) {}
ConnectionOptions::~ConnectionOptions() = default;

void ConnectionOptions::setConnectionConfig(const ConnectionConfig& config) {
    pImpl->currentConfig = config;
    
    // Benachrichtige Callback
    if (pImpl->connectionChangeCallback) {
        pImpl->connectionChangeCallback(config.type, config);
    }
}

ConnectionOptions::ConnectionConfig ConnectionOptions::getConnectionConfig() const {
    return pImpl->currentConfig;
}

std::vector<ConnectionOptions::ConnectionConfig> ConnectionOptions::getAvailableConnections() const {
    return pImpl->availableConnections;
}

bool ConnectionOptions::isConnectionAvailable(const std::string& connectionType) const {
    return std::find_if(pImpl->availableConnections.begin(), pImpl->availableConnections.end(),
        [&connectionType](const ConnectionConfig& config) { return config.type == connectionType; }) != pImpl->availableConnections.end();
}

void ConnectionOptions::setConnectionChangeCallback(ConnectionChangeCallback callback) {
    pImpl->connectionChangeCallback = callback;
}

} // namespace VRMusicStudio 