#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <string>
#include <vector>

namespace VRMusicStudio {
namespace UI {

class UI;
class VRInteraction;

class VRUI {
public:
    struct Panel {
        std::string id;
        glm::vec3 position;
        glm::quat rotation;
        glm::vec2 size;
        bool visible;
        bool interactive;
        glm::vec4 color;
        glm::vec4 textColor;
        float fontSize;
        bool hovered;
        bool clicked;
        glm::vec2 cursorPosition;
        std::string text;
        std::string texturePath;
        std::string fontPath;
    };

    VRUI(UI& ui, VRInteraction& vrInteraction);
    ~VRUI();

    bool initialize();
    void shutdown();
    void update();
    void render();

    void createPanel(const std::string& id, const glm::vec3& position, const glm::vec2& size);
    void destroyPanel(const std::string& id);
    void setPanelPosition(const std::string& id, const glm::vec3& position);
    void setPanelSize(const std::string& id, const glm::vec2& size);
    void setPanelVisible(const std::string& id, bool visible);
    void setPanelInteractive(const std::string& id, bool interactive);
    bool isPanelHovered(const std::string& id);
    bool isPanelClicked(const std::string& id);
    glm::vec2 getPanelCursorPosition(const std::string& id);
    void setPanelCursorPosition(const std::string& id, const glm::vec2& position);
    void setPanelColor(const std::string& id, const glm::vec4& color);
    void setPanelTexture(const std::string& id, const std::string& texturePath);
    void setPanelFont(const std::string& id, const std::string& fontPath, float size);
    void setPanelText(const std::string& id, const std::string& text);
    void setPanelTextColor(const std::string& id, const glm::vec4& color);

private:
    UI& ui;
    VRInteraction& vrInteraction;
    glm::vec3 cursorPosition;
    glm::quat cursorRotation;
    bool cursorVisible;
    float cursorSize;
    glm::vec4 cursorColor;
    std::vector<std::unique_ptr<Panel>> panels;
};

} // namespace UI
} // namespace VRMusicStudio 