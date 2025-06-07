#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace VR_DAW {

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

    bool initialize(int deviceIndex);
    void shutdown();
    void update();

    // Controller-Identifikation
    const std::string& getId() const { return id; }
    int getDeviceIndex() const { return deviceIndex; }

    // Controller-Status
    bool isConnected() const { return connected; }
    bool isTracking() const { return tracking; }
    const glm::vec3& getPosition() const { return position; }
    const glm::quat& getOrientation() const { return orientation; }

    // Eingabe-Handling
    bool handleInput(const std::string& action);
    const std::vector<ControllerEvent>& getEvents() const { return events; }
    void clearEvents() { events.clear(); }

    // Button-Status
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
    bool tracking;
    glm::vec3 position;
    glm::quat orientation;
    std::vector<ControllerEvent> events;
    glm::vec2 touchpadPosition;
    float triggerValue;

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

} // namespace VR_DAW
