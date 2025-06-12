#include "NetworkEngine.hpp"
<<<<<<< HEAD
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace Network_DAW {

NetworkEngine::NetworkEngine() {
    initializeComponents();
}

NetworkEngine::~NetworkEngine() {
    shutdown();
}

void NetworkEngine::initialize() {
    try {
        initializeComponents();
        updateState();
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::update() {
    try {
        updateState();
        processNetwork();
        processNetworkChannel();
        processNetworkBus();
        updateParameters();
        updateAnalysis();
        generateVisualization();
        validateState();
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::shutdown() {
    try {
        state.networkBuffers.clear();
        state.networkParameters.clear();
        state.networkBypasses.clear();
        state.networkMixes.clear();
        state.channels.clear();
        state.channelParameters.clear();
        state.channelBypasses.clear();
        state.channelMixes.clear();
        state.buses.clear();
        state.busParameters.clear();
        state.busBypasses.clear();
        state.busMixes.clear();
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::processNetwork(const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer) {
    try {
        if (!validateBuffer(inputBuffer)) {
            throw std::runtime_error("Invalid input buffer");
        }

        outputBuffer = inputBuffer;
        for (const auto& [name, buffer] : state.networkBuffers) {
            if (!state.networkBypasses[name]) {
                // Process network based on name
                if (name == "tcp") {
                    // Apply TCP network processing
                    for (size_t i = 0; i < outputBuffer.size(); ++i) {
                        outputBuffer[i] += buffer[i] * state.networkMixes[name];
                    }
                } else if (name == "udp") {
                    // Apply UDP network processing
                    for (size_t i = 0; i < outputBuffer.size(); ++i) {
                        outputBuffer[i] += buffer[i] * state.networkMixes[name];
                    }
                } else if (name == "websocket") {
                    // Apply WebSocket network processing
                    for (size_t i = 0; i < outputBuffer.size(); ++i) {
                        outputBuffer[i] *= state.networkParameters[name]["websocket_rate"];
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::processNetworkChannel(const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer) {
    try {
        if (!validateBuffer(inputBuffer)) {
            throw std::runtime_error("Invalid input buffer");
        }

        outputBuffer = inputBuffer;
        for (const auto& [name, buffer] : state.networkBuffers) {
            if (!state.networkBypasses[name]) {
                for (size_t i = 0; i < outputBuffer.size(); ++i) {
                    outputBuffer[i] += buffer[i] * state.networkMixes[name];
                }
            }
        }
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::processNetworkBus(const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer) {
    try {
        if (!validateBuffer(inputBuffer)) {
            throw std::runtime_error("Invalid input buffer");
        }

        outputBuffer = inputBuffer;
        for (const auto& [name, buffer] : state.networkBuffers) {
            if (!state.networkBypasses[name]) {
                for (size_t i = 0; i < outputBuffer.size(); ++i) {
                    outputBuffer[i] += buffer[i] * state.networkMixes[name];
                }
            }
        }
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::createChannel(const std::string& name) {
    try {
        state.channels[name] = std::vector<std::string>();
        state.channelParameters[name] = std::map<std::string, float>();
        state.channelBypasses[name] = false;
        state.channelMixes[name] = 1.0f;
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::deleteChannel(const std::string& name) {
    try {
        state.channels.erase(name);
        state.channelParameters.erase(name);
        state.channelBypasses.erase(name);
        state.channelMixes.erase(name);
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::setChannelParameter(const std::string& name, const std::string& parameterName, float value) {
    try {
        state.channelParameters[name][parameterName] = value;
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::setChannelBypass(const std::string& name, bool bypass) {
    try {
        state.channelBypasses[name] = bypass;
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::setChannelMix(const std::string& name, float mix) {
    try {
        state.channelMixes[name] = std::clamp(mix, 0.0f, 1.0f);
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::createBus(const std::string& name) {
    try {
        state.buses[name] = std::vector<std::string>();
        state.busParameters[name] = std::map<std::string, float>();
        state.busBypasses[name] = false;
        state.busMixes[name] = 1.0f;
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::deleteBus(const std::string& name) {
    try {
        state.buses.erase(name);
        state.busParameters.erase(name);
        state.busBypasses.erase(name);
        state.busMixes.erase(name);
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::setBusParameter(const std::string& name, const std::string& parameterName, float value) {
    try {
        state.busParameters[name][parameterName] = value;
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::setBusBypass(const std::string& name, bool bypass) {
    try {
        state.busBypasses[name] = bypass;
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::setBusMix(const std::string& name, float mix) {
    try {
        state.busMixes[name] = std::clamp(mix, 0.0f, 1.0f);
    } catch (const std::exception& e) {
        handleErrors();
    }
}

float NetworkEngine::getNetworkLevel(const std::string& name) {
    try {
        return analysis.networkLevels[name];
    } catch (const std::exception& e) {
        handleErrors();
        return 0.0f;
    }
}

float NetworkEngine::getNetworkSpectrum(const std::string& name) {
    try {
        return analysis.networkSpectrums[name];
    } catch (const std::exception& e) {
        handleErrors();
        return 0.0f;
    }
}

float NetworkEngine::getNetworkPhase(const std::string& name) {
    try {
        return analysis.networkPhases[name];
    } catch (const std::exception& e) {
        handleErrors();
        return 0.0f;
    }
}

float NetworkEngine::getNetworkCorrelation(const std::string& name) {
    try {
        return analysis.networkCorrelations[name];
    } catch (const std::exception& e) {
        handleErrors();
        return 0.0f;
    }
}

float NetworkEngine::getNetworkDynamics(const std::string& name) {
    try {
        return analysis.networkDynamics[name];
    } catch (const std::exception& e) {
        handleErrors();
        return 0.0f;
    }
}

float NetworkEngine::getNetworkStereo(const std::string& name) {
    try {
        return analysis.networkStereos[name];
    } catch (const std::exception& e) {
        handleErrors();
        return 0.0f;
    }
}

float NetworkEngine::getNetworkFrequency(const std::string& name) {
    try {
        return analysis.networkFrequencies[name];
    } catch (const std::exception& e) {
        handleErrors();
        return 0.0f;
    }
}

float NetworkEngine::getNetworkTransient(const std::string& name) {
    try {
        return analysis.networkTransients[name];
    } catch (const std::exception& e) {
        handleErrors();
        return 0.0f;
    }
}

void NetworkEngine::updateNetworkVisualization(const std::string& name) {
    try {
        // Update network visualization
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::updateChannelVisualization(const std::string& name) {
    try {
        // Update channel visualization
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::updateBusVisualization(const std::string& name) {
    try {
        // Update bus visualization
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::updateAnalysisVisualization() {
    try {
        // Update analysis visualization
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::initializeComponents() {
    try {
        // Initialize components
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::updateState() {
    try {
        // Update state
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::processNetwork() {
    try {
        // Process network
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::processNetworkChannel() {
    try {
        // Process network channel
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::processNetworkBus() {
    try {
        // Process network bus
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::updateParameters() {
    try {
        // Update parameters
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::updateAnalysis() {
    try {
        // Update analysis
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::generateVisualization() {
    try {
        // Generate visualization
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::validateState() {
    try {
        // Validate state
    } catch (const std::exception& e) {
        handleErrors();
    }
}

void NetworkEngine::handleErrors() {
    // Handle errors
}

bool NetworkEngine::validateBuffer(const std::vector<float>& buffer) {
    return !buffer.empty() && buffer.size() == parameters.bufferSize;
}

float NetworkEngine::calculateLevel(const std::vector<float>& buffer) {
    float sum = 0.0f;
    for (float sample : buffer) {
        sum += sample * sample;
    }
    return std::sqrt(sum / buffer.size());
}

float NetworkEngine::calculateSpectrum(const std::vector<float>& buffer) {
    // Implement spectrum calculation
    return 0.0f;
}

float NetworkEngine::calculatePhase(const std::vector<float>& buffer) {
    // Implement phase calculation
    return 0.0f;
}

float NetworkEngine::calculateCorrelation(const std::vector<float>& leftBuffer, const std::vector<float>& rightBuffer) {
    // Implement correlation calculation
    return 0.0f;
}

float NetworkEngine::calculateDynamics(const std::vector<float>& buffer) {
    // Implement dynamics calculation
    return 0.0f;
}

float NetworkEngine::calculateStereo(const std::vector<float>& leftBuffer, const std::vector<float>& rightBuffer) {
    // Implement stereo calculation
    return 0.0f;
}

float NetworkEngine::calculateFrequency(const std::vector<float>& buffer) {
    // Implement frequency calculation
    return 0.0f;
}

float NetworkEngine::calculateTransient(const std::vector<float>& buffer) {
    // Implement transient calculation
    return 0.0f;
}

} // namespace Network_DAW 
=======
#include <spdlog/spdlog.h>
#include <thread>
#include <cstring>

namespace VRMusicStudio {

struct NetworkEngine::Impl {
    ENetHost* host = nullptr;
    ENetPeer* serverPeer = nullptr;
    std::map<std::string, Peer> peers;
    bool serverMode = false;
    bool connected = false;
    uint16_t port = 0;

    std::function<void(const Peer&)> onConnect;
    std::function<void(const Peer&)> onDisconnect;
    std::function<void(const Message&)> onMessage;
    std::function<void(const std::string&)> onError;
};

NetworkEngine::NetworkEngine() : pImpl(std::make_unique<Impl>()) {}
NetworkEngine::~NetworkEngine() { shutdown(); }

bool NetworkEngine::initialize(uint16_t port) {
    if (enet_initialize() != 0) {
        spdlog::error("ENet konnte nicht initialisiert werden");
        return false;
    }
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;
    pImpl->host = enet_host_create(&address, 32, 2, 0, 0);
    if (!pImpl->host) {
        spdlog::error("ENet Host konnte nicht erstellt werden");
        return false;
    }
    pImpl->port = port;
    pImpl->serverMode = (port != 0);
    return true;
}

void NetworkEngine::shutdown() {
    if (pImpl->host) {
        enet_host_destroy(pImpl->host);
        pImpl->host = nullptr;
    }
    enet_deinitialize();
    pImpl->peers.clear();
    pImpl->serverPeer = nullptr;
    pImpl->connected = false;
}

bool NetworkEngine::startServer(uint16_t port) {
    return initialize(port);
}

bool NetworkEngine::connectToServer(const std::string& address, uint16_t port) {
    if (!initialize(0)) return false;
    ENetAddress enetAddress;
    enet_address_set_host(&enetAddress, address.c_str());
    enetAddress.port = port;
    pImpl->serverPeer = enet_host_connect(pImpl->host, &enetAddress, 2, 0);
    if (!pImpl->serverPeer) {
        spdlog::error("Verbindung zum Server fehlgeschlagen");
        return false;
    }
    pImpl->connected = true;
    return true;
}

void NetworkEngine::disconnect() {
    if (pImpl->serverPeer) {
        enet_peer_disconnect(pImpl->serverPeer, 0);
        pImpl->serverPeer = nullptr;
    }
    pImpl->connected = false;
}

bool NetworkEngine::isServer() const { return pImpl->serverMode; }
bool NetworkEngine::isConnected() const { return pImpl->connected; }

std::vector<NetworkEngine::Peer> NetworkEngine::getPeers() const {
    std::vector<Peer> result;
    for (const auto& [id, peer] : pImpl->peers) result.push_back(peer);
    return result;
}

bool NetworkEngine::sendToPeer(const std::string& peerId, const Message& msg, bool reliable) {
    // TODO: Implementiere Peer-Kommunikation
    return true;
}

bool NetworkEngine::broadcast(const Message& msg, bool reliable) {
    // TODO: Implementiere Broadcast
    return true;
}

bool NetworkEngine::sendAudioBlock(const std::string& peerId, const std::vector<float>& audio) {
    // TODO: Implementiere Audio-Block-Senden
    return true;
}

bool NetworkEngine::sendPosition(const std::string& peerId, float x, float y, float z) {
    // TODO: Implementiere Positions-Senden
    return true;
}

void NetworkEngine::setOnConnect(std::function<void(const Peer&)> cb) { pImpl->onConnect = cb; }
void NetworkEngine::setOnDisconnect(std::function<void(const Peer&)> cb) { pImpl->onDisconnect = cb; }
void NetworkEngine::setOnMessage(std::function<void(const Message&)> cb) { pImpl->onMessage = cb; }
void NetworkEngine::setOnError(std::function<void(const std::string&)> cb) { pImpl->onError = cb; }

void NetworkEngine::pollEvents() {
    if (!pImpl->host) return;
    ENetEvent event;
    while (enet_host_service(pImpl->host, &event, 0) > 0) {
        handleEvent(event);
    }
}

void NetworkEngine::handleEvent(ENetEvent& event) {
    switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT: {
            Peer peer;
            peer.id = std::to_string(reinterpret_cast<uintptr_t>(event.peer));
            peer.address = std::to_string(event.peer->address.host);
            peer.port = event.peer->address.port;
            peer.isConnected = true;
            pImpl->peers[peer.id] = peer;
            if (pImpl->onConnect) pImpl->onConnect(peer);
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT: {
            std::string id = std::to_string(reinterpret_cast<uintptr_t>(event.peer));
            if (pImpl->peers.count(id)) {
                Peer peer = pImpl->peers[id];
                pImpl->peers.erase(id);
                if (pImpl->onDisconnect) pImpl->onDisconnect(peer);
            }
            break;
        }
        case ENET_EVENT_TYPE_RECEIVE: {
            Message msg;
            msg.type = "data";
            msg.senderId = std::to_string(reinterpret_cast<uintptr_t>(event.peer));
            msg.data.assign(event.packet->data, event.packet->data + event.packet->dataLength);
            if (pImpl->onMessage) pImpl->onMessage(msg);
            enet_packet_destroy(event.packet);
            break;
        }
        case ENET_EVENT_TYPE_NONE:
        default:
            break;
    }
}

} // namespace VRMusicStudio 
>>>>>>> 0dff1c4 (init 2)
