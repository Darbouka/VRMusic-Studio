#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace VRMusicStudio {

class UIManager {
public:
    struct UIElement {
        std::string id;
        std::string name;
        glm::vec2 position;
        glm::vec2 size;
        bool visible;
        bool enabled;
        std::function<void()> onClick;
        std::function<void()> onHover;
        std::function<void()> onDrag;
    };

    struct UIWindow {
        std::string id;
        std::string title;
        glm::vec2 position;
        glm::vec2 size;
        bool visible;
        bool resizable;
        bool movable;
        std::vector<UIElement> elements;
    };

    struct UIPanel {
        std::string id;
        std::string name;
        glm::vec2 position;
        glm::vec2 size;
        bool visible;
        std::vector<UIElement> elements;
    };

    struct UIMenu {
        std::string id;
        std::string name;
        std::vector<std::string> items;
        std::function<void(const std::string&)> onSelect;
    };

    UIManager();
    ~UIManager();

    bool initialize();
    void shutdown();

    // Fenster-Management
    void createWindow(const std::string& id, const std::string& title, const glm::vec2& position, const glm::vec2& size);
    void destroyWindow(const std::string& id);
    void showWindow(const std::string& id);
    void hideWindow(const std::string& id);
    void setWindowPosition(const std::string& id, const glm::vec2& position);
    void setWindowSize(const std::string& id, const glm::vec2& size);

    // Panel-Management
    void createPanel(const std::string& id, const std::string& name, const glm::vec2& position, const glm::vec2& size);
    void destroyPanel(const std::string& id);
    void showPanel(const std::string& id);
    void hidePanel(const std::string& id);
    void setPanelPosition(const std::string& id, const glm::vec2& position);
    void setPanelSize(const std::string& id, const glm::vec2& size);

    // Element-Management
    void addElement(const std::string& windowId, const UIElement& element);
    void removeElement(const std::string& windowId, const std::string& elementId);
    void showElement(const std::string& windowId, const std::string& elementId);
    void hideElement(const std::string& windowId, const std::string& elementId);
    void setElementPosition(const std::string& windowId, const std::string& elementId, const glm::vec2& position);
    void setElementSize(const std::string& windowId, const std::string& elementId, const glm::vec2& size);

    // Menü-Management
    void createMenu(const std::string& id, const std::string& name, const std::vector<std::string>& items);
    void destroyMenu(const std::string& id);
    void setMenuCallback(const std::string& id, std::function<void(const std::string&)> callback);

    // Rendering
    void beginFrame();
    void endFrame();
    void render();
    void renderWindow(const std::string& id);
    void renderPanel(const std::string& id);
    void renderMenu(const std::string& id);

    // Callbacks
    void setWindowCallback(const std::string& id, std::function<void()> callback);
    void setPanelCallback(const std::string& id, std::function<void()> callback);
    void setElementCallback(const std::string& windowId, const std::string& elementId, std::function<void()> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace VRMusicStudio 