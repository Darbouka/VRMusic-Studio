#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <enet/enet.h>

namespace VRMusicStudio {

class VRNetwork {
public:
    struct NetworkEvent {
        enum class Type {
            Connect,
            Disconnect,
            Data,
            Error
        };
        
        Type type;
        std::string peerId;
        std::vector<uint8_t> data;
    };
    
    struct NetworkPeer {
        std::string id;
        ENetPeer* peer;
        glm::vec3 position;
        glm::quat rotation;
        bool connected;
    };

    VRNetwork();
    ~VRNetwork();

    bool initialize();
    void shutdown();
    void update();

    // Server
    bool startServer(uint16_t port, int maxClients = 32);
    void stopServer();
    bool isServer() const;
    std::vector<std::string> getConnectedClients() const;

    // Client
    bool connectToServer(const std::string& host, uint16_t port);
    void disconnect();
    bool isConnected() const;
    std::string getServerAddress() const;

    // Daten senden
    void sendToAll(const std::vector<uint8_t>& data, bool reliable = true);
    void sendToPeer(const std::string& peerId, const std::vector<uint8_t>& data, bool reliable = true);
    void broadcastPosition(const glm::vec3& position, const glm::quat& rotation);
    void broadcastAudio(const std::vector<float>& audioData, uint32_t sampleRate);

    // Event-Handler
    void setConnectCallback(std::function<void(const std::string&)> callback);
    void setDisconnectCallback(std::function<void(const std::string&)> callback);
    void setDataCallback(std::function<void(const std::string&, const std::vector<uint8_t>&)> callback);
    void setErrorCallback(std::function<void(const std::string&)> callback);

    // Peer-Informationen
    glm::vec3 getPeerPosition(const std::string& peerId) const;
    glm::quat getPeerRotation(const std::string& peerId) const;
    bool isPeerConnected(const std::string& peerId) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void initializeENet();
    void processEvents();
    void handleConnect(ENetEvent& event);
    void handleDisconnect(ENetEvent& event);
    void handleData(ENetEvent& event);
    void handleError(ENetEvent& event);
    std::string generatePeerId();
};

} // namespace VRMusicStudio 