#include "VRManager.hpp"
#include "core/Logger.hpp"
#include <stdexcept>
#include <spdlog/spdlog.h>

namespace VRMusicStudio {
namespace VR {

VRManager& VRManager::getInstance() {
    static VRManager instance;
    return instance;
}

VRManager::VRManager()
    : initialized(false)
    , vrSystem(nullptr)
    , trackingSpace(vr::TrackingUniverseStanding)
    , renderWidth(0)
    , renderHeight(0)
    , ipd(0.0f) {
    hmdPose = glm::mat4(1.0f);
    for (auto& pose : controllerPoses) {
        pose = glm::mat4(1.0f);
    }
}

VRManager::~VRManager() {
    shutdown();
}

bool VRManager::initialize() {
    std::lock_guard<std::mutex> lock(mutex);

    if (initialized) {
        return true;
    }

    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Initialisiere VR-System...");

        vr::EVRInitError error = vr::VRInitError_None;
        vrSystem = vr::VR_Init(&error, vr::VRApplication_Scene);

        if (error != vr::VRInitError_None) {
            throw std::runtime_error("Fehler bei der VR-Initialisierung: " + std::string(vr::VR_GetVRInitErrorAsEnglishDescription(error)));
        }

        if (!vr::VRCompositor()) {
            throw std::runtime_error("Fehler bei der VR-Compositor-Initialisierung");
        }

        // Setze Standardwerte
        vrSystem->GetRecommendedRenderTargetSize(&renderWidth, &renderHeight);
        ipd = vrSystem->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_UserIpdMeters_Float);

        initialized = true;
        logger.info("VR-System erfolgreich initialisiert");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der VR-Initialisierung: {}", e.what());
        return false;
    }
}

void VRManager::shutdown() {
    std::lock_guard<std::mutex> lock(mutex);

    if (!initialized) {
        return;
    }

    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Beende VR-System...");

        if (vrSystem) {
            vr::VR_Shutdown();
            vrSystem = nullptr;
        }

        initialized = false;
        logger.info("VR-System erfolgreich beendet");
    } catch (const std::exception& e) {
        spdlog::error("Fehler beim Beenden des VR-Systems: {}", e.what());
    }
}

void VRManager::update() {
    std::lock_guard<std::mutex> lock(mutex);

    if (!initialized || !vrSystem) {
        return;
    }

    try {
        vr::VRCompositor()->WaitGetPoses(nullptr, 0, nullptr, 0);
        updateDevicePoses();
        updateHMDMatrixPose();
        updateControllerPoses();
    } catch (const std::exception& e) {
        spdlog::error("Fehler im VR-Update: {}", e.what());
    }
}

bool VRManager::isVRSystemActive() const {
    std::lock_guard<std::mutex> lock(mutex);
    return initialized && vrSystem != nullptr;
}

bool VRManager::isHMDConnected() const {
    std::lock_guard<std::mutex> lock(mutex);
    return initialized && vrSystem && vrSystem->IsTrackedDeviceConnected(vr::k_unTrackedDeviceIndex_Hmd);
}

bool VRManager::isControllerConnected(vr::ETrackedControllerRole role) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (!initialized || !vrSystem) {
        return false;
    }

    vr::TrackedDeviceIndex_t controllerId = vrSystem->GetTrackedDeviceIndexForControllerRole(role);
    return controllerId != vr::k_unTrackedDeviceIndexInvalid;
}

glm::mat4 VRManager::getHMDPose() const {
    std::lock_guard<std::mutex> lock(mutex);
    return hmdPose;
}

glm::mat4 VRManager::getControllerPose(vr::ETrackedControllerRole role) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (!initialized || !vrSystem) {
        return glm::mat4(1.0f);
    }

    vr::TrackedDeviceIndex_t controllerId = vrSystem->GetTrackedDeviceIndexForControllerRole(role);
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
    if (!initialized || !vrSystem || deviceId >= vr::k_unMaxTrackedDeviceCount) {
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
    if (!initialized || !vrSystem) {
        return;
    }

    vr::HmdMatrix44_t projectionMatrix = vrSystem->GetProjectionMatrix(eye, near, far);
    // Konvertiere die Matrix in glm::mat4
}

glm::mat4 VRManager::getProjectionMatrix(vr::Hmd_Eye eye) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (!initialized || !vrSystem) {
        return glm::mat4(1.0f);
    }

    vr::HmdMatrix44_t projectionMatrix = vrSystem->GetProjectionMatrix(eye, 0.1f, 100.0f);
    return glm::mat4(
        projectionMatrix.m[0][0], projectionMatrix.m[1][0], projectionMatrix.m[2][0], projectionMatrix.m[3][0],
        projectionMatrix.m[0][1], projectionMatrix.m[1][1], projectionMatrix.m[2][1], projectionMatrix.m[3][1],
        projectionMatrix.m[0][2], projectionMatrix.m[1][2], projectionMatrix.m[2][2], projectionMatrix.m[3][2],
        projectionMatrix.m[0][3], projectionMatrix.m[1][3], projectionMatrix.m[2][3], projectionMatrix.m[3][3]
    );
}

glm::mat4 VRManager::getEyeViewMatrix(vr::Hmd_Eye eye) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (!initialized || !vrSystem) {
        return glm::mat4(1.0f);
    }

    vr::HmdMatrix34_t eyeMatrix = vrSystem->GetEyeToHeadTransform(eye);
    return glm::mat4(
        eyeMatrix.m[0][0], eyeMatrix.m[1][0], eyeMatrix.m[2][0], 0.0f,
        eyeMatrix.m[0][1], eyeMatrix.m[1][1], eyeMatrix.m[2][1], 0.0f,
        eyeMatrix.m[0][2], eyeMatrix.m[1][2], eyeMatrix.m[2][2], 0.0f,
        eyeMatrix.m[0][3], eyeMatrix.m[1][3], eyeMatrix.m[2][3], 1.0f
    );
}

void VRManager::updateDevicePoses() {
    if (!initialized || !vrSystem) {
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
        deviceInfo.deviceClass = vrSystem->GetTrackedDeviceClass(deviceId);
        deviceInfo.isConnected = true;

        char deviceName[vr::k_unMaxPropertyStringSize];
        vrSystem->GetStringTrackedDeviceProperty(deviceId, vr::Prop_TrackingSystemName_String, deviceName, vr::k_unMaxPropertyStringSize);
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
    if (!initialized || !vrSystem) {
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
    if (!initialized || !vrSystem) {
        return;
    }

    for (vr::TrackedDeviceIndex_t deviceId = 0; deviceId < vr::k_unMaxTrackedDeviceCount; deviceId++) {
        if (vrSystem->GetTrackedDeviceClass(deviceId) != vr::TrackedDeviceClass_Controller) {
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