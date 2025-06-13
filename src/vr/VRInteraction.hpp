#pragma once

#include "VRManager.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <memory>

namespace VRMusicStudio {
namespace VR {

class VRInteraction {
public:
    VRInteraction(VRManager& vrManager);
    ~VRInteraction();

    bool initialize();
    void shutdown();
    void update();

    // Controller-Interaktionen
    bool isControllerButtonPressed(uint32_t controllerIndex, uint32_t button);
    bool isControllerButtonTouched(uint32_t controllerIndex, uint32_t button);
    glm::vec2 getControllerAxis(uint32_t controllerIndex, uint32_t axis);
    glm::vec3 getControllerPosition(uint32_t controllerIndex);
    glm::quat getControllerRotation(uint32_t controllerIndex);

    // HMD-Interaktionen
    glm::vec3 getHMDPosition();
    glm::quat getHMDRotation();
    float getIPD();
    void setIPD(float ipd);

    // Tracking-Interaktionen
    bool isDeviceConnected(uint32_t deviceIndex);
    glm::vec3 getDevicePosition(uint32_t deviceIndex);
    glm::quat getDeviceRotation(uint32_t deviceIndex);
    void setDeviceTransform(uint32_t deviceIndex, const glm::vec3& position, const glm::quat& rotation);

    // Raum-Interaktionen
    void setPlayAreaSize(float width, float length);
    glm::vec2 getPlayAreaSize();
    bool isPositionInPlayArea(const glm::vec3& position);
    void setChaperoneVisibility(bool visible);
    bool isChaperoneVisible();

    // Haptisches Feedback
    void triggerHapticPulse(uint32_t controllerIndex, float duration, float frequency, float amplitude);
    void stopHapticPulse(uint32_t controllerIndex);

private:
    VRManager& vrManager;
    std::vector<glm::vec3> controllerPositions;
    std::vector<glm::quat> controllerRotations;
    glm::vec3 hmdPosition;
    glm::quat hmdRotation;
    float ipd;
    glm::vec2 playAreaSize;
    bool chaperoneVisible;
    std::vector<bool> hapticActive;
};

} // namespace VR
} // namespace VRMusicStudio 