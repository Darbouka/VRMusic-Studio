#include "DesktopSpace.hpp"
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace VRMusicStudio {

struct DesktopSpace::Impl {
    DesktopConfig config;
    std::unordered_map<std::string, UIElementCallback> callbacks;
    
    // UI-Elemente Transformationen
    std::unordered_map<std::string, glm::mat4> elementTransforms;
    
    // Rendering-Zustand
    bool isInitialized = false;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    
    Impl() {
        // Initialisiere Standard-Transformationen
        initializeDefaultTransforms();
    }
    
    void initializeDefaultTransforms() {
        // Hauptmonitor
        elementTransforms["mainMonitor"] = glm::translate(glm::mat4(1.0f), 
            glm::vec3(0.0f, config.deskHeight, -config.monitorDistance));
            
        // Steuerungspanel
        elementTransforms["controlPanel"] = glm::translate(glm::mat4(1.0f),
            glm::vec3(-config.deskWidth/2 + 0.2f, config.deskHeight, 0.0f));
            
        // Mischpult-Panel
        elementTransforms["mixerPanel"] = glm::translate(glm::mat4(1.0f),
            glm::vec3(config.deskWidth/2 - 0.2f, config.deskHeight, 0.0f));
            
        // Instrumenten-Panel
        elementTransforms["instrumentPanel"] = glm::translate(glm::mat4(1.0f),
            glm::vec3(0.0f, config.deskHeight + 0.3f, 0.0f));
            
        // Effekte-Panel
        elementTransforms["effectsPanel"] = glm::translate(glm::mat4(1.0f),
            glm::vec3(0.0f, config.deskHeight - 0.3f, 0.0f));
    }
    
    void updateTransforms() {
        // Aktualisiere Transformationen basierend auf Konfiguration
        for (auto& [name, transform] : elementTransforms) {
            if (name == "mainMonitor") {
                transform = glm::translate(glm::mat4(1.0f),
                    glm::vec3(0.0f, config.deskHeight, -config.monitorDistance));
            }
            // Weitere Transformationen...
        }
    }
};

DesktopSpace::DesktopSpace() : pImpl(std::make_unique<Impl>()) {}
DesktopSpace::~DesktopSpace() = default;

void DesktopSpace::setConfig(const DesktopConfig& config) {
    pImpl->config = config;
    pImpl->updateTransforms();
}

DesktopSpace::DesktopConfig DesktopSpace::getConfig() const {
    return pImpl->config;
}

void DesktopSpace::setUIElementPosition(const std::string& elementName, const glm::vec3& position) {
    auto it = pImpl->elementTransforms.find(elementName);
    if (it != pImpl->elementTransforms.end()) {
        it->second = glm::translate(glm::mat4(1.0f), position);
        
        // Callback aufrufen
        auto callbackIt = pImpl->callbacks.find(elementName);
        if (callbackIt != pImpl->callbacks.end()) {
            callbackIt->second(elementName, position);
        }
    }
}

void DesktopSpace::setUIElementRotation(const std::string& elementName, const glm::vec3& rotation) {
    auto it = pImpl->elementTransforms.find(elementName);
    if (it != pImpl->elementTransforms.end()) {
        glm::quat quatRotation = glm::quat(rotation);
        it->second = glm::mat4_cast(quatRotation) * it->second;
    }
}

void DesktopSpace::setUIElementSize(const std::string& elementName, const glm::vec2& size) {
    auto it = pImpl->elementTransforms.find(elementName);
    if (it != pImpl->elementTransforms.end()) {
        it->second = glm::scale(it->second, glm::vec3(size.x, size.y, 1.0f));
    }
}

void DesktopSpace::setUIElementVisibility(const std::string& elementName, bool visible) {
    if (elementName == "mainMonitor") pImpl->config.mainMonitor.isVisible = visible;
    else if (elementName == "controlPanel") pImpl->config.controlPanel.isVisible = visible;
    else if (elementName == "mixerPanel") pImpl->config.mixerPanel.isVisible = visible;
    else if (elementName == "instrumentPanel") pImpl->config.instrumentPanel.isVisible = visible;
    else if (elementName == "effectsPanel") pImpl->config.effectsPanel.isVisible = visible;
}

void DesktopSpace::setInteractionMode(DesktopConfig::InteractionMode mode) {
    pImpl->config.currentMode = mode;
    
    // Passe UI basierend auf Modus an
    switch (mode) {
        case DesktopConfig::InteractionMode::VR:
            // VR-spezifische Anpassungen
            pImpl->config.monitorDistance = 2.0f;
            pImpl->config.monitorSize = 1.0f;
            break;
            
        case DesktopConfig::InteractionMode::Desktop:
            // Desktop-spezifische Anpassungen
            pImpl->config.monitorDistance = 0.8f;
            pImpl->config.monitorSize = 0.4f;
            break;
            
        case DesktopConfig::InteractionMode::Hybrid:
            // Hybrid-spezifische Anpassungen
            pImpl->config.monitorDistance = 1.5f;
            pImpl->config.monitorSize = 0.7f;
            break;
    }
    
    pImpl->updateTransforms();
}

DesktopSpace::DesktopConfig::InteractionMode DesktopSpace::getInteractionMode() const {
    return pImpl->config.currentMode;
}

void DesktopSpace::setRoomParameters(float size, float ambientLight, float contrast, float brightness) {
    pImpl->config.roomSize = size;
    pImpl->config.ambientLight = ambientLight;
    pImpl->config.contrast = contrast;
    pImpl->config.brightness = brightness;
}

void DesktopSpace::setPerformanceSettings(bool shadows, bool reflections, int renderQuality) {
    pImpl->config.enableShadows = shadows;
    pImpl->config.enableReflections = reflections;
    pImpl->config.renderQuality = renderQuality;
}

void DesktopSpace::setUIElementCallback(const std::string& elementName, UIElementCallback callback) {
    pImpl->callbacks[elementName] = callback;
}

void DesktopSpace::render() {
    if (!pImpl->isInitialized) {
        // Initialisiere Rendering-System
        pImpl->isInitialized = true;
    }
    
    // Rendere UI-Elemente basierend auf Sichtbarkeit
    if (pImpl->config.mainMonitor.isVisible) {
        // Rendere Hauptmonitor
    }
    
    if (pImpl->config.controlPanel.isVisible) {
        // Rendere Steuerungspanel
    }
    
    if (pImpl->config.mixerPanel.isVisible) {
        // Rendere Mischpult-Panel
    }
    
    if (pImpl->config.instrumentPanel.isVisible) {
        // Rendere Instrumenten-Panel
    }
    
    if (pImpl->config.effectsPanel.isVisible) {
        // Rendere Effekte-Panel
    }
}

void DesktopSpace::update() {
    // Aktualisiere UI-Elemente
    pImpl->updateTransforms();
    
    // Aktualisiere Kameraposition basierend auf Interaktionsmodus
    switch (pImpl->config.currentMode) {
        case DesktopConfig::InteractionMode::VR:
            // VR-Kamera-Update
            break;
            
        case DesktopConfig::InteractionMode::Desktop:
            // Desktop-Kamera-Update
            break;
            
        case DesktopConfig::InteractionMode::Hybrid:
            // Hybrid-Kamera-Update
            break;
    }
}

} // namespace VRMusicStudio 