#include "vr/VRIntegration.hpp"
#include <openvr.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <spdlog/spdlog.h>
#include <filesystem>

namespace VRMusicStudio {
namespace VR {

class VRIntegrationImpl : public VRIntegration {
public:
    VRIntegrationImpl() = default;
    ~VRIntegrationImpl() override { shutdown(); }

    // ComponentBase implementation
    std::string getName() const override { return "VR Integration"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getCategory() const override { return "VR"; }

    bool initialize() override {
        if (m_initialized) {
            return true;
        }

        initializeVR();
        if (!m_initialized) return false;

        scanDevices();
        tracking.initialize(vrSystem);
        audio.initialize(vrSystem);
        haptics.initialize(vrSystem);
        renderer.initialize(vrSystem, compositor);

        m_initialized = true;
        spdlog::info("VR-System initialisiert");
        return true;
    }

    void shutdown() override {
        if (m_initialized) {
            renderer.shutdown();
            haptics.shutdown();
            audio.shutdown();
            tracking.shutdown();

            if (vrSystem) {
                vr::VR_Shutdown();
                vrSystem = nullptr;
                compositor = nullptr;
                vrInput = nullptr;
            }

            m_initialized = false;
            spdlog::info("VR-System heruntergefahren");
        }
    }

    void update() override {
        if (!m_initialized) return;

        // Aktualisiere Tracking
        tracking.update();

        // Aktualisiere Controller-Status
        for (auto& controller : controllers) {
            updateControllerState(controller);
        }

        // Aktualisiere Audio
        audio.update();

        // Aktualisiere Haptics
        haptics.update();

        // Aktualisiere Renderer
        renderer.update();
    }

    bool isEnabled() const override { return enabled; }
    void setEnabled(bool value) override { enabled = value; }
    bool isInitialized() const override { return m_initialized; }
    std::string getLastError() const override { return lastError; }
    void clearError() override { lastError.clear(); }

    // VR device management
    bool initializeVR() {
        vr::EVRInitError error = vr::VRInitError_None;
        vrSystem = vr::VR_Init(&error, vr::VRApplication_Scene);

        if (error != vr::VRInitError_None) {
            spdlog::error("OpenVR Initialisierung fehlgeschlagen: {}", vr::VR_GetVRInitErrorAsEnglishDescription(error));
            return false;
        }

        compositor = vr::VRCompositor();
        if (!compositor) {
            spdlog::error("VR Compositor konnte nicht initialisiert werden");
            return false;
        }

        vrInput = vr::VRInput();
        if (!vrInput) {
            spdlog::error("VR Input konnte nicht initialisiert werden");
            return false;
        }

        m_initialized = true;
        spdlog::info("VR-System initialisiert");
        return true;
    }

    void scanDevices() {
        devices.clear();
        controllers.clear();

        // Scanne alle VR-Geräte
        for (vr::TrackedDeviceIndex_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
            if (!vrSystem->IsTrackedDeviceConnected(i)) continue;

            VRDevice device;
            device.id = i;
            device.type = static_cast<VRDeviceType>(vrSystem->GetTrackedDeviceClass(i));
            device.name = getDeviceName(i);
            devices.push_back(device);

            // Spezielle Behandlung für Controller
            if (device.type == VRDeviceType::Controller) {
                VRController controller;
                controller.device = device;
                controller.hand = (i == vrSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand)) 
                    ? VRHand::Left : VRHand::Right;
                controllers.push_back(controller);
            }
        }

        // Initialisiere HMD
        hmd.device = devices[0]; // HMD ist immer das erste Gerät
        hmd.resolution = getHMDRenderTargetSize();
        hmd.fov = getHMDProjectionRaw();
    }

    std::string getDeviceName(vr::TrackedDeviceIndex_t deviceId) {
        char buffer[vr::k_unMaxPropertyStringSize];
        vrSystem->GetStringTrackedDeviceProperty(deviceId, vr::Prop_TrackingSystemName_String, buffer, vr::k_unMaxPropertyStringSize);
        return std::string(buffer);
    }

    glm::uvec2 getHMDRenderTargetSize() {
        uint32_t width, height;
        vrSystem->GetRecommendedRenderTargetSize(&width, &height);
        return glm::uvec2(width, height);
    }

    glm::vec4 getHMDProjectionRaw() {
        float left, right, top, bottom;
        vrSystem->GetProjectionRaw(vr::Eye_Left, &left, &right, &top, &bottom);
        return glm::vec4(left, right, top, bottom);
    }

    bool isVRInitialized() const override { return m_initialized; }

    std::vector<std::string> getAvailableDevices() const override {
        std::vector<std::string> devices;
        if (!vrSystem) return devices;

        for (vr::TrackedDeviceIndex_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
            if (vrSystem->IsTrackedDeviceConnected(i)) {
                char deviceName[vr::k_unMaxPropertyStringSize];
                vr::ETrackedPropertyError error;
                vrSystem->GetStringTrackedDeviceProperty(i, vr::Prop_TrackingSystemName_String, deviceName, vr::k_unMaxPropertyStringSize, &error);
                if (error == vr::TrackedProp_Success) {
                    devices.push_back(deviceName);
                }
            }
        }
        return devices;
    }

    bool connectDevice(const std::string& deviceId) override {
        if (!vrSystem) return false;
        // Implementation
        return true;
    }

    void disconnectDevice(const std::string& deviceId) override {
        if (!vrSystem) return;
        // Implementation
    }

    // Controller management
    bool initializeControllers() override {
        if (!vrSystem) return false;
        // Implementation
        controllersInitialized = true;
        return true;
    }

    void shutdownControllers() override {
        controllersInitialized = false;
    }

    bool areControllersInitialized() const override { return controllersInitialized; }

    std::vector<std::string> getConnectedControllers() const override {
        std::vector<std::string> controllers;
        if (!vrSystem) return controllers;
        // Implementation
        return controllers;
    }

    bool isControllerConnected(const std::string& controllerId) const override {
        if (!vrSystem) return false;
        // Implementation
        return false;
    }

    glm::vec3 getControllerPosition(const std::string& controllerId) const override {
        if (!vrSystem) return glm::vec3(0.0f);
        // Implementation
        return glm::vec3(0.0f);
    }

    glm::quat getControllerRotation(const std::string& controllerId) const override {
        if (!vrSystem) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        // Implementation
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    bool getControllerButtonState(const std::string& controllerId, int button) const override {
        if (!vrSystem) return false;
        // Implementation
        return false;
    }

    float getControllerTriggerValue(const std::string& controllerId) const override {
        if (!vrSystem) return 0.0f;
        // Implementation
        return 0.0f;
    }

    glm::vec2 getControllerThumbstickValue(const std::string& controllerId) const override {
        if (!vrSystem) return glm::vec2(0.0f);
        // Implementation
        return glm::vec2(0.0f);
    }

    // HMD management
    bool initializeHMD() override {
        if (!vrSystem) return false;
        // Implementation
        hmdInitialized = true;
        return true;
    }

    void shutdownHMD() override {
        hmdInitialized = false;
    }

    bool isHMDInitialized() const override { return hmdInitialized; }

    std::string getConnectedHMD() const override {
        if (!vrSystem) return "";
        // Implementation
        return "";
    }

    bool isHMDConnected() const override {
        if (!vrSystem) return false;
        // Implementation
        return false;
    }

    glm::vec3 getHMDPosition() const override {
        if (!vrSystem) return glm::vec3(0.0f);
        // Implementation
        return glm::vec3(0.0f);
    }

    glm::quat getHMDRotation() const override {
        if (!vrSystem) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        // Implementation
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    glm::mat4 getHMDProjectionMatrix() const override {
        if (!vrSystem) return glm::mat4(1.0f);
        // Implementation
        return glm::mat4(1.0f);
    }

    glm::mat4 getHMDViewMatrix() const override {
        if (!vrSystem) return glm::mat4(1.0f);
        // Implementation
        return glm::mat4(1.0f);
    }

    // Tracking management
    bool initializeTracking() override {
        if (!vrSystem) return false;
        // Implementation
        trackingInitialized = true;
        return true;
    }

    void shutdownTracking() override {
        trackingInitialized = false;
    }

    bool isTrackingInitialized() const override { return trackingInitialized; }

    bool startTracking() override {
        if (!vrSystem) return false;
        // Implementation
        return true;
    }

    void stopTracking() override {
        // Implementation
    }

    bool isTracking() const override {
        if (!vrSystem) return false;
        // Implementation
        return false;
    }

    glm::vec3 getTrackingPosition() const override {
        if (!vrSystem) return glm::vec3(0.0f);
        // Implementation
        return glm::vec3(0.0f);
    }

    glm::quat getTrackingRotation() const override {
        if (!vrSystem) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        // Implementation
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    // Audio spatialization
    bool initializeSpatialAudio() override {
        // Implementation
        spatialAudioInitialized = true;
        return true;
    }

    void shutdownSpatialAudio() override {
        spatialAudioInitialized = false;
    }

    bool isSpatialAudioInitialized() const override { return spatialAudioInitialized; }

    bool setAudioSourcePosition(const std::string& sourceId, const glm::vec3& position) override {
        if (!spatialAudioInitialized) return false;
        // Implementation
        return true;
    }

    bool setAudioSourceRotation(const std::string& sourceId, const glm::quat& rotation) override {
        if (!spatialAudioInitialized) return false;
        // Implementation
        return true;
    }

    bool setAudioSourceVolume(const std::string& sourceId, float volume) override {
        if (!spatialAudioInitialized) return false;
        // Implementation
        return true;
    }

    bool setAudioSourceDistance(const std::string& sourceId, float distance) override {
        if (!spatialAudioInitialized) return false;
        // Implementation
        return true;
    }

    // Haptic feedback
    bool initializeHaptics() override {
        // Implementation
        hapticsInitialized = true;
        return true;
    }

    void shutdownHaptics() override {
        hapticsInitialized = false;
    }

    bool isHapticsInitialized() const override { return hapticsInitialized; }

    bool triggerHapticPulse(const std::string& controllerId, float amplitude, float duration) override {
        if (!hapticsInitialized) return false;
        // Implementation
        return true;
    }

    bool setHapticFrequency(const std::string& controllerId, float frequency) override {
        if (!hapticsInitialized) return false;
        // Implementation
        return true;
    }

    bool setHapticAmplitude(const std::string& controllerId, float amplitude) override {
        if (!hapticsInitialized) return false;
        // Implementation
        return true;
    }

    // Rendering
    bool initializeRendering() override {
        // Implementation
        renderingInitialized = true;
        return true;
    }

    void shutdownRendering() override {
        renderingInitialized = false;
    }

    bool isRenderingInitialized() const override { return renderingInitialized; }

    void beginFrame() override {
        if (!renderingInitialized) return;
        // Implementation
    }

    void endFrame() override {
        if (!renderingInitialized) return;
        // Implementation
    }

    bool submitFrame() override {
        if (!renderingInitialized) return false;
        // Implementation
        return true;
    }

    bool isFrameSubmitted() const override {
        if (!renderingInitialized) return false;
        // Implementation
        return false;
    }

    std::string getSteamVRPath() const override {
        return m_steamVRPath;
    }

    void updateControllerState(VRController& controller) {
        vr::VRControllerState_t state;
        if (vrSystem->GetControllerState(controller.device.id, &state, sizeof(state))) {
            controller.buttons = state.ulButtonPressed;
            controller.touches = state.ulButtonTouched;
            controller.axes = {
                glm::vec2(state.rAxis[0].x, state.rAxis[0].y),
                glm::vec2(state.rAxis[1].x, state.rAxis[1].y),
                glm::vec2(state.rAxis[2].x, state.rAxis[2].y),
                glm::vec2(state.rAxis[3].x, state.rAxis[3].y),
                glm::vec2(state.rAxis[4].x, state.rAxis[4].y)
            };
        }
    }

    const std::vector<VRDevice>& getDevices() const override {
        return devices;
    }

    const std::vector<VRController>& getControllers() const override {
        return controllers;
    }

    const VRHMD& getHMD() const override {
        return hmd;
    }

    const VRTracking& getTracking() const override {
        return tracking;
    }

    VRAudio& getAudio() override {
        return audio;
    }

    VRHaptics& getHaptics() override {
        return haptics;
    }

    VRRenderer& getRenderer() override {
        return renderer;
    }

private:
    vr::IVRSystem* vrSystem = nullptr;
    vr::IVRCompositor* compositor = nullptr;
    vr::IVRInput* vrInput = nullptr;
    bool enabled = false;
    bool m_initialized = false;
    bool vrInitialized = false;
    bool hmdInitialized = false;
    bool controllersInitialized = false;
    bool trackingInitialized = false;
    bool spatialAudioInitialized = false;
    bool hapticsInitialized = false;
    bool renderingInitialized = false;
    std::string lastError;
    std::string m_steamVRPath;
    std::vector<VRDevice> devices;
    std::vector<VRController> controllers;
    VRHMD hmd;
    VRTracking tracking;
    VRAudio audio;
    VRHaptics haptics;
    VRRenderer renderer;

    bool findSteamVRPath() {
#ifdef __APPLE__
        // macOS SteamVR-Pfade
        const std::vector<std::string> possiblePaths = {
            "/Applications/SteamVR.app/Contents/MacOS",
            "/Users/" + std::string(getenv("USER")) + "/Library/Application Support/Steam/steamapps/common/SteamVR",
            "/Users/" + std::string(getenv("USER")) + "/Library/Application Support/Steam/steamapps/common/SteamVR/bin/osx64"
        };
#else
        // Windows/Linux SteamVR-Pfade
        const std::vector<std::string> possiblePaths = {
            "C:/Program Files (x86)/Steam/steamapps/common/SteamVR",
            "/usr/local/SteamVR",
            "/opt/SteamVR"
        };
#endif

        for (const auto& path : possiblePaths) {
            if (std::filesystem::exists(path)) {
                m_steamVRPath = path;
                spdlog::info("SteamVR gefunden in: {}", m_steamVRPath);
                return true;
            }
        }

        spdlog::error("SteamVR konnte nicht gefunden werden");
        return false;
    }

    void updateVR() {
        if (!vrSystem) return;
        // Implementation
    }

    void updateControllers() {
        if (!vrSystem) return;
        // Implementation
    }

    void updateHMD() {
        if (!vrSystem) return;
        // Implementation
    }

    void updateTracking() {
        if (!vrSystem) return;
        // Implementation
    }

    void updateSpatialAudio() {
        if (!spatialAudioInitialized) return;
        // Implementation
    }

    void updateHaptics() {
        if (!hapticsInitialized) return;
        // Implementation
    }

    void updateRendering() {
        if (!renderingInitialized) return;
        // Implementation
    }
};

std::unique_ptr<VRIntegration> VRIntegration::create() {
    return std::make_unique<VRIntegrationImpl>();
}

} // namespace VR
} // namespace VRMusicStudio 