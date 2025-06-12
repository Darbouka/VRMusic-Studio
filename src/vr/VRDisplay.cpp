<<<<<<< HEAD
#include "VRDisplay.hpp"; namespace VR_DAW { VRDisplay::VRDisplay() {} VRDisplay::~VRDisplay() {} }
=======
#include "VRDisplay.hpp"
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace VRMusicStudio {

struct VRDisplay::Impl {
    // OpenVR
    vr::IVRSystem* vrSystem;
    vr::IVRCompositor* vrCompositor;
    
    // Display-Parameter
    int width;
    int height;
    float refreshRate;
    float fov;
    float aspectRatio;
    
    // Rendering
    GLuint framebuffer;
    GLuint colorTexture;
    GLuint depthTexture;
    
    // Zustand
    bool isInitialized;
    
    Impl() : vrSystem(nullptr), vrCompositor(nullptr),
             width(0), height(0), refreshRate(90.0f),
             fov(90.0f), aspectRatio(1.0f),
             framebuffer(0), colorTexture(0), depthTexture(0),
             isInitialized(false) {}
};

VRDisplay::VRDisplay() : pImpl(std::make_unique<Impl>()) {}

VRDisplay::~VRDisplay() {
    shutdown();
}

bool VRDisplay::initialize() {
    if (!initializeOpenVR()) {
        spdlog::error("Failed to initialize OpenVR");
        return false;
    }
    
    if (!initializeRendering()) {
        spdlog::error("Failed to initialize rendering");
        return false;
    }
    
    pImpl->isInitialized = true;
    return true;
}

void VRDisplay::shutdown() {
    if (pImpl->isInitialized) {
        cleanup();
    }
}

bool VRDisplay::initializeOpenVR() {
    vr::EVRInitError error = vr::VRInitError_None;
    pImpl->vrSystem = vr::VR_Init(&error, vr::VRApplication_Scene);
    
    if (error != vr::VRInitError_None) {
        spdlog::error("OpenVR initialization failed: {}", vr::VR_GetVRInitErrorAsEnglishDescription(error));
        return false;
    }
    
    pImpl->vrCompositor = vr::VRCompositor();
    if (!pImpl->vrCompositor) {
        spdlog::error("Failed to get VR compositor");
        return false;
    }
    
    // Hole Display-Parameter
    pImpl->vrSystem->GetRecommendedRenderTargetSize(&pImpl->width, &pImpl->height);
    pImpl->refreshRate = pImpl->vrSystem->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_DisplayFrequency_Float);
    
    float left, right, top, bottom;
    pImpl->vrSystem->GetProjectionRaw(vr::Eye_Left, &left, &right, &top, &bottom);
    pImpl->fov = std::abs(right - left);
    pImpl->aspectRatio = std::abs((bottom - top) / (right - left));
    
    return true;
}

bool VRDisplay::initializeRendering() {
    // Erstelle Framebuffer
    glGenFramebuffers(1, &pImpl->framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, pImpl->framebuffer);
    
    // Erstelle Color Texture
    glGenTextures(1, &pImpl->colorTexture);
    glBindTexture(GL_TEXTURE_2D, pImpl->colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, pImpl->width, pImpl->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pImpl->colorTexture, 0);
    
    // Erstelle Depth Texture
    glGenTextures(1, &pImpl->depthTexture);
    glBindTexture(GL_TEXTURE_2D, pImpl->depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, pImpl->width, pImpl->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pImpl->depthTexture, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        spdlog::error("Framebuffer is not complete");
        return false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void VRDisplay::cleanup() {
    if (pImpl->framebuffer) {
        glDeleteFramebuffers(1, &pImpl->framebuffer);
        pImpl->framebuffer = 0;
    }
    
    if (pImpl->colorTexture) {
        glDeleteTextures(1, &pImpl->colorTexture);
        pImpl->colorTexture = 0;
    }
    
    if (pImpl->depthTexture) {
        glDeleteTextures(1, &pImpl->depthTexture);
        pImpl->depthTexture = 0;
    }
    
    if (pImpl->vrSystem) {
        vr::VR_Shutdown();
        pImpl->vrSystem = nullptr;
        pImpl->vrCompositor = nullptr;
    }
    
    pImpl->isInitialized = false;
}

void VRDisplay::beginFrame() {
    if (!pImpl->isInitialized) {
        return;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, pImpl->framebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void VRDisplay::endFrame() {
    if (!pImpl->isInitialized) {
        return;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    vr::Texture_t leftEyeTexture = {
        (void*)(uintptr_t)pImpl->colorTexture,
        vr::TextureType_OpenGL,
        vr::ColorSpace_Gamma
    };
    
    vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
    vr::VRCompositor()->Submit(vr::Eye_Right, &leftEyeTexture);
}

glm::mat4 VRDisplay::getProjectionMatrix(vr::Hmd_Eye eye) {
    if (!pImpl->isInitialized) {
        return glm::mat4(1.0f);
    }
    
    float left, right, top, bottom;
    pImpl->vrSystem->GetProjectionRaw(eye, &left, &right, &top, &bottom);
    
    return glm::frustum(left, right, bottom, top, 0.1f, 100.0f);
}

glm::mat4 VRDisplay::getViewMatrix(vr::Hmd_Eye eye) {
    if (!pImpl->isInitialized) {
        return glm::mat4(1.0f);
    }
    
    vr::HmdMatrix34_t eyeToHead = pImpl->vrSystem->GetEyeToHeadTransform(eye);
    glm::mat4 eyeMatrix = glm::mat4(
        eyeToHead.m[0][0], eyeToHead.m[0][1], eyeToHead.m[0][2], eyeToHead.m[0][3],
        eyeToHead.m[1][0], eyeToHead.m[1][1], eyeToHead.m[1][2], eyeToHead.m[1][3],
        eyeToHead.m[2][0], eyeToHead.m[2][1], eyeToHead.m[2][2], eyeToHead.m[2][3],
        0.0f, 0.0f, 0.0f, 1.0f
    );
    
    return glm::inverse(eyeMatrix);
}

int VRDisplay::getWidth() const {
    return pImpl->width;
}

int VRDisplay::getHeight() const {
    return pImpl->height;
}

float VRDisplay::getRefreshRate() const {
    return pImpl->refreshRate;
}

float VRDisplay::getFOV() const {
    return pImpl->fov;
}

float VRDisplay::getAspectRatio() const {
    return pImpl->aspectRatio;
}

} // namespace VRMusicStudio
>>>>>>> 0dff1c4 (init 2)
