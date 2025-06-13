#pragma once

#include <memory>
#include <vector>
#include <string>
#include <mutex>
#include <openvr.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../core/Logger.hpp"
#include "../core/EventSystem.hpp"

namespace VR_DAW {

struct VRDevice {
    vr::TrackedDeviceIndex_t index;
    std::string name;
    glm::vec3 position;
    glm::quat rotation;
    bool isConnected;
    bool isTracking;
};

class VRManager {
public:
    static VRManager& getInstance();

    bool initialize();
    void shutdown();
    bool isInitialized() const;

    // VR-System-Verwaltung
    bool update();
    void resetSeatedPosition();
    
    // Controller-Verwaltung
    std::vector<VRDevice> getControllers() const;
    bool isControllerConnected(vr::TrackedDeviceIndex_t controllerIndex) const;
    glm::vec3 getControllerPosition(vr::TrackedDeviceIndex_t controllerIndex) const;
    glm::quat getControllerRotation(vr::TrackedDeviceIndex_t controllerIndex) const;
    
    // HMD-Verwaltung
    VRDevice getHMD() const;
    glm::mat4 getHMDPose() const;
    glm::mat4 getProjectionMatrix(vr::Hmd_Eye eye) const;
    glm::mat4 getEyeToHeadTransform(vr::Hmd_Eye eye) const;
    
    // Event-Handling
    void processVREvents();
    bool pollNextEvent(vr::VREvent_t& event);

private:
    VRManager();
    ~VRManager();
    VRManager(const VRManager&) = delete;
    VRManager& operator=(const VRManager&) = delete;

    bool initializeOpenVR();
    void shutdownOpenVR();
    VRDevice createDeviceInfo(vr::TrackedDeviceIndex_t deviceIndex);
    glm::mat4 convertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t& matPose);
    glm::mat4 convertSteamVRMatrixToMatrix4(const vr::HmdMatrix44_t& mat);

    vr::IVRSystem* vrSystem;
    bool initialized;
    std::mutex mutex;
    Logger logger;
    std::vector<VRDevice> controllers;
    VRDevice hmd;
};

} // namespace VR_DAW 