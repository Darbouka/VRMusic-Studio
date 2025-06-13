#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace VRMusicStudio {

class ConnectionOptions {
public:
    struct ConnectionConfig {
        std::string type;
        bool isEnabled;
        std::string version;
    };

    ConnectionOptions();
    ~ConnectionOptions();

    // Anschluss-Konfiguration
    void setConnectionConfig(const ConnectionConfig& config);
    ConnectionConfig getConnectionConfig() const;

    // Anschluss-Liste
    std::vector<ConnectionConfig> getAvailableConnections() const;
    bool isConnectionAvailable(const std::string& connectionType) const;

    // Callback-Funktionen
    using ConnectionChangeCallback = std::function<void(const std::string&, const ConnectionConfig&)>;
    void setConnectionChangeCallback(ConnectionChangeCallback callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace VRMusicStudio 