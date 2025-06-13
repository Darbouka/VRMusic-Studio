#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <openvr.h>

namespace VRMusicStudio {

class VRRenderer {
public:
    VRRenderer();
    ~VRRenderer();

    bool initialize();
    void shutdown();
    void render();

    // Kamera-Steuerung
    void setCameraPosition(const glm::vec3& position);
    void setCameraRotation(const glm::quat& rotation);
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    // Rendering-Einstellungen
    void setRenderScale(float scale);
    void setMSAALevel(int samples);
    void setRenderMode(const std::string& mode);

    // UI-Rendering
    void renderUI(const std::string& uiElement);
    void renderText(const std::string& text, const glm::vec2& position);
    void renderButton(const std::string& label, const glm::vec2& position, const glm::vec2& size);

    // Effekte
    void setPostProcessEffect(const std::string& effect);
    void setBloomIntensity(float intensity);
    void setVignetteStrength(float strength);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void initializeOpenVR();
    void initializeRenderTargets();
    void renderEye(vr::Hmd_Eye eye);
    void submitFrame();
    void updatePoses();
};

} // namespace VRMusicStudio 