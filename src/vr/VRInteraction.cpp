#include "VRInteraction.hpp"
#include "core/Logger.hpp"
#include <algorithm>

namespace VRMusicStudio {
namespace VR {

VRInteraction::VRInteraction(VRManager& vrManager)
    : vrManager(vrManager)
    , controllerPositions(2) // Maximal 2 Controller
    , controllerRotations(2)
    , hmdPosition(0.0f)
    , hmdRotation(1.0f, 0.0f, 0.0f, 0.0f)
    , ipd(0.064f) // Standard-IPD
    , playAreaSize(2.0f, 2.0f) // Standard-Spielbereich
    , chaperoneVisible(true)
    , hapticActive(2, false) {
}

VRInteraction::~VRInteraction() = default;

bool VRInteraction::initialize() {
    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Initialisiere VR-Interaktionen...");

        // Initialisiere Controller-Positionen und -Rotationen
        for (size_t i = 0; i < controllerPositions.size(); ++i) {
            controllerPositions[i] = glm::vec3(0.0f);
            controllerRotations[i] = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        }

        logger.info("VR-Interaktionen erfolgreich initialisiert");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der VR-Interaktions-Initialisierung: {}", e.what());
        return false;
    }
}

void VRInteraction::shutdown() {
    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Beende VR-Interaktionen...");

        // Stoppe alle haptischen Pulse
        for (size_t i = 0; i < hapticActive.size(); ++i) {
            if (hapticActive[i]) {
                stopHapticPulse(i);
            }
        }

        logger.info("VR-Interaktionen erfolgreich beendet");
    } catch (const std::exception& e) {
        spdlog::error("Fehler beim Beenden der VR-Interaktionen: {}", e.what());
    }
}

void VRInteraction::update() {
    // Aktualisiere Controller-Positionen und -Rotationen
    for (size_t i = 0; i < controllerPositions.size(); ++i) {
        if (isDeviceConnected(i)) {
            controllerPositions[i] = getDevicePosition(i);
            controllerRotations[i] = getDeviceRotation(i);
        }
    }

    // Aktualisiere HMD-Position und -Rotation
    hmdPosition = getDevicePosition(vr::k_unTrackedDeviceIndex_Hmd);
    hmdRotation = getDeviceRotation(vr::k_unTrackedDeviceIndex_Hmd);
}

bool VRInteraction::isControllerButtonPressed(uint32_t controllerIndex, uint32_t button) {
    if (controllerIndex >= controllerPositions.size()) {
        return false;
    }

    vr::VRControllerState_t state;
    if (vrManager.getControllerState(controllerIndex, &state)) {
        return (state.ulButtonPressed & (1ULL << button)) != 0;
    }
    return false;
}

bool VRInteraction::isControllerButtonTouched(uint32_t controllerIndex, uint32_t button) {
    if (controllerIndex >= controllerPositions.size()) {
        return false;
    }

    vr::VRControllerState_t state;
    if (vrManager.getControllerState(controllerIndex, &state)) {
        return (state.ulButtonTouched & (1ULL << button)) != 0;
    }
    return false;
}

glm::vec2 VRInteraction::getControllerAxis(uint32_t controllerIndex, uint32_t axis) {
    if (controllerIndex >= controllerPositions.size()) {
        return glm::vec2(0.0f);
    }

    vr::VRControllerState_t state;
    if (vrManager.getControllerState(controllerIndex, &state)) {
        return glm::vec2(state.rAxis[axis].x, state.rAxis[axis].y);
    }
    return glm::vec2(0.0f);
}

glm::vec3 VRInteraction::getControllerPosition(uint32_t controllerIndex) {
    if (controllerIndex >= controllerPositions.size()) {
        return glm::vec3(0.0f);
    }
    return controllerPositions[controllerIndex];
}

glm::quat VRInteraction::getControllerRotation(uint32_t controllerIndex) {
    if (controllerIndex >= controllerRotations.size()) {
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }
    return controllerRotations[controllerIndex];
}

glm::vec3 VRInteraction::getHMDPosition() {
    return hmdPosition;
}

glm::quat VRInteraction::getHMDRotation() {
    return hmdRotation;
}

float VRInteraction::getIPD() {
    return ipd;
}

void VRInteraction::setIPD(float newIPD) {
    ipd = std::clamp(newIPD, 0.05f, 0.08f);
}

bool VRInteraction::isDeviceConnected(uint32_t deviceIndex) {
    return vrManager.isDeviceConnected(deviceIndex);
}

glm::vec3 VRInteraction::getDevicePosition(uint32_t deviceIndex) {
    vr::TrackedDevicePose_t pose;
    if (vrManager.getDevicePose(deviceIndex, &pose)) {
        return glm::vec3(pose.mDeviceToAbsoluteTracking.m[0][3],
                        pose.mDeviceToAbsoluteTracking.m[1][3],
                        pose.mDeviceToAbsoluteTracking.m[2][3]);
    }
    return glm::vec3(0.0f);
}

glm::quat VRInteraction::getDeviceRotation(uint32_t deviceIndex) {
    vr::TrackedDevicePose_t pose;
    if (vrManager.getDevicePose(deviceIndex, &pose)) {
        glm::mat4 matrix(glm::mat4(
            pose.mDeviceToAbsoluteTracking.m[0][0], pose.mDeviceToAbsoluteTracking.m[1][0],
            pose.mDeviceToAbsoluteTracking.m[2][0], 0.0f,
            pose.mDeviceToAbsoluteTracking.m[0][1], pose.mDeviceToAbsoluteTracking.m[1][1],
            pose.mDeviceToAbsoluteTracking.m[2][1], 0.0f,
            pose.mDeviceToAbsoluteTracking.m[0][2], pose.mDeviceToAbsoluteTracking.m[1][2],
            pose.mDeviceToAbsoluteTracking.m[2][2], 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        ));
        return glm::quat_cast(matrix);
    }
    return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

void VRInteraction::setDeviceTransform(uint32_t deviceIndex, const glm::vec3& position, const glm::quat& rotation) {
    // Diese Funktion ist nur für Testzwecke gedacht
    if (deviceIndex < controllerPositions.size()) {
        controllerPositions[deviceIndex] = position;
        controllerRotations[deviceIndex] = rotation;
    }
}

void VRInteraction::setPlayAreaSize(float width, float length) {
    playAreaSize = glm::vec2(std::max(0.1f, width), std::max(0.1f, length));
}

glm::vec2 VRInteraction::getPlayAreaSize() {
    return playAreaSize;
}

bool VRInteraction::isPositionInPlayArea(const glm::vec3& position) {
    return std::abs(position.x) <= playAreaSize.x / 2.0f &&
           std::abs(position.z) <= playAreaSize.y / 2.0f;
}

void VRInteraction::setChaperoneVisibility(bool visible) {
    chaperoneVisible = visible;
    vrManager.setChaperoneVisibility(visible);
}

bool VRInteraction::isChaperoneVisible() {
    return chaperoneVisible;
}

void VRInteraction::triggerHapticPulse(uint32_t controllerIndex, float duration, float frequency, float amplitude) {
    if (controllerIndex >= hapticActive.size()) {
        return;
    }

    vrManager.triggerHapticPulse(controllerIndex, duration, frequency, amplitude);
    hapticActive[controllerIndex] = true;
}

void VRInteraction::stopHapticPulse(uint32_t controllerIndex) {
    if (controllerIndex >= hapticActive.size()) {
        return;
    }

    vrManager.stopHapticPulse(controllerIndex);
    hapticActive[controllerIndex] = false;
}

} // namespace VR
} // namespace VRMusicStudio 