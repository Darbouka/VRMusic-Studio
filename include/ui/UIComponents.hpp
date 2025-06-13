#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <memory>

namespace VRMusicStudio {

class UIComponents {
public:
    UIComponents();
    ~UIComponents();
    
    void initialize();
    void shutdown();
    void update();
    
    bool shouldClose() const;

private:
    void renderMainMenuBar();
    void renderAudioWindow();
    void renderMIDIWindow();
    void renderVRWindow();
    
    GLFWwindow* m_window;
    bool m_isInitialized;
    
    bool m_showDemoWindow;
    bool m_showAudioWindow;
    bool m_showMIDIWindow;
    bool m_showVRWindow;
};

} // namespace VRMusicStudio 