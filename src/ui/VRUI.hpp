#pragma once

#include "UI.hpp"
#include "../vr/VRInteraction.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>

namespace VRMusicStudio {
namespace UI {

class VRUI {
public:
    VRUI(UI& ui, VR::VRInteraction& vrInteraction);
    ~VRUI();

    bool initialize();
    void shutdown();
    void update();
    void render();

    // UI-Elemente
    void createPanel(const std::string& id, const glm::vec3& position, const glm::vec2& size);
    void destroyPanel(const std::string& id);
    void setPanelPosition(const std::string& id, const glm::vec3& position);
    void setPanelSize(const std::string& id, const glm::vec2& size);
    void setPanelVisible(const std::string& id, bool visible);
    void setPanelInteractive(const std::string& id, bool interactive);

    // Interaktion
    bool isPanelHovered(const std::string& id);
    bool isPanelClicked(const std::string& id);
    glm::vec2 getPanelCursorPosition(const std::string& id);
    void setPanelCursorPosition(const std::string& id, const glm::vec2& position);

    // Styling
    void setPanelColor(const std::string& id, const glm::vec4& color);
    void setPanelTexture(const std::string& id, const std::string& texturePath);
    void setPanelFont(const std::string& id, const std::string& fontPath, float size);
    void setPanelText(const std::string& id, const std::string& text);
    void setPanelTextColor(const std::string& id, const glm::vec4& color);

private:
    struct Panel {
        std::string id;
        glm::vec3 position;
        glm::vec2 size;
        bool visible;
        bool interactive;
        glm::vec4 color;
        std::string texturePath;
        std::string fontPath;
        float fontSize;
        std::string text;
        glm::vec4 textColor;
        bool hovered;
        bool clicked;
        glm::vec2 cursorPosition;
    };

    UI& ui;
    VR::VRInteraction& vrInteraction;
    std::vector<std::unique_ptr<Panel>> panels;
    std::string activePanel;
    glm::vec3 cursorPosition;
    glm::quat cursorRotation;
    bool cursorVisible;
    float cursorSize;
    glm::vec4 cursorColor;
};

} // namespace UI
} // namespace VRMusicStudio 