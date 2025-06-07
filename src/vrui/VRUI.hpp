#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace VR_DAW {

// Forward-Deklarationen
class VRUIElement;
class VRUIPanel;
class VRUIButton;
class VRUISlider;
class VRUIKnob;
class VRUIToggle;
class VRUILabel;
class VRUIMenu;
class VRUICanvas;
class VRUIRenderer;

// Basis-Klasse für alle VRUI-Elemente
class VRUIElement {
public:
    VRUIElement();
    virtual ~VRUIElement();

    // Position und Rotation
    void setPosition(const glm::vec3& position);
    void setRotation(const glm::quat& rotation);
    void setScale(const glm::vec3& scale);
    
    const glm::vec3& getPosition() const;
    const glm::quat& getRotation() const;
    const glm::vec3& getScale() const;

    // Sichtbarkeit
    void setVisible(bool visible);
    bool isVisible() const;

    // Interaktivität
    void setInteractive(bool interactive);
    bool isInteractive() const;

    // Event-Handler
    using EventCallback = std::function<void(VRUIElement*)>;
    void setOnHover(EventCallback callback);
    void setOnClick(EventCallback callback);
    void setOnDrag(EventCallback callback);
    void setOnRelease(EventCallback callback);

    // Rendering
    virtual void render(VRUIRenderer& renderer) = 0;

protected:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    bool visible;
    bool interactive;
    EventCallback onHover;
    EventCallback onClick;
    EventCallback onDrag;
    EventCallback onRelease;
};

// Panel-Klasse für Gruppierung von Elementen
class VRUIPanel : public VRUIElement {
public:
    VRUIPanel();
    ~VRUIPanel();

    // Element-Management
    void addElement(std::shared_ptr<VRUIElement> element);
    void removeElement(std::shared_ptr<VRUIElement> element);
    void clearElements();

    // Layout
    void setLayout(const std::string& layout);
    void setSpacing(float spacing);
    void setPadding(float padding);

    // Rendering
    void render(VRUIRenderer& renderer) override;

private:
    std::vector<std::shared_ptr<VRUIElement>> elements;
    std::string layout;
    float spacing;
    float padding;
};

// Button-Klasse
class VRUIButton : public VRUIElement {
public:
    VRUIButton();
    ~VRUIButton();

    // Text
    void setText(const std::string& text);
    const std::string& getText() const;

    // Styling
    void setColor(const glm::vec4& color);
    void setHoverColor(const glm::vec4& color);
    void setPressedColor(const glm::vec4& color);

    // Rendering
    void render(VRUIRenderer& renderer) override;

private:
    std::string text;
    glm::vec4 color;
    glm::vec4 hoverColor;
    glm::vec4 pressedColor;
    bool isHovered;
    bool isPressed;
};

// Slider-Klasse
class VRUISlider : public VRUIElement {
public:
    VRUISlider();
    ~VRUISlider();

    // Werte
    void setValue(float value);
    void setMinValue(float minValue);
    void setMaxValue(float maxValue);
    float getValue() const;

    // Styling
    void setTrackColor(const glm::vec4& color);
    void setHandleColor(const glm::vec4& color);
    void setHoverColor(const glm::vec4& color);

    // Rendering
    void render(VRUIRenderer& renderer) override;

private:
    float value;
    float minValue;
    float maxValue;
    glm::vec4 trackColor;
    glm::vec4 handleColor;
    glm::vec4 hoverColor;
    bool isHovered;
    bool isDragging;
};

// Knob-Klasse
class VRUIKnob : public VRUIElement {
public:
    VRUIKnob();
    ~VRUIKnob();

    // Werte
    void setValue(float value);
    void setMinValue(float minValue);
    void setMaxValue(float maxValue);
    float getValue() const;

    // Styling
    void setColor(const glm::vec4& color);
    void setHoverColor(const glm::vec4& color);
    void setIndicatorColor(const glm::vec4& color);

    // Rendering
    void render(VRUIRenderer& renderer) override;

private:
    float value;
    float minValue;
    float maxValue;
    glm::vec4 color;
    glm::vec4 hoverColor;
    glm::vec4 indicatorColor;
    bool isHovered;
    bool isDragging;
};

// Toggle-Klasse
class VRUIToggle : public VRUIElement {
public:
    VRUIToggle();
    ~VRUIToggle();

    // Zustand
    void setState(bool state);
    bool getState() const;

    // Styling
    void setOnColor(const glm::vec4& color);
    void setOffColor(const glm::vec4& color);
    void setHoverColor(const glm::vec4& color);

    // Rendering
    void render(VRUIRenderer& renderer) override;

private:
    bool state;
    glm::vec4 onColor;
    glm::vec4 offColor;
    glm::vec4 hoverColor;
    bool isHovered;
};

// Label-Klasse
class VRUILabel : public VRUIElement {
public:
    VRUILabel();
    ~VRUILabel();

    // Text
    void setText(const std::string& text);
    const std::string& getText() const;

    // Styling
    void setColor(const glm::vec4& color);
    void setFontSize(float fontSize);
    void setAlignment(const std::string& alignment);

    // Rendering
    void render(VRUIRenderer& renderer) override;

private:
    std::string text;
    glm::vec4 color;
    float fontSize;
    std::string alignment;
};

// Menu-Klasse
class VRUIMenu : public VRUIElement {
public:
    VRUIMenu();
    ~VRUIMenu();

    // Items
    void addItem(const std::string& text, std::function<void()> callback);
    void removeItem(const std::string& text);
    void clearItems();

    // Styling
    void setBackgroundColor(const glm::vec4& color);
    void setItemColor(const glm::vec4& color);
    void setHoverColor(const glm::vec4& color);

    // Rendering
    void render(VRUIRenderer& renderer) override;

private:
    struct MenuItem {
        std::string text;
        std::function<void()> callback;
    };

    std::vector<MenuItem> items;
    glm::vec4 backgroundColor;
    glm::vec4 itemColor;
    glm::vec4 hoverColor;
    int hoveredItem;
};

// Canvas-Klasse für 2D-Rendering
class VRUICanvas : public VRUIElement {
public:
    VRUICanvas();
    ~VRUICanvas();

    // Größe
    void setSize(const glm::vec2& size);
    const glm::vec2& getSize() const;

    // Rendering
    void render(VRUIRenderer& renderer) override;

private:
    glm::vec2 size;
};

// Renderer-Klasse
class VRUIRenderer {
public:
    VRUIRenderer();
    ~VRUIRenderer();

    // Initialisierung
    bool initialize();
    void shutdown();

    // Rendering
    void beginFrame();
    void endFrame();
    void renderElement(VRUIElement* element);

    // Shader
    void setShader(const std::string& shaderName);
    void setUniform(const std::string& name, const glm::mat4& value);
    void setUniform(const std::string& name, const glm::vec4& value);
    void setUniform(const std::string& name, float value);

private:
    // OpenGL-Zustand
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    unsigned int shaderProgram;
};

} // namespace VR_DAW 