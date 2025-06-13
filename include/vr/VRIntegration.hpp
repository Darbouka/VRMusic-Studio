#pragma once

#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace VRMusicStudio {

class VRIntegration {
public:
    virtual ~VRIntegration() = default;

    // Initialisierung und Shutdown
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    virtual std::string getSteamVRPath() const = 0;

    // VR-System Status
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool value) = 0;
    virtual std::string getLastError() const = 0;
    virtual void clearError() = 0;

    // Update und Rendering
    virtual void update() = 0;
    virtual void render() = 0;

    // Controller-Funktionen
    virtual bool isControllerConnected(int controllerId) const = 0;
    virtual glm::vec3 getControllerPosition(int controllerId) const = 0;
    virtual glm::quat getControllerRotation(int controllerId) const = 0;
    virtual bool getControllerButtonState(int controllerId, int buttonId) const = 0;
    virtual float getControllerTriggerValue(int controllerId) const = 0;
    virtual void triggerHapticPulse(int controllerId, float duration, float frequency, float amplitude) = 0;

    // HMD-Funktionen
    virtual bool isHMDConnected() const = 0;
    virtual glm::vec3 getHMDPosition() const = 0;
    virtual glm::quat getHMDRotation() const = 0;
    virtual void resetHMDPosition() = 0;

    // Tracking-Funktionen
    virtual bool isTrackingEnabled() const = 0;
    virtual void setTrackingEnabled(bool enabled) = 0;
    virtual glm::mat4 getTrackingMatrix() const = 0;

    // Audio-Funktionen
    virtual bool isSpatialAudioEnabled() const = 0;
    virtual void setSpatialAudioEnabled(bool enabled) = 0;
    virtual void setAudioListenerPosition(const glm::vec3& position) = 0;
    virtual void setAudioListenerOrientation(const glm::quat& orientation) = 0;

    // Factory-Methode
    static std::unique_ptr<VRIntegration> create();
};

} // namespace VRMusicStudio 