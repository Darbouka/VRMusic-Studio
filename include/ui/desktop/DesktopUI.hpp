#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

namespace VRMusicStudio {

class DesktopUI {
public:
    struct Window {
        SDL_Window* window = nullptr;
        SDL_GLContext glContext = nullptr;
        int width = 1280;
        int height = 720;
        std::string title = "VRMusic Studio";
    };

    struct Theme {
        glm::vec4 backgroundColor{0.1f, 0.1f, 0.1f, 1.0f};
        glm::vec4 textColor{1.0f, 1.0f, 1.0f, 1.0f};
        glm::vec4 accentColor{0.2f, 0.6f, 1.0f, 1.0f};
        float fontSize = 14.0f;
    };

    // Initialisierung und Shutdown
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;

    // Fenster-Management
    virtual void createWindow(const Window& window) = 0;
    virtual void destroyWindow() = 0;
    virtual void setWindowTitle(const std::string& title) = 0;
    virtual void setWindowSize(int width, int height) = 0;

    // UI-Rendering
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void render() = 0;

    // Event-Handling
    virtual void processEvents() = 0;
    virtual bool shouldClose() const = 0;

    // Theme-Management
    virtual void setTheme(const Theme& theme) = 0;
    virtual Theme getTheme() const = 0;

    // UI-Komponenten
    virtual void renderMainMenu() = 0;
    virtual void renderToolbar() = 0;
    virtual void renderTimeline() = 0;
    virtual void renderMixer() = 0;
    virtual void renderEffects() = 0;
    virtual void renderPianoRoll() = 0;
    virtual void renderTransport() = 0;

    // Callback-Registrierung
    using EventCallback = std::function<void(const SDL_Event&)>;
    virtual void setEventCallback(EventCallback callback) = 0;

    // Factory-Methode
    static std::unique_ptr<DesktopUI> create();
};

} // namespace VRMusicStudio 