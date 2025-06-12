#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <glm/glm.hpp>
#include "../audio/VirtualRooms.hpp"

namespace VRMusicStudio {

class RoomManager {
public:
    struct RoomTransition {
        float duration = 1.0f;        // Übergangsdauer in Sekunden
        bool fadeAudio = true;        // Audio-Überblendung
        bool fadeVisuals = true;      // Visuelle Überblendung
        bool adjustAcoustics = true;  // Akustik-Anpassung
    };

    struct RoomState {
        // Aktuelle Raum-Parameter
        VirtualRooms::RoomType currentRoom;
        float transitionProgress = 0.0f;
        bool isTransitioning = false;
        
        // Akustische Parameter
        float reverbLevel = 0.0f;
        float earlyReflections = 0.0f;
        float roomSize = 0.0f;
        
        // Visuelle Parameter
        float brightness = 1.0f;
        float contrast = 1.0f;
        float ambientLight = 1.0f;
        
        // Performance-Parameter
        int renderQuality = 2;
        bool enableShadows = true;
        bool enableReflections = true;
    };

    RoomManager();
    ~RoomManager();

    // Raum-Management
    void switchRoom(VirtualRooms::RoomType newRoom, const RoomTransition& transition = RoomTransition());
    void updateRoomParameters(const RoomState& newState);
    RoomState getCurrentRoomState() const;
    
    // Raum-Liste
    std::vector<VirtualRooms::RoomType> getAvailableRooms() const;
    bool isRoomAvailable(VirtualRooms::RoomType room) const;
    
    // Callback-Funktionen
    using RoomChangeCallback = std::function<void(VirtualRooms::RoomType, const RoomState&)>;
    using ParameterChangeCallback = std::function<void(const std::string&, float)>;
    
    void setRoomChangeCallback(RoomChangeCallback callback);
    void setParameterChangeCallback(ParameterChangeCallback callback);
    
    // Raum-Übergänge
    void startTransition(const RoomTransition& transition);
    void cancelTransition();
    float getTransitionProgress() const;
    
    // Raum-Parameter
    void setAcousticParameters(float reverb, float reflections, float size);
    void setVisualParameters(float brightness, float contrast, float ambient);
    void setPerformanceParameters(int quality, bool shadows, bool reflections);
    
    // Raum-Status
    bool isTransitioning() const;
    VirtualRooms::RoomType getCurrentRoom() const;
    
    // Raum-Optimierung
    void optimizeForCurrentRoom();
    void resetToDefaultParameters();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace VRMusicStudio 