#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>
#include <mutex>

namespace VRMusicStudio {
namespace UI {

class UI {
public:
    static UI& getInstance();

    UI(const UI&) = delete;
    UI& operator=(const UI&) = delete;
    UI(UI&&) = delete;
    UI& operator=(UI&&) = delete;

    bool initialize();
    void shutdown();
    void update();

    bool isWindowOpen() const;
    void closeWindow();

    void setWindowSize(int width, int height);
    void getWindowSize(int& width, int& height) const;

    void setWindowTitle(const std::string& title);
    std::string getWindowTitle() const;

    void setClearColor(const glm::vec4& color);
    glm::vec4 getClearColor() const;

    void setVSync(bool enabled);
    bool isVSyncEnabled() const;

    void setFullscreen(bool enabled);
    bool isFullscreen() const;

    void setCursorVisible(bool visible);
    bool isCursorVisible() const;

    void setCursorPosition(double x, double y);
    void getCursorPosition(double& x, double& y) const;

    bool isKeyPressed(int key) const;
    bool isMouseButtonPressed(int button) const;

    void setWindowIcon(const std::string& iconPath);
    void setWindowPosition(int x, int y);
    void getWindowPosition(int& x, int& y) const;

    void setWindowAspectRatio(int numerator, int denominator);
    void setWindowResizable(bool resizable);
    bool isWindowResizable() const;

    void setWindowDecorated(bool decorated);
    bool isWindowDecorated() const;

    void setWindowFloating(bool floating);
    bool isWindowFloating() const;

    void setWindowMaximized(bool maximized);
    bool isWindowMaximized() const;

    void setWindowMinimized(bool minimized);
    bool isWindowMinimized() const;

    void setWindowFocus(bool focused);
    bool isWindowFocused() const;

    void setWindowShouldClose(bool shouldClose);
    bool shouldWindowClose() const;

private:
    UI();
    ~UI();

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void windowFocusCallback(GLFWwindow* window, int focused);
    static void windowIconifyCallback(GLFWwindow* window, int iconified);
    static void windowMaximizeCallback(GLFWwindow* window, int maximized);

    bool initialized;
    GLFWwindow* window;
    std::string title;
    glm::vec4 clearColor;
    bool vsync;
    bool fullscreen;
    bool cursorVisible;
    bool resizable;
    bool decorated;
    bool floating;
    bool maximized;
    bool minimized;
    bool focused;
    bool shouldClose;

    mutable std::mutex mutex;
};

} // namespace UI
} // namespace VRMusicStudio 