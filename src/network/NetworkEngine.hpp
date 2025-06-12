#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <enet/enet.h>

namespace VRMusicStudio {

class NetworkEngine {
public:
    struct Peer {
        std::string id;
        std::string name;
        std::string address;
        uint16_t port;
        bool isConnected;
    };

    struct Message {
        std::string type;
        std::string senderId;
        std::string receiverId;
        std::vector<uint8_t> data;
    };

    NetworkEngine();
    ~NetworkEngine();

    bool initialize(uint16_t port = 0);
    void shutdown();

    // Server/Client
    bool startServer(uint16_t port);
    bool connectToServer(const std::string& address, uint16_t port);
    void disconnect();
    bool isServer() const;
    bool isConnected() const;

    // Peer-Management
    std::vector<Peer> getPeers() const;
    bool sendToPeer(const std::string& peerId, const Message& msg, bool reliable = true);
    bool broadcast(const Message& msg, bool reliable = true);

    // Audio/Position Sync
    bool sendAudioBlock(const std::string& peerId, const std::vector<float>& audio);
    bool sendPosition(const std::string& peerId, float x, float y, float z);

    // Event-Handling
    void setOnConnect(std::function<void(const Peer&)> cb);
    void setOnDisconnect(std::function<void(const Peer&)> cb);
    void setOnMessage(std::function<void(const Message&)> cb);
    void setOnError(std::function<void(const std::string&)> cb);

    void pollEvents();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void handleEvent(ENetEvent& event);
};

} // namespace VRMusicStudio 