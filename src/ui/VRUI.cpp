#include "ui/VRUI.hpp"
#include "core/Logger.hpp"
#include "vr/VRInteraction.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>

namespace VRMusicStudio {
namespace UI {

VRUI::VRUI(UI& ui, VR::VRInteraction& vrInteraction)
    : ui(ui)
    , vrInteraction(vrInteraction)
    , cursorPosition(0.0f)
    , cursorRotation(1.0f, 0.0f, 0.0f, 0.0f)
    , cursorVisible(true)
    , cursorSize(0.01f)
    , cursorColor(1.0f, 1.0f, 1.0f, 0.5f) {
}

VRUI::~VRUI() = default;

bool VRUI::initialize() {
    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Initialisiere VR-UI...");

        // Initialisiere Cursor
        cursorPosition = glm::vec3(0.0f);
        cursorRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        cursorVisible = true;

        logger.info("VR-UI erfolgreich initialisiert");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der VR-UI-Initialisierung: {}", e.what());
        return false;
    }
}

void VRUI::shutdown() {
    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Beende VR-UI...");

        // Lösche alle Panels
        panels.clear();

        logger.info("VR-UI erfolgreich beendet");
    } catch (const std::exception& e) {
        spdlog::error("Fehler beim Beenden der VR-UI: {}", e.what());
    }
}

void VRUI::update() {
    // Aktualisiere Cursor-Position und -Rotation
    cursorPosition = vrInteraction.getControllerPosition(0);
    cursorRotation = vrInteraction.getControllerRotation(0);

    // Aktualisiere Panel-Interaktionen
    for (auto& panel : panels) {
        if (!panel->visible || !panel->interactive) {
            continue;
        }

        // Berechne Cursor-Position relativ zum Panel
        glm::mat4 panelTransform = glm::translate(glm::mat4(1.0f), panel->position);
        glm::mat4 inverseTransform = glm::inverse(panelTransform);
        glm::vec4 localCursor = inverseTransform * glm::vec4(cursorPosition, 1.0f);
        
        // Prüfe, ob der Cursor über dem Panel ist
        panel->hovered = std::abs(localCursor.x) <= panel->size.x / 2.0f &&
                        std::abs(localCursor.y) <= panel->size.y / 2.0f;

        // Aktualisiere Klick-Status
        if (panel->hovered) {
            panel->clicked = vrInteraction.isButtonPressed(0, 0); // 0 für Trigger-Button
        } else {
            panel->clicked = false;
        }

        // Aktualisiere Cursor-Position im Panel
        if (panel->hovered) {
            panel->cursorPosition = glm::vec2(
                (localCursor.x + panel->size.x / 2.0f) / panel->size.x,
                (localCursor.y + panel->size.y / 2.0f) / panel->size.y
            );
        }
    }
}

void VRUI::render() {
    // Rendere Panels
    for (const auto& panel : panels) {
        if (!panel->visible) {
            continue;
        }

        // Rendere Panel-Hintergrund
        // TODO: Implementiere Panel-Rendering

        // Rendere Panel-Text
        if (!panel->text.empty()) {
            // TODO: Implementiere Text-Rendering
        }
    }

    // Rendere Cursor
    if (cursorVisible) {
        // TODO: Implementiere Cursor-Rendering
    }
}

void VRUI::createPanel(const std::string& id, const glm::vec3& position, const glm::vec2& size) {
    auto panel = std::make_unique<Panel>();
    panel->id = id;
    panel->position = position;
    panel->size = size;
    panel->visible = true;
    panel->interactive = true;
    panel->color = glm::vec4(0.2f, 0.2f, 0.2f, 0.8f);
    panel->textColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    panel->fontSize = 0.02f;
    panel->hovered = false;
    panel->clicked = false;
    panel->cursorPosition = glm::vec2(0.0f);

    panels.push_back(std::move(panel));
}

void VRUI::destroyPanel(const std::string& id) {
    panels.erase(
        std::remove_if(panels.begin(), panels.end(),
            [&id](const auto& panel) { return panel->id == id; }),
        panels.end()
    );
}

void VRUI::setPanelPosition(const std::string& id, const glm::vec3& position) {
    auto it = std::find_if(panels.begin(), panels.end(),
        [&id](const auto& panel) { return panel->id == id; });
    if (it != panels.end()) {
        (*it)->position = position;
    }
}

void VRUI::setPanelSize(const std::string& id, const glm::vec2& size) {
    auto it = std::find_if(panels.begin(), panels.end(),
        [&id](const auto& panel) { return panel->id == id; });
    if (it != panels.end()) {
        (*it)->size = size;
    }
}

void VRUI::setPanelVisible(const std::string& id, bool visible) {
    auto it = std::find_if(panels.begin(), panels.end(),
        [&id](const auto& panel) { return panel->id == id; });
    if (it != panels.end()) {
        (*it)->visible = visible;
    }
}

void VRUI::setPanelInteractive(const std::string& id, bool interactive) {
    auto it = std::find_if(panels.begin(), panels.end(),
        [&id](const auto& panel) { return panel->id == id; });
    if (it != panels.end()) {
        (*it)->interactive = interactive;
    }
}

bool VRUI::isPanelHovered(const std::string& id) {
    auto it = std::find_if(panels.begin(), panels.end(),
        [&id](const auto& panel) { return panel->id == id; });
    return it != panels.end() && (*it)->hovered;
}

bool VRUI::isPanelClicked(const std::string& id) {
    auto it = std::find_if(panels.begin(), panels.end(),
        [&id](const auto& panel) { return panel->id == id; });
    return it != panels.end() && (*it)->clicked;
}

glm::vec2 VRUI::getPanelCursorPosition(const std::string& id) {
    auto it = std::find_if(panels.begin(), panels.end(),
        [&id](const auto& panel) { return panel->id == id; });
    return it != panels.end() ? (*it)->cursorPosition : glm::vec2(0.0f);
}

void VRUI::setPanelCursorPosition(const std::string& id, const glm::vec2& position) {
    auto it = std::find_if(panels.begin(), panels.end(),
        [&id](const auto& panel) { return panel->id == id; });
    if (it != panels.end()) {
        (*it)->cursorPosition = position;
    }
}

void VRUI::setPanelColor(const std::string& id, const glm::vec4& color) {
    auto it = std::find_if(panels.begin(), panels.end(),
        [&id](const auto& panel) { return panel->id == id; });
    if (it != panels.end()) {
        (*it)->color = color;
    }
}

void VRUI::setPanelTexture(const std::string& id, const std::string& texturePath) {
    auto it = std::find_if(panels.begin(), panels.end(),
        [&id](const auto& panel) { return panel->id == id; });
    if (it != panels.end()) {
        (*it)->texturePath = texturePath;
    }
}

void VRUI::setPanelFont(const std::string& id, const std::string& fontPath, float size) {
    auto it = std::find_if(panels.begin(), panels.end(),
        [&id](const auto& panel) { return panel->id == id; });
    if (it != panels.end()) {
        (*it)->fontPath = fontPath;
        (*it)->fontSize = size;
    }
}

void VRUI::setPanelText(const std::string& id, const std::string& text) {
    auto it = std::find_if(panels.begin(), panels.end(),
        [&id](const auto& panel) { return panel->id == id; });
    if (it != panels.end()) {
        (*it)->text = text;
    }
}

void VRUI::setPanelTextColor(const std::string& id, const glm::vec4& color) {
    auto it = std::find_if(panels.begin(), panels.end(),
        [&id](const auto& panel) { return panel->id == id; });
    if (it != panels.end()) {
        (*it)->textColor = color;
    }
}

} // namespace UI
} // namespace VRMusicStudio 