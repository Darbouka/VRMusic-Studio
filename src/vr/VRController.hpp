#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
<<<<<<< HEAD

namespace VR_DAW {
=======
#include <map>
#include <openvr.h>

namespace VRMusicStudio {
>>>>>>> 0dff1c4 (init 2)

struct ControllerEvent {
    enum class Type {
        ButtonPress,
        ButtonRelease,
        TriggerPress,
        TriggerRelease,
        GripPress,
        GripRelease,
        TouchpadTouch,
        TouchpadRelease,
        TouchpadMove
    };

    Type type;
    std::string button;
    glm::vec2 touchpadPosition;
    float triggerValue;
    double timestamp;
};

class VRController {
public:
    VRController();
    ~VRController();

<<<<<<< HEAD
    bool initialize(int deviceIndex);
    void shutdown();
    void update();
=======
    bool initialize();
    void update();
    void shutdown();

    // Controller Status
    bool isConnected() const;
    glm::vec3 getPosition() const;
    glm::quat getRotation() const;
    bool isButtonPressed(vr::EVRButtonId button) const;
    float getTriggerValue() const;
    glm::vec2 getTrackpadPosition() const;

    // Haptisches Feedback
    void triggerHapticPulse(uint16_t durationMicros);
    void setHapticFeedback(float intensity, float duration);

    // Gestenerkennung
    bool isGestureActive(const std::string& gestureName) const;
    void registerGesture(const std::string& gestureName, 
                        const std::vector<vr::EVRButtonId>& requiredButtons);
>>>>>>> 0dff1c4 (init 2)

    // Controller-Identifikation
    const std::string& getId() const { return id; }
    int getDeviceIndex() const { return deviceIndex; }

    // Controller-Status
<<<<<<< HEAD
    bool isConnected() const { return connected; }
=======
>>>>>>> 0dff1c4 (init 2)
    bool isTracking() const { return tracking; }
    const glm::vec3& getPosition() const { return position; }
    const glm::quat& getOrientation() const { return orientation; }

    // Eingabe-Handling
    bool handleInput(const std::string& action);
    const std::vector<ControllerEvent>& getEvents() const { return events; }
    void clearEvents() { events.clear(); }

    // Button-Status
<<<<<<< HEAD
    bool isButtonPressed(const std::string& button) const;
    bool isTriggerPressed() const;
    bool isGripPressed() const;
    bool isTouchpadTouched() const;
    const glm::vec2& getTouchpadPosition() const { return touchpadPosition; }
    float getTriggerValue() const { return triggerValue; }

private:
    std::string id;
    int deviceIndex;
    bool connected;
=======
    bool isGripPressed() const;
    bool isTouchpadTouched() const;
    const glm::vec2& getTouchpadPosition() const { return touchpadPosition; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // OpenVR spezifische Variablen
    vr::IVRSystem* vrSystem;
    vr::TrackedDevicePose_t devicePoses[vr::k_unMaxTrackedDeviceCount];
    vr::VRControllerState_t controllerState;
    
    // Gesten-Tracking
    std::map<std::string, std::vector<vr::EVRButtonId>> registeredGestures;
    std::map<std::string, bool> activeGestures;
    
    // Haptisches Feedback
    float currentHapticIntensity;
    float currentHapticDuration;
    bool hapticActive;

    std::string id;
    int deviceIndex;
>>>>>>> 0dff1c4 (init 2)
    bool tracking;
    glm::vec3 position;
    glm::quat orientation;
    std::vector<ControllerEvent> events;
    glm::vec2 touchpadPosition;
<<<<<<< HEAD
    float triggerValue;
=======
>>>>>>> 0dff1c4 (init 2)

    // Button-Status
    std::map<std::string, bool> buttonStates;
    bool triggerPressed;
    bool gripPressed;
    bool touchpadTouched;

    // Hilfsfunktionen
    void generateId();
    void updateButtonStates();
    void processEvents();
};

<<<<<<< HEAD
} // namespace VR_DAW
=======
} // namespace VRMusicStudio
>>>>>>> 0dff1c4 (init 2)
