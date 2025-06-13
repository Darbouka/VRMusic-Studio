#pragma once

#include <openvr.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <string>
#include <vector>
#include <mutex>

namespace VRMusicStudio {
namespace VR {

struct VRDeviceInfo {
    vr::TrackedDeviceIndex_t deviceId;
    std::string deviceName;
    vr::ETrackedDeviceClass deviceClass;
    glm::vec3 position;
    glm::quat rotation;
    bool isConnected;
};

class VRManager {
public:
    static VRManager& getInstance();

    VRManager(const VRManager&) = delete;
    VRManager& operator=(const VRManager&) = delete;
    VRManager(VRManager&&) = delete;
    VRManager& operator=(VRManager&&) = delete;

    bool initialize();
    void shutdown();
    void update();

    bool isVRSystemActive() const;
    bool isHMDConnected() const;
    bool isControllerConnected(vr::ETrackedControllerRole role) const;

    glm::mat4 getHMDPose() const;
    glm::mat4 getControllerPose(vr::ETrackedControllerRole role) const;
    glm::vec3 getHMDPosition() const;
    glm::quat getHMDRotation() const;

    std::vector<VRDeviceInfo> getConnectedDevices() const;
    VRDeviceInfo getDeviceInfo(vr::TrackedDeviceIndex_t deviceId) const;

    void setTrackingSpace(vr::ETrackingUniverseOrigin origin);
    vr::ETrackingUniverseOrigin getTrackingSpace() const;

    void setRenderTargetSize(uint32_t width, uint32_t height);
    void getRenderTargetSize(uint32_t& width, uint32_t& height) const;

    void setIPD(float ipd);
    float getIPD() const;

    void setProjectionMatrix(vr::Hmd_Eye eye, float near, float far);
    glm::mat4 getProjectionMatrix(vr::Hmd_Eye eye) const;
    glm::mat4 getEyeViewMatrix(vr::Hmd_Eye eye) const;

private:
    VRManager();
    ~VRManager();

    void updateDevicePoses();
    void updateHMDMatrixPose();
    void updateControllerPoses();

    bool initialized;
    vr::IVRSystem* vrSystem;
    vr::ETrackingUniverseOrigin trackingSpace;
    uint32_t renderWidth;
    uint32_t renderHeight;
    float ipd;

    glm::mat4 hmdPose;
    glm::mat4 controllerPoses[vr::k_unMaxTrackedDeviceCount];
    std::vector<VRDeviceInfo> connectedDevices;

    mutable std::mutex mutex;
};

} // namespace VR
} // namespace VRMusicStudio 