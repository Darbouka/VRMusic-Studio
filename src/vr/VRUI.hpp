#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

namespace VRMusicStudio {

class VRUI {
public:
    struct UIElement {
        enum class Type {
            Button,
            Slider,
            Toggle,
            Text,
            Panel,
            List,
            Input
        };
        
        Type type;
        std::string id;
        std::string label;
        glm::vec2 position;
        glm::vec2 size;
        bool visible;
        bool interactive;
        std::function<void()> callback;
    };
    
    struct UIPanel {
        std::string id;
        std::string title;
        glm::vec2 position;
        glm::vec2 size;
        bool visible;
        bool draggable;
        std::vector<UIElement> elements;
    };

    VRUI();
    ~VRUI();

    bool initialize();
    void shutdown();
    void update();
    void render();

    // Panel-Management
    void createPanel(const std::string& id, const std::string& title, 
                    const glm::vec2& position, const glm::vec2& size);
    void destroyPanel(const std::string& id);
    void showPanel(const std::string& id);
    void hidePanel(const std::string& id);
    bool isPanelVisible(const std::string& id) const;
    void setPanelPosition(const std::string& id, const glm::vec2& position);
    void setPanelSize(const std::string& id, const glm::vec2& size);

    // Element-Management
    void addButton(const std::string& panelId, const std::string& id,
                  const std::string& label, const glm::vec2& position,
                  const glm::vec2& size, std::function<void()> callback);
    void addSlider(const std::string& panelId, const std::string& id,
                  const std::string& label, const glm::vec2& position,
                  const glm::vec2& size, float* value, float min, float max);
    void addToggle(const std::string& panelId, const std::string& id,
                  const std::string& label, const glm::vec2& position,
                  const glm::vec2& size, bool* value);
    void addText(const std::string& panelId, const std::string& id,
                const std::string& text, const glm::vec2& position);
    void addInput(const std::string& panelId, const std::string& id,
                 const std::string& label, const glm::vec2& position,
                 const glm::vec2& size, char* buffer, size_t bufferSize);

    // Styling
    void setTheme(const std::string& theme);
    void setFont(const std::string& fontPath, float size);
    void setColor(const std::string& elementId, const glm::vec4& color);
    void setOpacity(const std::string& elementId, float opacity);

    // Interaktion
    bool isHovered(const std::string& elementId) const;
    bool isClicked(const std::string& elementId) const;
    void setInteractive(const std::string& elementId, bool interactive);
    void setCallback(const std::string& elementId, std::function<void()> callback);

    // Layout
    void setLayout(const std::string& panelId, const std::string& layout);
    void setSpacing(const std::string& panelId, float spacing);
    void setPadding(const std::string& panelId, float padding);
    void setAlignment(const std::string& panelId, const std::string& alignment);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void initializeImGui();
    void renderPanel(const UIPanel& panel);
    void renderElement(const UIElement& element);
    void handleInteraction(UIElement& element);
    void updateLayout(UIPanel& panel);
    glm::vec2 calculateElementPosition(const UIElement& element, const UIPanel& panel);
};

} // namespace VRMusicStudio 