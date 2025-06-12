#include "VRUI.hpp"
#include "spdlog/spdlog.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace VRMusicStudio {

struct VRUI::Impl {
    GLFWwindow* window = nullptr;
    ImGuiContext* context = nullptr;
    std::map<std::string, UIPanel> panels;
    std::string currentTheme;
    std::string currentFont;
    float currentFontSize = 16.0f;
    
    // Styling
    struct {
        glm::vec4 backgroundColor = glm::vec4(0.1f, 0.1f, 0.1f, 0.9f);
        glm::vec4 textColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec4 buttonColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
        glm::vec4 buttonHoverColor = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
        glm::vec4 buttonActiveColor = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
        float cornerRadius = 4.0f;
        float padding = 8.0f;
        float spacing = 4.0f;
    } style;
};

VRUI::VRUI() : pImpl(std::make_unique<Impl>()) {}

VRUI::~VRUI() {
    shutdown();
}

bool VRUI::initialize() {
    if (!glfwInit()) {
        spdlog::error("Failed to initialize GLFW");
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    pImpl->window = glfwCreateWindow(1280, 720, "VR UI", nullptr, nullptr);
    if (!pImpl->window) {
        spdlog::error("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(pImpl->window);
    glfwSwapInterval(1);
    
    IMGUI_CHECKVERSION();
    pImpl->context = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(pImpl->window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    return true;
}

void VRUI::shutdown() {
    if (pImpl->context) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(pImpl->context);
        pImpl->context = nullptr;
    }
    
    if (pImpl->window) {
        glfwDestroyWindow(pImpl->window);
        pImpl->window = nullptr;
    }
    
    glfwTerminate();
}

void VRUI::update() {
    if (!pImpl->window) return;
    
    glfwPollEvents();
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    for (auto& panel : pImpl->panels) {
        if (panel.second.visible) {
            updateLayout(panel.second);
        }
    }
}

void VRUI::render() {
    if (!pImpl->window) return;
    
    ImGui::Render();
    
    int display_w, display_h;
    glfwGetFramebufferSize(pImpl->window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    glfwSwapBuffers(pImpl->window);
}

void VRUI::createPanel(const std::string& id, const std::string& title,
                      const glm::vec2& position, const glm::vec2& size) {
    UIPanel panel;
    panel.id = id;
    panel.title = title;
    panel.position = position;
    panel.size = size;
    panel.visible = true;
    panel.draggable = true;
    pImpl->panels[id] = panel;
}

void VRUI::destroyPanel(const std::string& id) {
    pImpl->panels.erase(id);
}

void VRUI::showPanel(const std::string& id) {
    auto it = pImpl->panels.find(id);
    if (it != pImpl->panels.end()) {
        it->second.visible = true;
    }
}

void VRUI::hidePanel(const std::string& id) {
    auto it = pImpl->panels.find(id);
    if (it != pImpl->panels.end()) {
        it->second.visible = false;
    }
}

bool VRUI::isPanelVisible(const std::string& id) const {
    auto it = pImpl->panels.find(id);
    return it != pImpl->panels.end() && it->second.visible;
}

void VRUI::setPanelPosition(const std::string& id, const glm::vec2& position) {
    auto it = pImpl->panels.find(id);
    if (it != pImpl->panels.end()) {
        it->second.position = position;
    }
}

void VRUI::setPanelSize(const std::string& id, const glm::vec2& size) {
    auto it = pImpl->panels.find(id);
    if (it != pImpl->panels.end()) {
        it->second.size = size;
    }
}

void VRUI::addButton(const std::string& panelId, const std::string& id,
                    const std::string& label, const glm::vec2& position,
                    const glm::vec2& size, std::function<void()> callback) {
    auto it = pImpl->panels.find(panelId);
    if (it != pImpl->panels.end()) {
        UIElement element;
        element.type = UIElement::Type::Button;
        element.id = id;
        element.label = label;
        element.position = position;
        element.size = size;
        element.visible = true;
        element.interactive = true;
        element.callback = callback;
        it->second.elements.push_back(element);
    }
}

void VRUI::addSlider(const std::string& panelId, const std::string& id,
                    const std::string& label, const glm::vec2& position,
                    const glm::vec2& size, float* value, float min, float max) {
    auto it = pImpl->panels.find(panelId);
    if (it != pImpl->panels.end()) {
        UIElement element;
        element.type = UIElement::Type::Slider;
        element.id = id;
        element.label = label;
        element.position = position;
        element.size = size;
        element.visible = true;
        element.interactive = true;
        it->second.elements.push_back(element);
    }
}

void VRUI::addToggle(const std::string& panelId, const std::string& id,
                    const std::string& label, const glm::vec2& position,
                    const glm::vec2& size, bool* value) {
    auto it = pImpl->panels.find(panelId);
    if (it != pImpl->panels.end()) {
        UIElement element;
        element.type = UIElement::Type::Toggle;
        element.id = id;
        element.label = label;
        element.position = position;
        element.size = size;
        element.visible = true;
        element.interactive = true;
        it->second.elements.push_back(element);
    }
}

void VRUI::addText(const std::string& panelId, const std::string& id,
                  const std::string& text, const glm::vec2& position) {
    auto it = pImpl->panels.find(panelId);
    if (it != pImpl->panels.end()) {
        UIElement element;
        element.type = UIElement::Type::Text;
        element.id = id;
        element.label = text;
        element.position = position;
        element.size = glm::vec2(0.0f);
        element.visible = true;
        element.interactive = false;
        it->second.elements.push_back(element);
    }
}

void VRUI::addInput(const std::string& panelId, const std::string& id,
                   const std::string& label, const glm::vec2& position,
                   const glm::vec2& size, char* buffer, size_t bufferSize) {
    auto it = pImpl->panels.find(panelId);
    if (it != pImpl->panels.end()) {
        UIElement element;
        element.type = UIElement::Type::Input;
        element.id = id;
        element.label = label;
        element.position = position;
        element.size = size;
        element.visible = true;
        element.interactive = true;
        it->second.elements.push_back(element);
    }
}

void VRUI::setTheme(const std::string& theme) {
    pImpl->currentTheme = theme;
    // TODO: Implementiere Theme-Wechsel
}

void VRUI::setFont(const std::string& fontPath, float size) {
    pImpl->currentFont = fontPath;
    pImpl->currentFontSize = size;
    // TODO: Implementiere Font-Laden
}

void VRUI::setColor(const std::string& elementId, const glm::vec4& color) {
    // TODO: Implementiere Farbänderung
}

void VRUI::setOpacity(const std::string& elementId, float opacity) {
    // TODO: Implementiere Transparenz
}

bool VRUI::isHovered(const std::string& elementId) const {
    // TODO: Implementiere Hover-Erkennung
    return false;
}

bool VRUI::isClicked(const std::string& elementId) const {
    // TODO: Implementiere Click-Erkennung
    return false;
}

void VRUI::setInteractive(const std::string& elementId, bool interactive) {
    // TODO: Implementiere Interaktivitätsänderung
}

void VRUI::setCallback(const std::string& elementId, std::function<void()> callback) {
    // TODO: Implementiere Callback-Änderung
}

void VRUI::setLayout(const std::string& panelId, const std::string& layout) {
    // TODO: Implementiere Layout-Änderung
}

void VRUI::setSpacing(const std::string& panelId, float spacing) {
    auto it = pImpl->panels.find(panelId);
    if (it != pImpl->panels.end()) {
        pImpl->style.spacing = spacing;
    }
}

void VRUI::setPadding(const std::string& panelId, float padding) {
    auto it = pImpl->panels.find(panelId);
    if (it != pImpl->panels.end()) {
        pImpl->style.padding = padding;
    }
}

void VRUI::setAlignment(const std::string& panelId, const std::string& alignment) {
    // TODO: Implementiere Ausrichtung
}

void VRUI::renderPanel(const UIPanel& panel) {
    if (!panel.visible) return;
    
    ImGui::SetNextWindowPos(ImVec2(panel.position.x, panel.position.y));
    ImGui::SetNextWindowSize(ImVec2(panel.size.x, panel.size.y));
    
    ImGui::Begin(panel.title.c_str(), nullptr,
                 ImGuiWindowFlags_NoCollapse |
                 (panel.draggable ? 0 : ImGuiWindowFlags_NoMove));
    
    for (const auto& element : panel.elements) {
        renderElement(element);
    }
    
    ImGui::End();
}

void VRUI::renderElement(const UIElement& element) {
    if (!element.visible) return;
    
    ImGui::SetCursorPos(ImVec2(element.position.x, element.position.y));
    
    switch (element.type) {
        case UIElement::Type::Button:
            if (ImGui::Button(element.label.c_str(),
                            ImVec2(element.size.x, element.size.y))) {
                if (element.callback) {
                    element.callback();
                }
            }
            break;
            
        case UIElement::Type::Slider:
            // TODO: Implementiere Slider
            break;
            
        case UIElement::Type::Toggle:
            // TODO: Implementiere Toggle
            break;
            
        case UIElement::Type::Text:
            ImGui::Text("%s", element.label.c_str());
            break;
            
        case UIElement::Type::Input:
            // TODO: Implementiere Input
            break;
            
        default:
            break;
    }
}

void VRUI::handleInteraction(UIElement& element) {
    if (!element.interactive) return;
    
    // TODO: Implementiere Interaktion
}

void VRUI::updateLayout(UIPanel& panel) {
    float currentY = panel.position.y + pImpl->style.padding;
    
    for (auto& element : panel.elements) {
        element.position.y = currentY;
        currentY += element.size.y + pImpl->style.spacing;
    }
}

glm::vec2 VRUI::calculateElementPosition(const UIElement& element,
                                       const UIPanel& panel) {
    // TODO: Implementiere Positionsberechnung
    return element.position;
}

} // namespace VRMusicStudio 
} // namespace VRMusicStudio 