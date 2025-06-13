#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <glm/glm.hpp>

namespace VRMusicStudio {

class DesktopSpace {
public:
    struct DesktopConfig {
        // 3D-Desktop Konfiguration
        float deskWidth = 2.0f;        // Breite des virtuellen Desktops in Metern
        float deskDepth = 1.0f;        // Tiefe des virtuellen Desktops
        float deskHeight = 0.8f;       // Höhe des virtuellen Desktops
        float monitorDistance = 1.0f;  // Abstand zum virtuellen Monitor
        float monitorSize = 0.5f;      // Größe des virtuellen Monitors
        
        // UI-Elemente Positionierung
        struct UIElement {
            glm::vec3 position;        // 3D-Position im Raum
            glm::vec3 rotation;        // Rotation im Raum
            glm::vec2 size;           // Größe des Elements
            bool isVisible;           // Sichtbarkeit
        };
        
        // Hauptbereiche
        UIElement mainMonitor;        // Hauptmonitor
        UIElement controlPanel;       // Steuerungspanel
        UIElement mixerPanel;         // Mischpult-Panel
        UIElement instrumentPanel;    // Instrumenten-Panel
        UIElement effectsPanel;       // Effekte-Panel
        
        // Interaktionsmodus
        enum class InteractionMode {
            VR,                        // VR-Modus
            Desktop,                   // Desktop-Modus
            Hybrid                     // Hybrid-Modus
        };
        
        InteractionMode currentMode = InteractionMode::Hybrid;
        
        // Raum-Parameter
        float roomSize = 5.0f;        // Größe des virtuellen Raums
        float ambientLight = 0.8f;    // Umgebungslicht
        float contrast = 1.0f;        // Kontrast
        float brightness = 1.0f;      // Helligkeit
        
        // Performance-Einstellungen
        bool enableShadows = true;    // Schatten aktivieren
        bool enableReflections = true;// Reflexionen aktivieren
        int renderQuality = 2;        // Renderqualität (0-3)
    };

    DesktopSpace();
    ~DesktopSpace();

    // Konfiguration
    void setConfig(const DesktopConfig& config);
    DesktopConfig getConfig() const;

    // UI-Elemente Management
    void setUIElementPosition(const std::string& elementName, const glm::vec3& position);
    void setUIElementRotation(const std::string& elementName, const glm::vec3& rotation);
    void setUIElementSize(const std::string& elementName, const glm::vec2& size);
    void setUIElementVisibility(const std::string& elementName, bool visible);

    // Interaktionsmodus
    void setInteractionMode(DesktopConfig::InteractionMode mode);
    DesktopConfig::InteractionMode getInteractionMode() const;

    // Raum-Parameter
    void setRoomParameters(float size, float ambientLight, float contrast, float brightness);
    void setPerformanceSettings(bool shadows, bool reflections, int renderQuality);

    // Callback-Funktionen
    using UIElementCallback = std::function<void(const std::string&, const glm::vec3&)>;
    void setUIElementCallback(const std::string& elementName, UIElementCallback callback);

    // Rendering
    void render();
    void update();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace VRMusicStudio 