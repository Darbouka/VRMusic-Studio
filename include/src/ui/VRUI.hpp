#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../core/Logger.hpp"
#include "../core/EventSystem.hpp"
#include "../vr/VRManager.hpp"

namespace VR_DAW {

// Basis-UI-Element
struct UIElement {
    std::string id;
    std::string text;
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;
    bool visible;
    bool interactive;
    std::function<void()> onClick;
    std::function<void()> onHover;
};

// Track-Ansicht
struct TrackView {
    std::string name;
    std::vector<UIElement> controls;
    std::vector<UIElement> plugins;
    bool isSelected;
    float volume;
    bool isMuted;
};

// Plugin-Ansicht
struct PluginView {
    std::string name;
    std::vector<UIElement> parameters;
    std::vector<UIElement> controls;
    bool isExpanded;
};

// Text-Element
struct TextElement : public UIElement {
    std::string font;
    float fontSize;
    glm::vec4 color;
};

// Button-Element
struct ButtonElement : public UIElement {
    glm::vec4 backgroundColor;
    glm::vec4 hoverColor;
    glm::vec4 textColor;
    float cornerRadius;
};

// Slider-Element
struct SliderElement : public UIElement {
    float minValue;
    float maxValue;
    float currentValue;
    glm::vec4 trackColor;
    glm::vec4 handleColor;
    std::function<void(float)> onValueChanged;
};

// Panel-Element
struct PanelElement : public UIElement {
    std::vector<std::shared_ptr<UIElement>> children;
    glm::vec4 backgroundColor;
    float cornerRadius;
};

class VRUI {
public:
    static VRUI& getInstance();

    bool initialize();
    void shutdown();
    bool isInitialized() const;

    // UI-Verwaltung
    void update();
    void render();
    void handleInput();

    // Element-Verwaltung
    std::shared_ptr<UIElement> createElement(const UIElement& element);
    void removeElement(const std::string& elementId);
    std::shared_ptr<UIElement> getElement(const std::string& elementId);
    
    // Track-Verwaltung
    std::shared_ptr<TrackView> createTrack(const std::string& name);
    void removeTrack(const std::string& trackId);
    std::shared_ptr<TrackView> getTrack(const std::string& trackId);
    
    // Plugin-Verwaltung
    std::shared_ptr<PluginView> createPluginView(const std::string& name);
    void removePluginView(const std::string& pluginId);
    std::shared_ptr<PluginView> getPluginView(const std::string& pluginId);

private:
    VRUI();
    ~VRUI();
    VRUI(const VRUI&) = delete;
    VRUI& operator=(const VRUI&) = delete;

    bool initializeRenderer();
    void shutdownRenderer();
    void updateElementTransforms();
    void renderElement(const std::shared_ptr<UIElement>& element);
    bool checkInteraction(const std::shared_ptr<UIElement>& element, const glm::vec3& controllerPosition);

    std::vector<std::shared_ptr<UIElement>> elements;
    std::vector<std::shared_ptr<TrackView>> tracks;
    std::vector<std::shared_ptr<PluginView>> plugins;
    bool initialized;
    std::mutex mutex;
    Logger logger;
};

} // namespace VR_DAW 