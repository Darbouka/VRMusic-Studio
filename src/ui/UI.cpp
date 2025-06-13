#include "UI.hpp"
#include "core/Logger.hpp"
#include <spdlog/spdlog.h>

namespace VRMusicStudio {
namespace UI {

UI& UI::getInstance() {
    static UI instance;
    return instance;
}

UI::UI()
    : initialized(false)
    , window(nullptr)
    , title("VRMusicStudio")
    , clearColor(0.2f, 0.3f, 0.3f, 1.0f)
    , vsync(true)
    , fullscreen(false)
    , cursorVisible(true)
    , resizable(true)
    , decorated(true)
    , floating(false)
    , maximized(false)
    , minimized(false)
    , focused(true)
    , shouldClose(false) {
}

UI::~UI() {
    shutdown();
}

bool UI::initialize() {
    std::lock_guard<std::mutex> lock(mutex);

    if (initialized) {
        return true;
    }

    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Initialisiere UI-System...");

        if (!glfwInit()) {
            throw std::runtime_error("Fehler bei der GLFW-Initialisierung");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, resizable);
        glfwWindowHint(GLFW_DECORATED, decorated);
        glfwWindowHint(GLFW_FLOATING, floating);

        window = glfwCreateWindow(1280, 720, title.c_str(), nullptr, nullptr);
        if (!window) {
            throw std::runtime_error("Fehler beim Erstellen des GLFW-Fensters");
        }

        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);

        // Setze Callbacks
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetCursorPosCallback(window, cursorPositionCallback);
        glfwSetScrollCallback(window, scrollCallback);
        glfwSetWindowFocusCallback(window, windowFocusCallback);
        glfwSetWindowIconifyCallback(window, windowIconifyCallback);
        glfwSetWindowMaximizeCallback(window, windowMaximizeCallback);

        // Initialisiere GLEW
        if (glewInit() != GLEW_OK) {
            throw std::runtime_error("Fehler bei der GLEW-Initialisierung");
        }

        // Setze VSync
        glfwSwapInterval(vsync ? 1 : 0);

        // Setze Cursor-Sichtbarkeit
        glfwSetInputMode(window, GLFW_CURSOR, cursorVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);

        initialized = true;
        logger.info("UI-System erfolgreich initialisiert");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der UI-Initialisierung: {}", e.what());
        return false;
    }
}

void UI::shutdown() {
    std::lock_guard<std::mutex> lock(mutex);

    if (!initialized) {
        return;
    }

    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Beende UI-System...");

        if (window) {
            glfwDestroyWindow(window);
            window = nullptr;
        }

        glfwTerminate();
        initialized = false;
        logger.info("UI-System erfolgreich beendet");
    } catch (const std::exception& e) {
        spdlog::error("Fehler beim Beenden des UI-Systems: {}", e.what());
    }
}

void UI::update() {
    std::lock_guard<std::mutex> lock(mutex);

    if (!initialized || !window) {
        return;
    }

    try {
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // UI-Rendering hier implementieren

        glfwSwapBuffers(window);
        glfwPollEvents();
    } catch (const std::exception& e) {
        spdlog::error("Fehler im UI-Update: {}", e.what());
    }
}

bool UI::isWindowOpen() const {
    std::lock_guard<std::mutex> lock(mutex);
    return initialized && window && !glfwWindowShouldClose(window);
}

void UI::closeWindow() {
    std::lock_guard<std::mutex> lock(mutex);
    if (window) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void UI::setWindowSize(int width, int height) {
    std::lock_guard<std::mutex> lock(mutex);
    if (window) {
        glfwSetWindowSize(window, width, height);
    }
}

void UI::getWindowSize(int& width, int& height) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (window) {
        glfwGetWindowSize(window, &width, &height);
    } else {
        width = 0;
        height = 0;
    }
}

void UI::setWindowTitle(const std::string& newTitle) {
    std::lock_guard<std::mutex> lock(mutex);
    title = newTitle;
    if (window) {
        glfwSetWindowTitle(window, title.c_str());
    }
}

std::string UI::getWindowTitle() const {
    std::lock_guard<std::mutex> lock(mutex);
    return title;
}

void UI::setClearColor(const glm::vec4& color) {
    std::lock_guard<std::mutex> lock(mutex);
    clearColor = color;
}

glm::vec4 UI::getClearColor() const {
    std::lock_guard<std::mutex> lock(mutex);
    return clearColor;
}

void UI::setVSync(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex);
    vsync = enabled;
    if (window) {
        glfwSwapInterval(vsync ? 1 : 0);
    }
}

bool UI::isVSyncEnabled() const {
    std::lock_guard<std::mutex> lock(mutex);
    return vsync;
}

void UI::setFullscreen(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex);
    if (fullscreen == enabled) {
        return;
    }

    if (window) {
        if (enabled) {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        } else {
            glfwSetWindowMonitor(window, nullptr, 100, 100, 1280, 720, 0);
        }
    }

    fullscreen = enabled;
}

bool UI::isFullscreen() const {
    std::lock_guard<std::mutex> lock(mutex);
    return fullscreen;
}

void UI::setCursorVisible(bool visible) {
    std::lock_guard<std::mutex> lock(mutex);
    cursorVisible = visible;
    if (window) {
        glfwSetInputMode(window, GLFW_CURSOR, cursorVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
    }
}

bool UI::isCursorVisible() const {
    std::lock_guard<std::mutex> lock(mutex);
    return cursorVisible;
}

void UI::setCursorPosition(double x, double y) {
    std::lock_guard<std::mutex> lock(mutex);
    if (window) {
        glfwSetCursorPos(window, x, y);
    }
}

void UI::getCursorPosition(double& x, double& y) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (window) {
        glfwGetCursorPos(window, &x, &y);
    } else {
        x = 0.0;
        y = 0.0;
    }
}

bool UI::isKeyPressed(int key) const {
    std::lock_guard<std::mutex> lock(mutex);
    return window && glfwGetKey(window, key) == GLFW_PRESS;
}

bool UI::isMouseButtonPressed(int button) const {
    std::lock_guard<std::mutex> lock(mutex);
    return window && glfwGetMouseButton(window, button) == GLFW_PRESS;
}

void UI::setWindowIcon(const std::string& iconPath) {
    std::lock_guard<std::mutex> lock(mutex);
    if (window) {
        GLFWimage image;
        // Icon-Laden hier implementieren
        glfwSetWindowIcon(window, 1, &image);
    }
}

void UI::setWindowPosition(int x, int y) {
    std::lock_guard<std::mutex> lock(mutex);
    if (window) {
        glfwSetWindowPos(window, x, y);
    }
}

void UI::getWindowPosition(int& x, int& y) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (window) {
        glfwGetWindowPos(window, &x, &y);
    } else {
        x = 0;
        y = 0;
    }
}

void UI::setWindowAspectRatio(int numerator, int denominator) {
    std::lock_guard<std::mutex> lock(mutex);
    if (window) {
        glfwSetWindowAspectRatio(window, numerator, denominator);
    }
}

void UI::setWindowResizable(bool resizable) {
    std::lock_guard<std::mutex> lock(mutex);
    this->resizable = resizable;
    if (window) {
        glfwSetWindowAttrib(window, GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
    }
}

bool UI::isWindowResizable() const {
    std::lock_guard<std::mutex> lock(mutex);
    return resizable;
}

void UI::setWindowDecorated(bool decorated) {
    std::lock_guard<std::mutex> lock(mutex);
    this->decorated = decorated;
    if (window) {
        glfwSetWindowAttrib(window, GLFW_DECORATED, decorated ? GLFW_TRUE : GLFW_FALSE);
    }
}

bool UI::isWindowDecorated() const {
    std::lock_guard<std::mutex> lock(mutex);
    return decorated;
}

void UI::setWindowFloating(bool floating) {
    std::lock_guard<std::mutex> lock(mutex);
    this->floating = floating;
    if (window) {
        glfwSetWindowAttrib(window, GLFW_FLOATING, floating ? GLFW_TRUE : GLFW_FALSE);
    }
}

bool UI::isWindowFloating() const {
    std::lock_guard<std::mutex> lock(mutex);
    return floating;
}

void UI::setWindowMaximized(bool maximized) {
    std::lock_guard<std::mutex> lock(mutex);
    if (window) {
        if (maximized) {
            glfwMaximizeWindow(window);
        } else {
            glfwRestoreWindow(window);
        }
    }
}

bool UI::isWindowMaximized() const {
    std::lock_guard<std::mutex> lock(mutex);
    return window && glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE;
}

void UI::setWindowMinimized(bool minimized) {
    std::lock_guard<std::mutex> lock(mutex);
    if (window) {
        if (minimized) {
            glfwIconifyWindow(window);
        } else {
            glfwRestoreWindow(window);
        }
    }
}

bool UI::isWindowMinimized() const {
    std::lock_guard<std::mutex> lock(mutex);
    return window && glfwGetWindowAttrib(window, GLFW_ICONIFIED) == GLFW_TRUE;
}

void UI::setWindowFocus(bool focused) {
    std::lock_guard<std::mutex> lock(mutex);
    if (window) {
        if (focused) {
            glfwFocusWindow(window);
        }
    }
}

bool UI::isWindowFocused() const {
    std::lock_guard<std::mutex> lock(mutex);
    return window && glfwGetWindowAttrib(window, GLFW_FOCUSED) == GLFW_TRUE;
}

void UI::setWindowShouldClose(bool shouldClose) {
    std::lock_guard<std::mutex> lock(mutex);
    this->shouldClose = shouldClose;
    if (window) {
        glfwSetWindowShouldClose(window, shouldClose ? GLFW_TRUE : GLFW_FALSE);
    }
}

bool UI::shouldWindowClose() const {
    std::lock_guard<std::mutex> lock(mutex);
    return shouldClose || (window && glfwWindowShouldClose(window));
}

void UI::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto* ui = static_cast<UI*>(glfwGetWindowUserPointer(window));
    if (ui) {
        glViewport(0, 0, width, height);
    }
}

void UI::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto* ui = static_cast<UI*>(glfwGetWindowUserPointer(window));
    if (ui) {
        // Tastatur-Callback-Logik hier implementieren
    }
}

void UI::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto* ui = static_cast<UI*>(glfwGetWindowUserPointer(window));
    if (ui) {
        // Maus-Button-Callback-Logik hier implementieren
    }
}

void UI::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* ui = static_cast<UI*>(glfwGetWindowUserPointer(window));
    if (ui) {
        // Cursor-Position-Callback-Logik hier implementieren
    }
}

void UI::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto* ui = static_cast<UI*>(glfwGetWindowUserPointer(window));
    if (ui) {
        // Scroll-Callback-Logik hier implementieren
    }
}

void UI::windowFocusCallback(GLFWwindow* window, int focused) {
    auto* ui = static_cast<UI*>(glfwGetWindowUserPointer(window));
    if (ui) {
        ui->focused = focused == GLFW_TRUE;
    }
}

void UI::windowIconifyCallback(GLFWwindow* window, int iconified) {
    auto* ui = static_cast<UI*>(glfwGetWindowUserPointer(window));
    if (ui) {
        ui->minimized = iconified == GLFW_TRUE;
    }
}

void UI::windowMaximizeCallback(GLFWwindow* window, int maximized) {
    auto* ui = static_cast<UI*>(glfwGetWindowUserPointer(window));
    if (ui) {
        ui->maximized = maximized == GLFW_TRUE;
    }
}

} // namespace UI
} // namespace VRMusicStudio 