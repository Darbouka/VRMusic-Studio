#include "VRManager.hpp"
#include <openvr.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <stdexcept>
#include <spdlog/spdlog.h>

namespace VRMusicStudio {
namespace VR {

VRManager& VRManager::getInstance() {
    static VRManager instance;
    return instance;
}

VRManager::VRManager()
    : m_isInitialized(false)
    , m_hmd(nullptr)
    , trackingSpace(vr::TrackingUniverseStanding)
    , renderWidth(0)
    , renderHeight(0)
    , ipd(0.0f) {
    hmdPose = glm::mat4(1.0f);
    for (auto& pose : controllerPoses) {
        pose = glm::mat4(1.0f);
    }
    initialize();
}

VRManager::~VRManager() {
    if (m_isInitialized) {
        shutdown();
    }
}

void VRManager::initialize() {
    vr::EVRInitError error = vr::VRInitError_None;
    m_hmd = vr::VR_Init(&error, vr::VRApplication_Scene);

    if (error != vr::VRInitError_None) {
        throw std::runtime_error("Failed to initialize OpenVR: " + std::string(vr::VR_GetVRInitErrorAsEnglishDescription(error)));
    }

    m_isInitialized = true;
    spdlog::info("VR system initialized successfully");
}

void VRManager::shutdown() {
    if (m_isInitialized) {
        vr::VR_Shutdown();
        m_hmd = nullptr;
        m_isInitialized = false;
        spdlog::info("VR system shut down");
    }
}

void VRManager::update() {
    if (!m_isInitialized) return;

    vr::VREvent_t event;
    while (m_hmd->PollNextEvent(&event, sizeof(event))) {
        processVREvent(event);
    }

    updateTracking();
}

void VRManager::processVREvent(const vr::VREvent_t& event) {
    switch (event.eventType) {
        case vr::VREvent_TrackedDeviceActivated:
            handleDeviceActivated(event.trackedDeviceIndex);
            break;
        case vr::VREvent_TrackedDeviceDeactivated:
            handleDeviceDeactivated(event.trackedDeviceIndex);
            break;
        case vr::VREvent_ButtonPress:
            handleButtonPress(event.trackedDeviceIndex, event.data.controller.button);
            break;
        case vr::VREvent_ButtonUnpress:
            handleButtonUnpress(event.trackedDeviceIndex, event.data.controller.button);
            break;
    }
}

void VRManager::updateTracking() {
    if (!m_isInitialized) return;

    vr::VRCompositor()->WaitGetPoses(m_trackedDevicePoses, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

    for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i) {
        if (m_trackedDevicePoses[i].bPoseIsValid) {
            updateDevicePose(i, m_trackedDevicePoses[i]);
        }
    }
}

void VRManager::updateDevicePose(uint32_t deviceIndex, const vr::TrackedDevicePose_t& pose) {
    glm::mat4 deviceMatrix = convertSteamVRMatrixToGLMMatrix(pose.mDeviceToAbsoluteTracking);
    m_devicePoses[deviceIndex] = deviceMatrix;
}

glm::mat4 VRManager::convertSteamVRMatrixToGLMMatrix(const vr::HmdMatrix34_t& matrix) {
    glm::mat4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i][j] = matrix.m[j][i];
        }
    }
    return result;
}

void VRManager::handleDeviceActivated(uint32_t deviceIndex) {
    spdlog::info("VR device activated: {}", deviceIndex);
    // Implement device activation logic
}

void VRManager::handleDeviceDeactivated(uint32_t deviceIndex) {
    spdlog::info("VR device deactivated: {}", deviceIndex);
    // Implement device deactivation logic
}

void VRManager::handleButtonPress(uint32_t deviceIndex, uint32_t button) {
    spdlog::info("VR button pressed: device={}, button={}", deviceIndex, button);
    // Implement button press logic
}

void VRManager::handleButtonUnpress(uint32_t deviceIndex, uint32_t button) {
    spdlog::info("VR button released: device={}, button={}", deviceIndex, button);
    // Implement button release logic
}

glm::mat4 VRManager::getDevicePose(uint32_t deviceIndex) const {
    auto it = m_devicePoses.find(deviceIndex);
    if (it != m_devicePoses.end()) {
        return it->second;
    }
    return glm::mat4(1.0f);
}

bool VRManager::isDeviceConnected(uint32_t deviceIndex) const {
    if (!m_isInitialized) return false;
    return m_hmd->IsTrackedDeviceConnected(deviceIndex);
}

std::string VRManager::getDeviceName(uint32_t deviceIndex) const {
    if (!m_isInitialized) return "";

    char buffer[vr::k_unMaxPropertyStringSize];
    vr::ETrackedPropertyError error;
    m_hmd->GetStringTrackedDeviceProperty(deviceIndex, vr::Prop_TrackingSystemName_String, buffer, vr::k_unMaxPropertyStringSize, &error);
    
    if (error == vr::TrackedProp_Success) {
        return std::string(buffer);
    }
    return "";
}

bool VRManager::isVRSystemActive() const {
    std::lock_guard<std::mutex> lock(mutex);
    return m_isInitialized && m_hmd != nullptr;
}

bool VRManager::isHMDConnected() const {
    std::lock_guard<std::mutex> lock(mutex);
    return m_isInitialized && m_hmd && m_hmd->IsTrackedDeviceConnected(vr::k_unTrackedDeviceIndex_Hmd);
}

bool VRManager::isControllerConnected(vr::ETrackedControllerRole role) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (!m_isInitialized || !m_hmd) {
        return false;
    }

    vr::TrackedDeviceIndex_t controllerId = m_hmd->GetTrackedDeviceIndexForControllerRole(role);
    return controllerId != vr::k_unTrackedDeviceIndexInvalid;
}

glm::mat4 VRManager::getHMDPose() const {
    std::lock_guard<std::mutex> lock(mutex);
    return hmdPose;
}

glm::mat4 VRManager::getControllerPose(vr::ETrackedControllerRole role) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (!m_isInitialized || !m_hmd) {
        return glm::mat4(1.0f);
    }

    vr::TrackedDeviceIndex_t controllerId = m_hmd->GetTrackedDeviceIndexForControllerRole(role);
    if (controllerId == vr::k_unTrackedDeviceIndexInvalid) {
        return glm::mat4(1.0f);
    }

    return controllerPoses[controllerId];
}

glm::vec3 VRManager::getHMDPosition() const {
    std::lock_guard<std::mutex> lock(mutex);
    return glm::vec3(hmdPose[3]);
}

glm::quat VRManager::getHMDRotation() const {
    std::lock_guard<std::mutex> lock(mutex);
    return glm::quat_cast(glm::mat3(hmdPose));
}

std::vector<VRDeviceInfo> VRManager::getConnectedDevices() const {
    std::lock_guard<std::mutex> lock(mutex);
    return connectedDevices;
}

VRDeviceInfo VRManager::getDeviceInfo(vr::TrackedDeviceIndex_t deviceId) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (!m_isInitialized || !m_hmd || deviceId >= vr::k_unMaxTrackedDeviceCount) {
        return VRDeviceInfo{};
    }

    for (const auto& device : connectedDevices) {
        if (device.deviceId == deviceId) {
            return device;
        }
    }

    return VRDeviceInfo{};
}

void VRManager::setTrackingSpace(vr::ETrackingUniverseOrigin origin) {
    std::lock_guard<std::mutex> lock(mutex);
    trackingSpace = origin;
}

vr::ETrackingUniverseOrigin VRManager::getTrackingSpace() const {
    std::lock_guard<std::mutex> lock(mutex);
    return trackingSpace;
}

void VRManager::setRenderTargetSize(uint32_t width, uint32_t height) {
    std::lock_guard<std::mutex> lock(mutex);
    renderWidth = width;
    renderHeight = height;
}

void VRManager::getRenderTargetSize(uint32_t& width, uint32_t& height) const {
    std::lock_guard<std::mutex> lock(mutex);
    width = renderWidth;
    height = renderHeight;
}

void VRManager::setIPD(float newIpd) {
    std::lock_guard<std::mutex> lock(mutex);
    ipd = newIpd;
}

float VRManager::getIPD() const {
    std::lock_guard<std::mutex> lock(mutex);
    return ipd;
}

void VRManager::setProjectionMatrix(vr::Hmd_Eye eye, float near, float far) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!m_isInitialized || !m_hmd) {
        return;
    }

    vr::HmdMatrix44_t projectionMatrix = m_hmd->GetProjectionMatrix(eye, near, far);
    // Konvertiere die Matrix in glm::mat4
}

glm::mat4 VRManager::getProjectionMatrix(vr::Hmd_Eye eye) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (!m_isInitialized || !m_hmd) {
        return glm::mat4(1.0f);
    }

    vr::HmdMatrix44_t projectionMatrix = m_hmd->GetProjectionMatrix(eye, 0.1f, 100.0f);
    return glm::mat4(
        projectionMatrix.m[0][0], projectionMatrix.m[1][0], projectionMatrix.m[2][0], projectionMatrix.m[3][0],
        projectionMatrix.m[0][1], projectionMatrix.m[1][1], projectionMatrix.m[2][1], projectionMatrix.m[3][1],
        projectionMatrix.m[0][2], projectionMatrix.m[1][2], projectionMatrix.m[2][2], projectionMatrix.m[3][2],
        projectionMatrix.m[0][3], projectionMatrix.m[1][3], projectionMatrix.m[2][3], projectionMatrix.m[3][3]
    );
}

glm::mat4 VRManager::getEyeViewMatrix(vr::Hmd_Eye eye) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (!m_isInitialized || !m_hmd) {
        return glm::mat4(1.0f);
    }

    vr::HmdMatrix34_t eyeMatrix = m_hmd->GetEyeToHeadTransform(eye);
    return glm::mat4(
        eyeMatrix.m[0][0], eyeMatrix.m[1][0], eyeMatrix.m[2][0], 0.0f,
        eyeMatrix.m[0][1], eyeMatrix.m[1][1], eyeMatrix.m[2][1], 0.0f,
        eyeMatrix.m[0][2], eyeMatrix.m[1][2], eyeMatrix.m[2][2], 0.0f,
        eyeMatrix.m[0][3], eyeMatrix.m[1][3], eyeMatrix.m[2][3], 1.0f
    );
}

void VRManager::updateDevicePoses() {
    if (!m_isInitialized || !m_hmd) {
        return;
    }

    vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount];
    vr::VRCompositor()->GetLastPoses(poses, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

    connectedDevices.clear();
    for (vr::TrackedDeviceIndex_t deviceId = 0; deviceId < vr::k_unMaxTrackedDeviceCount; deviceId++) {
        if (!poses[deviceId].bDeviceIsConnected) {
            continue;
        }

        VRDeviceInfo deviceInfo;
        deviceInfo.deviceId = deviceId;
        deviceInfo.deviceClass = m_hmd->GetTrackedDeviceClass(deviceId);
        deviceInfo.isConnected = true;

        char deviceName[vr::k_unMaxPropertyStringSize];
        m_hmd->GetStringTrackedDeviceProperty(deviceId, vr::Prop_TrackingSystemName_String, deviceName, vr::k_unMaxPropertyStringSize);
        deviceInfo.deviceName = deviceName;

        if (poses[deviceId].bPoseIsValid) {
            const vr::HmdMatrix34_t& matrix = poses[deviceId].mDeviceToAbsoluteTracking;
            deviceInfo.position = glm::vec3(matrix.m[0][3], matrix.m[1][3], matrix.m[2][3]);
            deviceInfo.rotation = glm::quat_cast(glm::mat3(
                matrix.m[0][0], matrix.m[1][0], matrix.m[2][0],
                matrix.m[0][1], matrix.m[1][1], matrix.m[2][1],
                matrix.m[0][2], matrix.m[1][2], matrix.m[2][2]
            ));
        }

        connectedDevices.push_back(deviceInfo);
    }
}

void VRManager::updateHMDMatrixPose() {
    if (!m_isInitialized || !m_hmd) {
        return;
    }

    vr::TrackedDevicePose_t hmdPose;
    vr::VRCompositor()->GetLastPoses(&hmdPose, 1, nullptr, 0);

    if (hmdPose.bPoseIsValid) {
        const vr::HmdMatrix34_t& matrix = hmdPose.mDeviceToAbsoluteTracking;
        this->hmdPose = glm::mat4(
            matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], 0.0f,
            matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], 0.0f,
            matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], 0.0f,
            matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], 1.0f
        );
    }
}

void VRManager::updateControllerPoses() {
    if (!m_isInitialized || !m_hmd) {
        return;
    }

    for (vr::TrackedDeviceIndex_t deviceId = 0; deviceId < vr::k_unMaxTrackedDeviceCount; deviceId++) {
        if (m_hmd->GetTrackedDeviceClass(deviceId) != vr::TrackedDeviceClass_Controller) {
            continue;
        }

        vr::TrackedDevicePose_t controllerPose;
        vr::VRCompositor()->GetLastPoses(&controllerPose, 1, nullptr, 0);

        if (controllerPose.bPoseIsValid) {
            const vr::HmdMatrix34_t& matrix = controllerPose.mDeviceToAbsoluteTracking;
            controllerPoses[deviceId] = glm::mat4(
                matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], 0.0f,
                matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], 0.0f,
                matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], 0.0f,
                matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], 1.0f
            );
        }
    }
}

} // namespace VR
} // namespace VRMusicStudio 