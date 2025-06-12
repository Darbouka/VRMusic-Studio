#include "VRController.hpp"
#include <spdlog/spdlog.h>
<<<<<<< HEAD
#include <random>
#include <sstream>
#include <iomanip>

namespace VR_DAW {

VRController::VRController()
    : deviceIndex(-1)
    , connected(false)
    , tracking(false)
    , position(0.0f)
    , orientation(1.0f, 0.0f, 0.0f, 0.0f)
    , touchpadPosition(0.0f)
    , triggerValue(0.0f)
    , triggerPressed(false)
    , gripPressed(false)
    , touchpadTouched(false)
{
    generateId();
=======
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <random>
#include <sstream>
#include <iomanip>
#include <map>

namespace VRMusicStudio {

struct VRController::Impl {
    vr::IVRSystem* vrSystem;
    vr::TrackedDevicePose_t devicePoses[vr::k_unMaxTrackedDeviceCount];
    vr::VRControllerState_t controllerState;
    std::map<std::string, std::vector<vr::EVRButtonId>> registeredGestures;
    std::map<std::string, bool> activeGestures;
    float currentHapticIntensity;
    float currentHapticDuration;
    bool hapticActive;
    uint32_t deviceIndex;
};

VRController::VRController() : pImpl(std::make_unique<Impl>()) {
    pImpl->vrSystem = nullptr;
    pImpl->currentHapticIntensity = 0.0f;
    pImpl->currentHapticDuration = 0.0f;
    pImpl->hapticActive = false;
    pImpl->deviceIndex = 0;
>>>>>>> 0dff1c4 (init 2)
}

VRController::~VRController() {
    shutdown();
}

void VRController::generateId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    const char* hex = "0123456789abcdef";
    
    std::stringstream ss;
    for (int i = 0; i < 32; ++i) {
        ss << hex[dis(gen)];
    }
    id = ss.str();
}

<<<<<<< HEAD
bool VRController::initialize(int index) {
    deviceIndex = index;
    
    // Hier würde die OpenVR-Controller-Initialisierung erfolgen
    // vr::VRSystem()->GetTrackedDeviceClass(deviceIndex) == vr::TrackedDeviceClass_Controller
    
    connected = true;
    tracking = true;
    
    spdlog::info("VR-Controller initialisiert: {}", id);
=======
bool VRController::initialize() {
    vr::EVRInitError error = vr::VRInitError_None;
    pImpl->vrSystem = vr::VR_Init(&error, vr::VRApplication_Scene);
    
    if (error != vr::VRInitError_None) {
        spdlog::error("OpenVR Initialisierungsfehler: {}", vr::VR_GetVRInitErrorAsEnglishDescription(error));
        return false;
    }
    
    // Controller-Index finden
    for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
        if (pImpl->vrSystem->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_Controller) {
            pImpl->deviceIndex = i;
            break;
        }
    }
    
    spdlog::info("VR-Controller initialisiert");
>>>>>>> 0dff1c4 (init 2)
    return true;
}

void VRController::shutdown() {
<<<<<<< HEAD
    if (connected) {
        connected = false;
        tracking = false;
        spdlog::info("VR-Controller beendet: {}", id);
=======
    if (pImpl->vrSystem) {
        vr::VR_Shutdown();
        pImpl->vrSystem = nullptr;
>>>>>>> 0dff1c4 (init 2)
    }
}

void VRController::update() {
<<<<<<< HEAD
    if (!connected) return;

    // Hier würde die OpenVR-Controller-Aktualisierung erfolgen
    // vr::VRSystem()->GetDeviceToAbsoluteTrackingPose()
    // vr::VRSystem()->GetControllerState()

    updateButtonStates();
    processEvents();
}

bool VRController::handleInput(const std::string& action) {
    if (!connected) return false;

    // Hier würde die Aktion basierend auf dem Controller-Status verarbeitet werden
    // z.B. Audio-Steuerung, UI-Interaktionen, etc.
    
    return true;
}

bool VRController::isButtonPressed(const std::string& button) const {
    auto it = buttonStates.find(button);
    return it != buttonStates.end() && it->second;
}

bool VRController::isTriggerPressed() const {
    return triggerPressed;
}

bool VRController::isGripPressed() const {
    return gripPressed;
}

bool VRController::isTouchpadTouched() const {
    return touchpadTouched;
}

void VRController::updateButtonStates() {
    // Hier würden die Button-Status von OpenVR aktualisiert werden
    // vr::VRSystem()->GetControllerState()
}

void VRController::processEvents() {
    // Hier würden die Controller-Events verarbeitet werden
    // z.B. Button-Press/Release, Trigger-Werte, Touchpad-Position, etc.
}

} // namespace VR_DAW
=======
    if (!pImpl->vrSystem) return;
    
    // Controller-Status aktualisieren
    pImpl->vrSystem->GetDeviceToAbsoluteTrackingPose(
        vr::TrackingUniverseStanding,
        0.0f,
        pImpl->devicePoses,
        vr::k_unMaxTrackedDeviceCount
    );
    
    // Controller-Status abrufen
    pImpl->vrSystem->GetControllerState(
        pImpl->deviceIndex,
        &pImpl->controllerState,
        sizeof(vr::VRControllerState_t)
    );
    
    // Gesten aktualisieren
    for (const auto& [gestureName, buttons] : pImpl->registeredGestures) {
        bool allButtonsPressed = true;
        for (const auto& button : buttons) {
            if (!(pImpl->controllerState.ulButtonPressed & vr::ButtonMaskFromId(button))) {
                allButtonsPressed = false;
                break;
            }
        }
        pImpl->activeGestures[gestureName] = allButtonsPressed;
    }
    
    // Haptisches Feedback aktualisieren
    if (pImpl->hapticActive) {
        pImpl->currentHapticDuration -= 0.016f; // ~60fps
        if (pImpl->currentHapticDuration <= 0.0f) {
            pImpl->hapticActive = false;
        } else {
            triggerHapticPulse(static_cast<uint16_t>(pImpl->currentHapticIntensity * 3999.0f));
        }
    }
}

bool VRController::isConnected() const {
    return pImpl->vrSystem && 
           pImpl->devicePoses[pImpl->deviceIndex].bDeviceIsConnected;
}

glm::vec3 VRController::getPosition() const {
    if (!isConnected()) return glm::vec3(0.0f);
    
    const auto& pose = pImpl->devicePoses[pImpl->deviceIndex];
    return glm::vec3(
        pose.mDeviceToAbsoluteTracking.m[0][3],
        pose.mDeviceToAbsoluteTracking.m[1][3],
        pose.mDeviceToAbsoluteTracking.m[2][3]
    );
}

glm::quat VRController::getRotation() const {
    if (!isConnected()) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    
    const auto& pose = pImpl->devicePoses[pImpl->deviceIndex];
    glm::mat4 mat(
        pose.mDeviceToAbsoluteTracking.m[0][0], pose.mDeviceToAbsoluteTracking.m[1][0], pose.mDeviceToAbsoluteTracking.m[2][0], 0.0f,
        pose.mDeviceToAbsoluteTracking.m[0][1], pose.mDeviceToAbsoluteTracking.m[1][1], pose.mDeviceToAbsoluteTracking.m[2][1], 0.0f,
        pose.mDeviceToAbsoluteTracking.m[0][2], pose.mDeviceToAbsoluteTracking.m[1][2], pose.mDeviceToAbsoluteTracking.m[2][2], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    return glm::quat_cast(mat);
}

bool VRController::isButtonPressed(vr::EVRButtonId button) const {
    return (pImpl->controllerState.ulButtonPressed & vr::ButtonMaskFromId(button)) != 0;
}

float VRController::getTriggerValue() const {
    return pImpl->controllerState.rAxis[1].x;
}

glm::vec2 VRController::getTrackpadPosition() const {
    return glm::vec2(
        pImpl->controllerState.rAxis[0].x,
        pImpl->controllerState.rAxis[0].y
    );
}

void VRController::triggerHapticPulse(uint16_t durationMicros) {
    if (!isConnected()) return;
    pImpl->vrSystem->TriggerHapticPulse(pImpl->deviceIndex, 0, durationMicros);
}

void VRController::setHapticFeedback(float intensity, float duration) {
    pImpl->currentHapticIntensity = glm::clamp(intensity, 0.0f, 1.0f);
    pImpl->currentHapticDuration = duration;
    pImpl->hapticActive = true;
}

bool VRController::isGestureActive(const std::string& gestureName) const {
    auto it = pImpl->activeGestures.find(gestureName);
    return it != pImpl->activeGestures.end() && it->second;
}

void VRController::registerGesture(const std::string& gestureName,
                                 const std::vector<vr::EVRButtonId>& requiredButtons) {
    pImpl->registeredGestures[gestureName] = requiredButtons;
    pImpl->activeGestures[gestureName] = false;
}

} // namespace VRMusicStudio
>>>>>>> 0dff1c4 (init 2)
