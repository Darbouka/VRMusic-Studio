#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <openvr.h>

namespace VRMusicStudio {
namespace VR {

class VRInteraction {
public:
    VRInteraction();
    ~VRInteraction();

    bool initialize();
    void shutdown();
    void update();

    glm::vec3 getControllerPosition(int controllerIndex) const;
    glm::quat getControllerRotation(int controllerIndex) const;
    bool isControllerButtonPressed(int controllerIndex, vr::EVRButtonId button) const;

private:
    vr::IVRSystem* m_vrSystem;
    vr::TrackedDevicePose_t m_trackedDevicePoses[vr::k_unMaxTrackedDeviceCount];
    bool m_isInitialized;
};

} // namespace VR
} // namespace VRMusicStudio 