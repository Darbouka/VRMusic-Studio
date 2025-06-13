#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#include <string>
#include <memory>

namespace VRMusicStudio {

class VRManager {
public:
    VRManager();
    ~VRManager();
    
    void initialize();
    void shutdown();
    void update();
    
    glm::mat4 getDevicePose(uint32_t deviceIndex) const;
    bool isDeviceConnected(uint32_t deviceIndex) const;
    std::string getDeviceName(uint32_t deviceIndex) const;

private:
    void processVREvent(const struct VREvent_t& event);
    void updateTracking();
    void updateDevicePose(uint32_t deviceIndex, const struct TrackedDevicePose_t& pose);
    glm::mat4 convertSteamVRMatrixToGLMMatrix(const struct HmdMatrix34_t& matrix);
    
    void handleDeviceActivated(uint32_t deviceIndex);
    void handleDeviceDeactivated(uint32_t deviceIndex);
    void handleButtonPress(uint32_t deviceIndex, uint32_t button);
    void handleButtonUnpress(uint32_t deviceIndex, uint32_t button);
    
    class IVRSystem;
    class IVRCompositor;
    
    IVRSystem* m_hmd;
    IVRCompositor* m_compositor;
    bool m_isInitialized;
    
    struct TrackedDevicePose_t;
    TrackedDevicePose_t m_trackedDevicePoses[64];
    std::unordered_map<uint32_t, glm::mat4> m_devicePoses;
};

} // namespace VRMusicStudio 