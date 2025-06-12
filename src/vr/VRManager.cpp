#include "VRManager.hpp"
#include <stdexcept>
#include <spdlog/spdlog.h>

namespace VR_DAW {

<<<<<<< HEAD
std::unique_ptr<VRManager> VRManager::instance = nullptr;

VRManager& VRManager::getInstance() {
    if (!instance) {
        instance = std::unique_ptr<VRManager>(new VRManager());
    }
    return *instance;
=======
VRManager& VRManager::getInstance() {
    static VRManager instance;
    return instance;
>>>>>>> 0dff1c4 (init 2)
}

VRManager::VRManager()
    : vrSystem(nullptr)
    , initialized(false)
<<<<<<< HEAD
    , running(false)
    , listenerPosition(0.0f)
    , listenerOrientation(1.0f, 0.0f, 0.0f, 0.0f)
=======
>>>>>>> 0dff1c4 (init 2)
    , logger("VRManager") {
}

VRManager::~VRManager() {
    shutdown();
}

bool VRManager::initialize() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (initialized) return true;

    try {
        if (!initializeOpenVR()) {
            logger.error("Fehler bei der OpenVR-Initialisierung");
            return false;
        }

        // HMD-Informationen abrufen
        hmd = createDeviceInfo(vr::k_unTrackedDeviceIndex_Hmd);
        
        // Controller-Informationen abrufen
        controllers.clear();
        for (vr::TrackedDeviceIndex_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i) {
            if (vrSystem->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_Controller) {
                controllers.push_back(createDeviceInfo(i));
            }
        }

        // Audio-Engine initialisieren
        if (!AudioEngine::getInstance().initialize()) {
            spdlog::error("Konnte Audio-Engine nicht initialisieren");
            return false;
        }

        initialized = true;
<<<<<<< HEAD
        running = true;
=======
>>>>>>> 0dff1c4 (init 2)
        logger.info("VR-Manager erfolgreich initialisiert");
        return true;
    }
    catch (const std::exception& e) {
        logger.error("Fehler bei der VR-Manager-Initialisierung: {}", e.what());
        return false;
    }
}

void VRManager::shutdown() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        shutdownOpenVR();
        AudioEngine::getInstance().shutdown();
        controllers.clear();
        hmd = VRDevice();
        initialized = false;
<<<<<<< HEAD
        running = false;
=======
>>>>>>> 0dff1c4 (init 2)
        logger.info("VR-Manager erfolgreich beendet");
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Beenden des VR-Managers: {}", e.what());
    }
}

bool VRManager::isInitialized() const {
    std::lock_guard<std::mutex> lock(mutex);
    return initialized;
}

<<<<<<< HEAD
void VRManager::update() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized || !running) return;

    try {
        processVREvents();

        // HMD-Status aktualisieren
        hmd = createDeviceInfo(vr::k_unTrackedDeviceIndex_Hmd);

        // Controller-Status aktualisieren
        for (auto& controller : controllers) {
            controller = createDeviceInfo(controller.index);
        }

        // Audio-Positionen aktualisieren
        updateAudioPositions();

        // Audio-Engine aktualisieren
        updateAudioEngine();

        return true;
    }
    catch (const std::exception& e) {
=======
bool VRManager::update() {
    std::lock_guard<std::mutex> lock(mutex);
    if (!initialized) return false;
    try {
        processVREvents();
        hmd = createDeviceInfo(vr::k_unTrackedDeviceIndex_Hmd);
        for (auto& controller : controllers) {
            controller = createDeviceInfo(controller.index);
        }
        return true;
    } catch (const std::exception& e) {
>>>>>>> 0dff1c4 (init 2)
        logger.error("Fehler beim VR-Update: {}", e.what());
        return false;
    }
}

void VRManager::resetSeatedPosition() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        vrSystem->ResetSeatedZeroPose();
        logger.info("Sitzposition zurückgesetzt");
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Zurücksetzen der Sitzposition: {}", e.what());
    }
}

std::vector<VRDevice> VRManager::getControllers() const {
    std::lock_guard<std::mutex> lock(mutex);
    return controllers;
}

bool VRManager::isControllerConnected(vr::TrackedDeviceIndex_t controllerIndex) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return false;
    }

    return vrSystem->IsTrackedDeviceConnected(controllerIndex);
}

glm::vec3 VRManager::getControllerPosition(vr::TrackedDeviceIndex_t controllerIndex) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return glm::vec3(0.0f);
    }

    vr::TrackedDevicePose_t pose;
    vrSystem->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseSeated, 0.0f, &pose, 1);
    
    if (pose.bPoseIsValid) {
        glm::mat4 matrix = convertSteamVRMatrixToMatrix4(pose.mDeviceToAbsoluteTracking);
        return glm::vec3(matrix[3]);
    }
    
    return glm::vec3(0.0f);
}

glm::quat VRManager::getControllerRotation(vr::TrackedDeviceIndex_t controllerIndex) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return glm::quat();
    }

    vr::TrackedDevicePose_t pose;
    vrSystem->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseSeated, 0.0f, &pose, 1);
    
    if (pose.bPoseIsValid) {
        glm::mat4 matrix = convertSteamVRMatrixToMatrix4(pose.mDeviceToAbsoluteTracking);
        return glm::quat_cast(matrix);
    }
    
    return glm::quat();
}

VRDevice VRManager::getHMD() const {
    std::lock_guard<std::mutex> lock(mutex);
    return hmd;
}

glm::mat4 VRManager::getHMDPose() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return glm::mat4(1.0f);
    }

    vr::TrackedDevicePose_t pose;
    vrSystem->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseSeated, 0.0f, &pose, 1);
    
    if (pose.bPoseIsValid) {
        return convertSteamVRMatrixToMatrix4(pose.mDeviceToAbsoluteTracking);
    }
    
    return glm::mat4(1.0f);
}

glm::mat4 VRManager::getProjectionMatrix(vr::Hmd_Eye eye) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return glm::mat4(1.0f);
    }

    vr::HmdMatrix44_t matrix = vrSystem->GetProjectionMatrix(eye, 0.1f, 1000.0f);
    return convertSteamVRMatrixToMatrix4(matrix);
}

glm::mat4 VRManager::getEyeToHeadTransform(vr::Hmd_Eye eye) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return glm::mat4(1.0f);
    }

    vr::HmdMatrix34_t matrix = vrSystem->GetEyeToHeadTransform(eye);
    return convertSteamVRMatrixToMatrix4(matrix);
}

void VRManager::processVREvents() {
    vr::VREvent_t event;
    while (pollNextEvent(event)) {
        // Hier können VR-Events verarbeitet werden
        // z.B. Controller-Input, Tracking-Status, etc.
    }
}

bool VRManager::pollNextEvent(vr::VREvent_t& event) {
    if (!initialized) {
        return false;
    }
    return vrSystem->PollNextEvent(&event, sizeof(event));
}

bool VRManager::initializeOpenVR() {
    vr::EVRInitError error = vr::VRInitError_None;
    vrSystem = vr::VR_Init(&error, vr::VRApplication_Scene);
    
    if (error != vr::VRInitError_None) {
        logger.error("OpenVR-Initialisierungsfehler: {}", vr::VR_GetVRInitErrorAsEnglishDescription(error));
        return false;
    }
    
    return true;
}

void VRManager::shutdownOpenVR() {
    if (vrSystem) {
        vr::VR_Shutdown();
        vrSystem = nullptr;
    }
}

VRDevice VRManager::createDeviceInfo(vr::TrackedDeviceIndex_t deviceIndex) {
    VRDevice device;
    device.index = deviceIndex;
    device.isConnected = vrSystem->IsTrackedDeviceConnected(deviceIndex);
    
    if (device.isConnected) {
        char buffer[vr::k_unMaxPropertyStringSize];
        vrSystem->GetStringTrackedDeviceProperty(deviceIndex, vr::Prop_TrackingSystemName_String, buffer, sizeof(buffer));
        device.name = buffer;
        
        vr::TrackedDevicePose_t pose;
        vrSystem->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseSeated, 0.0f, &pose, 1);
        
        device.isTracking = pose.bPoseIsValid;
        if (device.isTracking) {
            glm::mat4 matrix = convertSteamVRMatrixToMatrix4(pose.mDeviceToAbsoluteTracking);
            device.position = glm::vec3(matrix[3]);
            device.rotation = glm::quat_cast(matrix);
        }
    }
    
    return device;
}

glm::mat4 VRManager::convertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t& matPose) {
    glm::mat4 matrix(
        matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0f,
        matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0f,
        matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0f,
        matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
    );
    return matrix;
}

glm::mat4 VRManager::convertSteamVRMatrixToMatrix4(const vr::HmdMatrix44_t& mat) {
    glm::mat4 matrix(
        mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
        mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
        mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
        mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
    );
    return matrix;
}

<<<<<<< HEAD
void VRManager::updateAudioPositions() {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Listener-Position aktualisieren
    AudioEngine::getInstance().setListenerPosition(listenerPosition);
    AudioEngine::getInstance().setListenerOrientation(listenerOrientation);

    // Audio-Quellen-Positionen aktualisieren
    for (const auto& [sourceId, position] : audioSourcePositions) {
        AudioEngine::getInstance().setSourcePosition(sourceId, position);
    }

    for (const auto& [sourceId, orientation] : audioSourceOrientations) {
        AudioEngine::getInstance().setSourceOrientation(sourceId, orientation);
    }
}

void VRManager::updateAudioEngine() {
    // Hier können zusätzliche Audio-Engine-Updates durchgeführt werden
    // z.B. Plugin-Updates, Effekt-Verarbeitung, etc.
}

=======
>>>>>>> 0dff1c4 (init 2)
} // namespace VR_DAW 