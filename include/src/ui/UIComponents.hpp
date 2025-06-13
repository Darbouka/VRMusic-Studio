#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <glm/glm.hpp>
#include <imgui.h>

namespace VRMusicStudio {

// Basis-UI-Komponente
class UIComponent {
public:
    UIComponent(const std::string& label, const glm::vec2& position, const glm::vec2& size)
        : label(label), position(position), size(size), visible(true), interactive(true) {}
    
    virtual ~UIComponent() = default;
    virtual void render() = 0;
    
    void setPosition(const glm::vec2& pos) { position = pos; }
    void setSize(const glm::vec2& s) { size = s; }
    void setVisible(bool v) { visible = v; }
    void setInteractive(bool i) { interactive = i; }
    
    const glm::vec2& getPosition() const { return position; }
    const glm::vec2& getSize() const { return size; }
    bool isVisible() const { return visible; }
    bool isInteractive() const { return interactive; }
    
protected:
    std::string label;
    glm::vec2 position;
    glm::vec2 size;
    bool visible;
    bool interactive;
};

// Slider-Komponente
class UISlider : public UIComponent {
public:
    UISlider(const std::string& label, float value, float min, float max,
             const glm::vec2& position, const glm::vec2& size)
        : UIComponent(label, position, size), value(value), min(min), max(max) {}
    
    void render() override;
    
    void setValue(float v) { value = v; }
    float getValue() const { return value; }
    
    std::function<void(float)> onValueChanged;
    
private:
    float value;
    float min;
    float max;
};

// Toggle-Komponente
class UIToggle : public UIComponent {
public:
    UIToggle(const std::string& label, bool value,
             const glm::vec2& position, const glm::vec2& size)
        : UIComponent(label, position, size), value(value) {}
    
    void render() override;
    
    void setValue(bool v) { value = v; }
    bool getValue() const { return value; }
    
    std::function<void(bool)> onValueChanged;
    
private:
    bool value;
};

// Input-Komponente
class UIInput : public UIComponent {
public:
    UIInput(const std::string& label, char* buffer, size_t bufferSize,
            const glm::vec2& position, const glm::vec2& size)
        : UIComponent(label, position, size), buffer(buffer), bufferSize(bufferSize) {}
    
    void render() override;
    
    std::function<void(const char*)> onValueChanged;
    
private:
    char* buffer;
    size_t bufferSize;
};

// Button-Komponente
class UIButton : public UIComponent {
public:
    UIButton(const std::string& label,
             const glm::vec2& position, const glm::vec2& size)
        : UIComponent(label, position, size) {}
    
    void render() override;
    
    std::function<void()> onClick;
};

// Panel-Komponente
class UIPanel : public UIComponent {
public:
    UIPanel(const std::string& title,
            const glm::vec2& position, const glm::vec2& size,
            bool draggable = true, bool resizable = true)
        : UIComponent(title, position, size), title(title),
          draggable(draggable), resizable(resizable) {}
    
    void render() override;
    
    void addElement(std::shared_ptr<UIComponent> element) {
        elements.push_back(element);
    }
    
private:
    std::string title;
    bool draggable;
    bool resizable;
    std::vector<std::shared_ptr<UIComponent>> elements;
};

// Menu-Komponente
class UIMenu : public UIComponent {
public:
    UIMenu(const std::string& label,
           const std::vector<std::string>& items,
           const glm::vec2& position)
        : UIComponent(label, position, glm::vec2(0)), items(items) {}
    
    void render() override;
    
    std::function<void(const std::string&)> onItemSelected;
    
private:
    std::vector<std::string> items;
};

// Waveform-Komponente
class UIWaveform : public UIComponent {
public:
    UIWaveform(const std::string& label,
               const std::vector<float>& data,
               const glm::vec2& position, const glm::vec2& size,
               bool autoScale = true)
        : UIComponent(label, position, size), data(data), autoScale(autoScale) {}
    
    void render() override;
    
    void setData(const std::vector<float>& newData) { data = newData; }
    
private:
    std::vector<float> data;
    bool autoScale;
};

// Spectrum-Komponente
class UISpectrum : public UIComponent {
public:
    UISpectrum(const std::string& label,
               const std::vector<float>& data,
               const glm::vec2& position, const glm::vec2& size,
               bool autoScale = true)
        : UIComponent(label, position, size), data(data), autoScale(autoScale) {}
    
    void render() override;
    
    void setData(const std::vector<float>& newData) { data = newData; }
    
private:
    std::vector<float> data;
    bool autoScale;
};

// Knob-Komponente
class UIKnob : public UIComponent {
public:
    UIKnob(const std::string& label, float value, float min, float max,
           const glm::vec2& position, float size)
        : UIComponent(label, position, glm::vec2(size)), value(value), min(min), max(max) {}
    
    void render() override;
    
    void setValue(float v) { value = v; }
    float getValue() const { return value; }
    
    std::function<void(float)> onValueChanged;
    
private:
    float value;
    float min;
    float max;
};

// Theme-Manager
class ThemeManager {
public:
    void setTheme(const std::string& theme);
    void applyTheme();
    
    glm::vec4 backgroundColor;
    glm::vec4 textColor;
    glm::vec4 accentColor;
};

// Layout-Manager
class LayoutManager {
public:
    LayoutManager(const glm::vec2& position, const glm::vec2& size,
                 float padding = 10.0f, float spacing = 5.0f)
        : position(position), size(size), padding(padding), spacing(spacing) {}
    
    void setLayout(const std::string& layout);
    void addElement(std::shared_ptr<UIComponent> element) {
        elements.push_back(element);
    }
    
private:
    void arrangeVertical();
    void arrangeHorizontal();
    void arrangeGrid();
    
    glm::vec2 position;
    glm::vec2 size;
    float padding;
    float spacing;
    std::vector<std::shared_ptr<UIComponent>> elements;
};

} // namespace VRMusicStudio 