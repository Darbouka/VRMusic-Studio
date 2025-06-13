#include "ui/desktop/DesktopUI.hpp"
#include <spdlog/spdlog.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

namespace VRMusicStudio {

class DesktopUIImpl : public DesktopUI {
private:
    Window window;
    Theme theme;
    bool initialized = false;
    bool shouldCloseWindow = false;
    EventCallback eventCallback;

public:
    DesktopUIImpl() = default;
    ~DesktopUIImpl() override { shutdown(); }

    bool initialize() override {
        if (initialized) return true;

        // SDL initialisieren
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
            spdlog::error("SDL konnte nicht initialisiert werden: {}", SDL_GetError());
            return false;
        }

        // OpenGL-Version setzen
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        // ImGui initialisieren
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // ImGui Style setzen
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 4.0f;
        style.FrameRounding = 4.0f;
        style.GrabRounding = 4.0f;
        style.ScrollbarRounding = 4.0f;

        initialized = true;
        spdlog::info("Desktop-UI initialisiert");
        return true;
    }

    void shutdown() override {
        if (!initialized) return;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        if (window.window) {
            SDL_GL_DeleteContext(window.glContext);
            SDL_DestroyWindow(window.window);
        }

        SDL_Quit();
        initialized = false;
        spdlog::info("Desktop-UI heruntergefahren");
    }

    void createWindow(const Window& newWindow) override {
        window = newWindow;

        window.window = SDL_CreateWindow(
            window.title.c_str(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            window.width, window.height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
        );

        if (!window.window) {
            spdlog::error("Fenster konnte nicht erstellt werden: {}", SDL_GetError());
            return;
        }

        window.glContext = SDL_GL_CreateContext(window.window);
        SDL_GL_MakeCurrent(window.window, window.glContext);

        // ImGui Backend initialisieren
        ImGui_ImplSDL2_InitForOpenGL(window.window, window.glContext);
        ImGui_ImplOpenGL3_Init("#version 150");
    }

    void destroyWindow() override {
        if (window.window) {
            SDL_GL_DeleteContext(window.glContext);
            SDL_DestroyWindow(window.window);
            window.window = nullptr;
            window.glContext = nullptr;
        }
    }

    void setWindowTitle(const std::string& title) override {
        if (window.window) {
            SDL_SetWindowTitle(window.window, title.c_str());
        }
    }

    void setWindowSize(int width, int height) override {
        if (window.window) {
            SDL_SetWindowSize(window.window, width, height);
        }
    }

    void beginFrame() override {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void endFrame() override {
        ImGui::Render();
        SDL_GL_MakeCurrent(window.window, window.glContext);
        glViewport(0, 0, window.width, window.height);
        glClearColor(
            theme.backgroundColor.r,
            theme.backgroundColor.g,
            theme.backgroundColor.b,
            theme.backgroundColor.a
        );
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window.window);
    }

    void render() override {
        // Docking-Bereich
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // Hauptmenü
        renderMainMenu();

        // Toolbar
        renderToolbar();

        // Timeline
        renderTimeline();

        // Mixer
        renderMixer();

        // Effekte
        renderEffects();

        // Piano Roll
        renderPianoRoll();

        // Transport
        renderTransport();
    }

    void processEvents() override {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT) {
                shouldCloseWindow = true;
            }

            if (eventCallback) {
                eventCallback(event);
            }
        }
    }

    bool shouldClose() const override {
        return shouldCloseWindow;
    }

    void setTheme(const Theme& newTheme) override {
        theme = newTheme;
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_WindowBg] = ImVec4(
            theme.backgroundColor.r,
            theme.backgroundColor.g,
            theme.backgroundColor.b,
            theme.backgroundColor.a
        );
        style.Colors[ImGuiCol_Text] = ImVec4(
            theme.textColor.r,
            theme.textColor.g,
            theme.textColor.b,
            theme.textColor.a
        );
        style.Colors[ImGuiCol_Button] = ImVec4(
            theme.accentColor.r,
            theme.accentColor.g,
            theme.accentColor.b,
            theme.accentColor.a
        );
    }

    Theme getTheme() const override {
        return theme;
    }

    void renderMainMenu() override {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Datei")) {
                if (ImGui::MenuItem("Neu")) {}
                if (ImGui::MenuItem("Öffnen")) {}
                if (ImGui::MenuItem("Speichern")) {}
                if (ImGui::MenuItem("Beenden")) { shouldCloseWindow = true; }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Bearbeiten")) {
                if (ImGui::MenuItem("Rückgängig")) {}
                if (ImGui::MenuItem("Wiederholen")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Ansicht")) {
                if (ImGui::MenuItem("Timeline")) {}
                if (ImGui::MenuItem("Mixer")) {}
                if (ImGui::MenuItem("Effekte")) {}
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void renderToolbar() override {
        if (ImGui::Begin("Toolbar")) {
            if (ImGui::Button("Play")) {}
            ImGui::SameLine();
            if (ImGui::Button("Stop")) {}
            ImGui::SameLine();
            if (ImGui::Button("Record")) {}
            ImGui::End();
        }
    }

    void renderTimeline() override {
        if (ImGui::Begin("Timeline")) {
            // Timeline-Implementierung
            ImGui::End();
        }
    }

    void renderMixer() override {
        if (ImGui::Begin("Mixer")) {
            // Mixer-Implementierung
            ImGui::End();
        }
    }

    void renderEffects() override {
        if (ImGui::Begin("Effekte")) {
            // Effekte-Implementierung
            ImGui::End();
        }
    }

    void renderPianoRoll() override {
        if (ImGui::Begin("Piano Roll")) {
            // Piano Roll-Implementierung
            ImGui::End();
        }
    }

    void renderTransport() override {
        if (ImGui::Begin("Transport")) {
            // Transport-Implementierung
            ImGui::End();
        }
    }

    void setEventCallback(EventCallback callback) override {
        eventCallback = callback;
    }
};

std::unique_ptr<DesktopUI> DesktopUI::create() {
    return std::make_unique<DesktopUIImpl>();
}

} // namespace VRMusicStudio 