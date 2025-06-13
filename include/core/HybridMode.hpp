#pragma once

#include <memory>
#include <string>
#include <functional>
#include <glm/glm.hpp>

namespace VRMusicStudio {

class HybridMode {
public:
    enum class Mode {
        Desktop,     // Nur Desktop-UI
        VR,         // Nur VR-UI
        Hybrid      // Beide Modi gleichzeitig
    };

    struct Settings {
        bool enableMirroring = true;        // Desktop-Mirroring der VR-Ansicht
        bool enableDesktopControls = true;  // Desktop-Steuerung im VR-Modus
        bool enableVRControls = true;       // VR-Steuerung im Desktop-Modus
        float uiScale = 1.0f;              // UI-Skalierung
        glm::vec3 desktopOffset{0.0f};     // Offset für Desktop-UI im VR
    };

    virtual ~HybridMode() = default;

    // Modus-Management
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void setMode(Mode mode) = 0;
    virtual Mode getCurrentMode() const = 0;
    virtual bool isHybridEnabled() const = 0;

    // Einstellungen
    virtual void setSettings(const Settings& settings) = 0;
    virtual Settings getSettings() const = 0;

    // UI-Management
    virtual void updateUI() = 0;
    virtual void renderUI() = 0;
    virtual void syncUIState() = 0;

    // Event-Handling
    virtual void handleDesktopEvent(const void* event) = 0;
    virtual void handleVREvent(const void* event) = 0;

    // Callback-Registrierung
    using ModeChangeCallback = std::function<void(Mode)>;
    virtual void setModeChangeCallback(ModeChangeCallback callback) = 0;

    // Factory-Methode
    static std::unique_ptr<HybridMode> create();
};

} // namespace VRMusicStudio 