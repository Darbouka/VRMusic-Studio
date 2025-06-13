#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace VRMusicStudio {
namespace VR {

class VRInteraction {
public:
    VRInteraction();
    ~VRInteraction();

    // Initialisierung und Shutdown
    bool initialize();
    void shutdown();

    // Controller-Interaktionen
    glm::vec3 getControllerPosition(int controllerIndex) const;
    glm::quat getControllerRotation(int controllerIndex) const;
    bool isButtonPressed(int controllerIndex, int buttonId) const;
    float getTriggerValue(int controllerIndex) const;
    float getGripValue(int controllerIndex) const;

    // Raum-Interaktionen
    void setRoomScale(bool enabled);
    bool isRoomScaleEnabled() const;
    void setPlayAreaSize(float width, float length);
    glm::vec2 getPlayAreaSize() const;

    // Objekt-Interaktionen
    bool grabObject(const std::string& objectId);
    void releaseObject(const std::string& objectId);
    bool isObjectGrabbed(const std::string& objectId) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace VR
} // namespace VRMusicStudio 