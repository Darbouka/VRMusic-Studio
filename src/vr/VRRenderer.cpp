#include "VRRenderer.hpp"
#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace VRMusicStudio {

struct VRRenderer::Impl {
    vr::IVRSystem* vrSystem;
    vr::IVRCompositor* compositor;
    vr::TrackedDevicePose_t devicePoses[vr::k_unMaxTrackedDeviceCount];
    
    // Rendering
    uint32_t renderWidth;
    uint32_t renderHeight;
    float renderScale;
    int msaaSamples;
    std::string renderMode;
    
    // Kamera
    glm::vec3 cameraPosition;
    glm::quat cameraRotation;
    
    // Effekte
    std::string currentPostProcess;
    float bloomIntensity;
    float vignetteStrength;
    
    // OpenGL
    GLuint framebufferId;
    GLuint renderTextureId;
    GLuint depthBufferId;
    
    Impl() : vrSystem(nullptr), compositor(nullptr),
             renderWidth(0), renderHeight(0),
             renderScale(1.0f), msaaSamples(4),
             renderMode("default"),
             cameraPosition(0.0f),
             cameraRotation(1.0f, 0.0f, 0.0f, 0.0f),
             currentPostProcess("none"),
             bloomIntensity(0.5f),
             vignetteStrength(0.3f),
             framebufferId(0),
             renderTextureId(0),
             depthBufferId(0) {}
};

VRRenderer::VRRenderer() : pImpl(std::make_unique<Impl>()) {}

VRRenderer::~VRRenderer() {
    shutdown();
}

bool VRRenderer::initialize() {
    if (!initializeOpenVR()) {
        spdlog::error("OpenVR-Initialisierung fehlgeschlagen");
        return false;
    }
    
    if (!initializeRenderTargets()) {
        spdlog::error("Render-Target-Initialisierung fehlgeschlagen");
        return false;
    }
    
    spdlog::info("VR-Renderer initialisiert");
    return true;
}

void VRRenderer::shutdown() {
    if (pImpl->compositor) {
        vr::VR_Shutdown();
        pImpl->compositor = nullptr;
        pImpl->vrSystem = nullptr;
    }
    
    // OpenGL-Ressourcen freigeben
    if (pImpl->framebufferId) {
        glDeleteFramebuffers(1, &pImpl->framebufferId);
        glDeleteTextures(1, &pImpl->renderTextureId);
        glDeleteRenderbuffers(1, &pImpl->depthBufferId);
    }
}

void VRRenderer::render() {
    if (!pImpl->vrSystem || !pImpl->compositor) return;
    
    updatePoses();
    
    // Für jedes Auge rendern
    renderEye(vr::Eye_Left);
    renderEye(vr::Eye_Right);
    
    submitFrame();
}

void VRRenderer::setCameraPosition(const glm::vec3& position) {
    pImpl->cameraPosition = position;
}

void VRRenderer::setCameraRotation(const glm::quat& rotation) {
    pImpl->cameraRotation = rotation;
}

glm::mat4 VRRenderer::getViewMatrix() const {
    glm::mat4 view = glm::mat4_cast(pImpl->cameraRotation);
    view = glm::translate(view, -pImpl->cameraPosition);
    return view;
}

glm::mat4 VRRenderer::getProjectionMatrix() const {
    if (!pImpl->vrSystem) return glm::mat4(1.0f);
    
    vr::HmdMatrix44_t proj = pImpl->vrSystem->GetProjectionMatrix(
        vr::Eye_Left,
        0.1f, 100.0f,
        vr::API_OpenGL
    );
    
    return glm::mat4(
        proj.m[0][0], proj.m[1][0], proj.m[2][0], proj.m[3][0],
        proj.m[0][1], proj.m[1][1], proj.m[2][1], proj.m[3][1],
        proj.m[0][2], proj.m[1][2], proj.m[2][2], proj.m[3][2],
        proj.m[0][3], proj.m[1][3], proj.m[2][3], proj.m[3][3]
    );
}

void VRRenderer::setRenderScale(float scale) {
    pImpl->renderScale = glm::clamp(scale, 0.5f, 2.0f);
    initializeRenderTargets();
}

void VRRenderer::setMSAALevel(int samples) {
    pImpl->msaaSamples = samples;
    initializeRenderTargets();
}

void VRRenderer::setRenderMode(const std::string& mode) {
    pImpl->renderMode = mode;
}

void VRRenderer::renderUI(const std::string& uiElement) {
    // UI-Rendering-Logik hier implementieren
}

void VRRenderer::renderText(const std::string& text, const glm::vec2& position) {
    // Text-Rendering-Logik hier implementieren
}

void VRRenderer::renderButton(const std::string& label, const glm::vec2& position, const glm::vec2& size) {
    // Button-Rendering-Logik hier implementieren
}

void VRRenderer::setPostProcessEffect(const std::string& effect) {
    pImpl->currentPostProcess = effect;
}

void VRRenderer::setBloomIntensity(float intensity) {
    pImpl->bloomIntensity = glm::clamp(intensity, 0.0f, 1.0f);
}

void VRRenderer::setVignetteStrength(float strength) {
    pImpl->vignetteStrength = glm::clamp(strength, 0.0f, 1.0f);
}

void VRRenderer::initializeOpenVR() {
    vr::EVRInitError error = vr::VRInitError_None;
    pImpl->vrSystem = vr::VR_Init(&error, vr::VRApplication_Scene);
    
    if (error != vr::VRInitError_None) {
        spdlog::error("OpenVR-Initialisierungsfehler: {}", vr::VR_GetVRInitErrorAsEnglishDescription(error));
        return;
    }
    
    pImpl->compositor = vr::VRCompositor();
    if (!pImpl->compositor) {
        spdlog::error("OpenVR Compositor konnte nicht initialisiert werden");
        return;
    }
    
    // Render-Auflösung abrufen
    pImpl->vrSystem->GetRecommendedRenderTargetSize(&pImpl->renderWidth, &pImpl->renderHeight);
}

bool VRRenderer::initializeRenderTargets() {
    // Alte Ressourcen freigeben
    if (pImpl->framebufferId) {
        glDeleteFramebuffers(1, &pImpl->framebufferId);
        glDeleteTextures(1, &pImpl->renderTextureId);
        glDeleteRenderbuffers(1, &pImpl->depthBufferId);
    }
    
    // Neue Framebuffer erstellen
    glGenFramebuffers(1, &pImpl->framebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, pImpl->framebufferId);
    
    // Render-Textur erstellen
    glGenTextures(1, &pImpl->renderTextureId);
    glBindTexture(GL_TEXTURE_2D, pImpl->renderTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 pImpl->renderWidth, pImpl->renderHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Depth-Buffer erstellen
    glGenRenderbuffers(1, &pImpl->depthBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, pImpl->depthBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                         pImpl->renderWidth, pImpl->renderHeight);
    
    // Framebuffer konfigurieren
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                          GL_TEXTURE_2D, pImpl->renderTextureId, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                             GL_RENDERBUFFER, pImpl->depthBufferId);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        spdlog::error("Framebuffer ist nicht vollständig");
        return false;
    }
    
    return true;
}

void VRRenderer::renderEye(vr::Hmd_Eye eye) {
    // Hier die eigentliche Rendering-Logik für jedes Auge implementieren
}

void VRRenderer::submitFrame() {
    if (!pImpl->compositor) return;
    
    vr::Texture_t leftEyeTexture = {
        (void*)(uintptr_t)pImpl->renderTextureId,
        vr::TextureType_OpenGL,
        vr::ColorSpace_Gamma
    };
    
    vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
    vr::VRCompositor()->Submit(vr::Eye_Right, &leftEyeTexture);
}

void VRRenderer::updatePoses() {
    if (!pImpl->vrSystem) return;
    
    pImpl->vrSystem->GetDeviceToAbsoluteTrackingPose(
        vr::TrackingUniverseStanding,
        0.0f,
        pImpl->devicePoses,
        vr::k_unMaxTrackedDeviceCount
    );
}

} // namespace VRMusicStudio 