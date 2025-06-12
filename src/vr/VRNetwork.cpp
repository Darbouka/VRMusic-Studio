#include "VRNetwork.hpp"
#include "spdlog/spdlog.h"
#include <random>
#include <sstream>
#include <iomanip>

namespace VRMusicStudio {

struct VRNetwork::Impl {
    ENetHost* host = nullptr;
    ENetAddress address;
    bool isServerMode = false;
    std::string serverAddress;
    std::map<std::string, NetworkPeer> peers;
    
    // Callbacks
    std::function<void(const std::string&)> connectCallback;
    std::function<void(const std::string&)> disconnectCallback;
    std::function<void(const std::string&, const std::vector<uint8_t>&)> dataCallback;
    std::function<void(const std::string&)> errorCallback;
    
    // ENet-Konfiguration
    const int maxChannels = 2;
    const int maxIncomingBandwidth = 0; // Unbegrenzt
    const int maxOutgoingBandwidth = 0; // Unbegrenzt
};

VRNetwork::VRNetwork() : pImpl(std::make_unique<Impl>()) {}

VRNetwork::~VRNetwork() {
    shutdown();
}

bool VRNetwork::initialize() {
    if (enet_initialize() != 0) {
        spdlog::error("Failed to initialize ENet");
        return false;
    }
    return true;
}

void VRNetwork::shutdown() {
    if (pImpl->host) {
        enet_host_destroy(pImpl->host);
        pImpl->host = nullptr;
    }
    enet_deinitialize();
}

void VRNetwork::update() {
    if (!pImpl->host) return;
    
    ENetEvent event;
    while (enet_host_service(pImpl->host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                handleConnect(event);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                handleDisconnect(event);
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                handleData(event);
                enet_packet_destroy(event.packet);
                break;
            default:
                break;
        }
    }
}

bool VRNetwork::startServer(uint16_t port, int maxClients) {
    if (pImpl->host) {
        spdlog::error("Server already running");
        return false;
    }
    
    pImpl->address.host = ENET_HOST_ANY;
    pImpl->address.port = port;
    
    pImpl->host = enet_host_create(&pImpl->address, maxClients, 
                                 pImpl->maxChannels,
                                 pImpl->maxIncomingBandwidth,
                                 pImpl->maxOutgoingBandwidth);
    
    if (!pImpl->host) {
        spdlog::error("Failed to create server");
        return false;
    }
    
    pImpl->isServerMode = true;
    spdlog::info("Server started on port {}", port);
    return true;
}

void VRNetwork::stopServer() {
    if (!pImpl->isServerMode || !pImpl->host) return;
    
    enet_host_destroy(pImpl->host);
    pImpl->host = nullptr;
    pImpl->isServerMode = false;
    pImpl->peers.clear();
}

bool VRNetwork::isServer() const {
    return pImpl->isServerMode;
}

std::vector<std::string> VRNetwork::getConnectedClients() const {
    std::vector<std::string> clients;
    for (const auto& peer : pImpl->peers) {
        if (peer.second.connected) {
            clients.push_back(peer.first);
        }
    }
    return clients;
}

bool VRNetwork::connectToServer(const std::string& host, uint16_t port) {
    if (pImpl->host) {
        spdlog::error("Already connected or hosting");
        return false;
    }
    
    pImpl->host = enet_host_create(nullptr, 1,
                                 pImpl->maxChannels,
                                 pImpl->maxIncomingBandwidth,
                                 pImpl->maxOutgoingBandwidth);
    
    if (!pImpl->host) {
        spdlog::error("Failed to create client");
        return false;
    }
    
    enet_address_set_host(&pImpl->address, host.c_str());
    pImpl->address.port = port;
    
    ENetPeer* peer = enet_host_connect(pImpl->host, &pImpl->address, 
                                     pImpl->maxChannels, 0);
    
    if (!peer) {
        spdlog::error("Failed to initiate connection");
        enet_host_destroy(pImpl->host);
        pImpl->host = nullptr;
        return false;
    }
    
    pImpl->serverAddress = host + ":" + std::to_string(port);
    return true;
}

void VRNetwork::disconnect() {
    if (!pImpl->host) return;
    
    if (pImpl->isServerMode) {
        stopServer();
    } else {
        enet_host_destroy(pImpl->host);
        pImpl->host = nullptr;
        pImpl->peers.clear();
    }
}

bool VRNetwork::isConnected() const {
    return pImpl->host != nullptr;
}

std::string VRNetwork::getServerAddress() const {
    return pImpl->serverAddress;
}

void VRNetwork::sendToAll(const std::vector<uint8_t>& data, bool reliable) {
    if (!pImpl->host) return;
    
    ENetPacket* packet = enet_packet_create(data.data(), data.size(),
                                          reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
    
    enet_host_broadcast(pImpl->host, 0, packet);
}

void VRNetwork::sendToPeer(const std::string& peerId, const std::vector<uint8_t>& data, bool reliable) {
    if (!pImpl->host) return;
    
    auto it = pImpl->peers.find(peerId);
    if (it == pImpl->peers.end()) return;
    
    ENetPacket* packet = enet_packet_create(data.data(), data.size(),
                                          reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
    
    enet_peer_send(it->second.peer, 0, packet);
}

void VRNetwork::broadcastPosition(const glm::vec3& position, const glm::quat& rotation) {
    // TODO: Implementiere Position/Rotation Serialisierung
    std::vector<uint8_t> data;
    // ... Serialisierung ...
    sendToAll(data, false);
}

void VRNetwork::broadcastAudio(const std::vector<float>& audioData, uint32_t sampleRate) {
    // TODO: Implementiere Audio-Serialisierung
    std::vector<uint8_t> data;
    // ... Serialisierung ...
    sendToAll(data, true);
}

void VRNetwork::setConnectCallback(std::function<void(const std::string&)> callback) {
    pImpl->connectCallback = callback;
}

void VRNetwork::setDisconnectCallback(std::function<void(const std::string&)> callback) {
    pImpl->disconnectCallback = callback;
}

void VRNetwork::setDataCallback(std::function<void(const std::string&, const std::vector<uint8_t>&)> callback) {
    pImpl->dataCallback = callback;
}

void VRNetwork::setErrorCallback(std::function<void(const std::string&)> callback) {
    pImpl->errorCallback = callback;
}

glm::vec3 VRNetwork::getPeerPosition(const std::string& peerId) const {
    auto it = pImpl->peers.find(peerId);
    return it != pImpl->peers.end() ? it->second.position : glm::vec3(0.0f);
}

glm::quat VRNetwork::getPeerRotation(const std::string& peerId) const {
    auto it = pImpl->peers.find(peerId);
    return it != pImpl->peers.end() ? it->second.rotation : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

bool VRNetwork::isPeerConnected(const std::string& peerId) const {
    auto it = pImpl->peers.find(peerId);
    return it != pImpl->peers.end() && it->second.connected;
}

void VRNetwork::handleConnect(ENetEvent& event) {
    std::string peerId = generatePeerId();
    NetworkPeer peer;
    peer.id = peerId;
    peer.peer = event.peer;
    peer.connected = true;
    pImpl->peers[peerId] = peer;
    
    if (pImpl->connectCallback) {
        pImpl->connectCallback(peerId);
    }
}

void VRNetwork::handleDisconnect(ENetEvent& event) {
    for (auto it = pImpl->peers.begin(); it != pImpl->peers.end(); ++it) {
        if (it->second.peer == event.peer) {
            if (pImpl->disconnectCallback) {
                pImpl->disconnectCallback(it->first);
            }
            pImpl->peers.erase(it);
            break;
        }
    }
}

void VRNetwork::handleData(ENetEvent& event) {
    for (const auto& peer : pImpl->peers) {
        if (peer.second.peer == event.peer) {
            if (pImpl->dataCallback) {
                std::vector<uint8_t> data(event.packet->data,
                                        event.packet->data + event.packet->dataLength);
                pImpl->dataCallback(peer.first, data);
            }
            break;
        }
    }
}

void VRNetwork::handleError(ENetEvent& event) {
    if (pImpl->errorCallback) {
        pImpl->errorCallback("Network error occurred");
    }
}

std::string VRNetwork::generatePeerId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static const char* hex = "0123456789abcdef";
    
    std::stringstream ss;
    for (int i = 0; i < 8; ++i) {
        ss << hex[dis(gen)];
    }
    return ss.str();
}

} // namespace VRMusicStudio 