#include "RoomManager.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace VRMusicStudio {

struct RoomManager::Impl {
    RoomState currentState;
    RoomTransition currentTransition;
    std::vector<VirtualRooms::RoomType> availableRooms;
    
    RoomChangeCallback roomChangeCallback;
    ParameterChangeCallback parameterChangeCallback;
    
    Impl() {
        // Initialisiere verfügbare Räume
        availableRooms = {
            VirtualRooms::RoomType::ConcertHall,
            VirtualRooms::RoomType::OperaHouse,
            VirtualRooms::RoomType::RecordingStudio,
            VirtualRooms::RoomType::MixingStudio,
            VirtualRooms::RoomType::Cathedral,
            VirtualRooms::RoomType::Church,
            VirtualRooms::RoomType::ChamberRoom,
            VirtualRooms::RoomType::JazzClub,
            VirtualRooms::RoomType::VirtualSpace,
            VirtualRooms::RoomType::QuantumSpace
        };
        
        // Setze Standard-Raum
        currentState.currentRoom = VirtualRooms::RoomType::MixingStudio;
        optimizeForCurrentRoom();
    }
    
    void optimizeForCurrentRoom() {
        switch (currentState.currentRoom) {
            case VirtualRooms::RoomType::ConcertHall:
                currentState.reverbLevel = 0.8f;
                currentState.earlyReflections = 0.7f;
                currentState.roomSize = 1.0f;
                currentState.brightness = 0.9f;
                currentState.contrast = 1.1f;
                currentState.ambientLight = 0.8f;
                break;
                
            case VirtualRooms::RoomType::OperaHouse:
                currentState.reverbLevel = 0.9f;
                currentState.earlyReflections = 0.8f;
                currentState.roomSize = 1.2f;
                currentState.brightness = 0.8f;
                currentState.contrast = 1.2f;
                currentState.ambientLight = 0.7f;
                break;
                
            case VirtualRooms::RoomType::RecordingStudio:
                currentState.reverbLevel = 0.2f;
                currentState.earlyReflections = 0.3f;
                currentState.roomSize = 0.5f;
                currentState.brightness = 1.0f;
                currentState.contrast = 1.0f;
                currentState.ambientLight = 1.0f;
                break;
                
            case VirtualRooms::RoomType::MixingStudio:
                currentState.reverbLevel = 0.3f;
                currentState.earlyReflections = 0.4f;
                currentState.roomSize = 0.6f;
                currentState.brightness = 1.0f;
                currentState.contrast = 1.0f;
                currentState.ambientLight = 1.0f;
                break;
                
            case VirtualRooms::RoomType::Cathedral:
                currentState.reverbLevel = 1.0f;
                currentState.earlyReflections = 0.9f;
                currentState.roomSize = 1.5f;
                currentState.brightness = 0.7f;
                currentState.contrast = 1.3f;
                currentState.ambientLight = 0.6f;
                break;
                
            case VirtualRooms::RoomType::Church:
                currentState.reverbLevel = 0.9f;
                currentState.earlyReflections = 0.8f;
                currentState.roomSize = 1.3f;
                currentState.brightness = 0.8f;
                currentState.contrast = 1.2f;
                currentState.ambientLight = 0.7f;
                break;
                
            case VirtualRooms::RoomType::ChamberRoom:
                currentState.reverbLevel = 0.5f;
                currentState.earlyReflections = 0.6f;
                currentState.roomSize = 0.8f;
                currentState.brightness = 0.9f;
                currentState.contrast = 1.1f;
                currentState.ambientLight = 0.8f;
                break;
                
            case VirtualRooms::RoomType::JazzClub:
                currentState.reverbLevel = 0.6f;
                currentState.earlyReflections = 0.5f;
                currentState.roomSize = 0.9f;
                currentState.brightness = 0.7f;
                currentState.contrast = 1.2f;
                currentState.ambientLight = 0.6f;
                break;
                
            case VirtualRooms::RoomType::VirtualSpace:
                currentState.reverbLevel = 0.4f;
                currentState.earlyReflections = 0.4f;
                currentState.roomSize = 1.0f;
                currentState.brightness = 1.0f;
                currentState.contrast = 1.0f;
                currentState.ambientLight = 1.0f;
                break;
                
            case VirtualRooms::RoomType::QuantumSpace:
                currentState.reverbLevel = 0.5f;
                currentState.earlyReflections = 0.5f;
                currentState.roomSize = 1.1f;
                currentState.brightness = 1.1f;
                currentState.contrast = 1.1f;
                currentState.ambientLight = 1.1f;
                break;
        }
        
        // Benachrichtige Callbacks über Parameteränderungen
        if (parameterChangeCallback) {
            parameterChangeCallback("reverb", currentState.reverbLevel);
            parameterChangeCallback("reflections", currentState.earlyReflections);
            parameterChangeCallback("roomSize", currentState.roomSize);
            parameterChangeCallback("brightness", currentState.brightness);
            parameterChangeCallback("contrast", currentState.contrast);
            parameterChangeCallback("ambientLight", currentState.ambientLight);
        }
    }
};

RoomManager::RoomManager() : pImpl(std::make_unique<Impl>()) {}
RoomManager::~RoomManager() = default;

void RoomManager::switchRoom(VirtualRooms::RoomType newRoom, const RoomTransition& transition) {
    if (!pImpl->isTransitioning() && pImpl->isRoomAvailable(newRoom)) {
        pImpl->currentTransition = transition;
        pImpl->currentState.isTransitioning = true;
        pImpl->currentState.transitionProgress = 0.0f;
        
        // Starte Übergang
        if (pImpl->roomChangeCallback) {
            pImpl->roomChangeCallback(newRoom, pImpl->currentState);
        }
        
        // Optimiere für neuen Raum
        pImpl->currentState.currentRoom = newRoom;
        pImpl->optimizeForCurrentRoom();
    }
}

void RoomManager::updateRoomParameters(const RoomState& newState) {
    pImpl->currentState = newState;
    
    // Benachrichtige Callbacks
    if (pImpl->roomChangeCallback) {
        pImpl->roomChangeCallback(pImpl->currentState.currentRoom, pImpl->currentState);
    }
}

RoomManager::RoomState RoomManager::getCurrentRoomState() const {
    return pImpl->currentState;
}

std::vector<VirtualRooms::RoomType> RoomManager::getAvailableRooms() const {
    return pImpl->availableRooms;
}

bool RoomManager::isRoomAvailable(VirtualRooms::RoomType room) const {
    return std::find(pImpl->availableRooms.begin(), pImpl->availableRooms.end(), room) 
           != pImpl->availableRooms.end();
}

void RoomManager::setRoomChangeCallback(RoomChangeCallback callback) {
    pImpl->roomChangeCallback = callback;
}

void RoomManager::setParameterChangeCallback(ParameterChangeCallback callback) {
    pImpl->parameterChangeCallback = callback;
}

void RoomManager::startTransition(const RoomTransition& transition) {
    pImpl->currentTransition = transition;
    pImpl->currentState.isTransitioning = true;
    pImpl->currentState.transitionProgress = 0.0f;
}

void RoomManager::cancelTransition() {
    pImpl->currentState.isTransitioning = false;
    pImpl->currentState.transitionProgress = 0.0f;
}

float RoomManager::getTransitionProgress() const {
    return pImpl->currentState.transitionProgress;
}

void RoomManager::setAcousticParameters(float reverb, float reflections, float size) {
    pImpl->currentState.reverbLevel = reverb;
    pImpl->currentState.earlyReflections = reflections;
    pImpl->currentState.roomSize = size;
    
    if (pImpl->parameterChangeCallback) {
        pImpl->parameterChangeCallback("reverb", reverb);
        pImpl->parameterChangeCallback("reflections", reflections);
        pImpl->parameterChangeCallback("roomSize", size);
    }
}

void RoomManager::setVisualParameters(float brightness, float contrast, float ambient) {
    pImpl->currentState.brightness = brightness;
    pImpl->currentState.contrast = contrast;
    pImpl->currentState.ambientLight = ambient;
    
    if (pImpl->parameterChangeCallback) {
        pImpl->parameterChangeCallback("brightness", brightness);
        pImpl->parameterChangeCallback("contrast", contrast);
        pImpl->parameterChangeCallback("ambientLight", ambient);
    }
}

void RoomManager::setPerformanceParameters(int quality, bool shadows, bool reflections) {
    pImpl->currentState.renderQuality = quality;
    pImpl->currentState.enableShadows = shadows;
    pImpl->currentState.enableReflections = reflections;
}

bool RoomManager::isTransitioning() const {
    return pImpl->currentState.isTransitioning;
}

VirtualRooms::RoomType RoomManager::getCurrentRoom() const {
    return pImpl->currentState.currentRoom;
}

void RoomManager::optimizeForCurrentRoom() {
    pImpl->optimizeForCurrentRoom();
}

void RoomManager::resetToDefaultParameters() {
    pImpl->currentState = RoomState();
    pImpl->optimizeForCurrentRoom();
}

} // namespace VRMusicStudio 