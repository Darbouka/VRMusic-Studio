#pragma once

#include "core/ComponentBase.hpp"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <functional>

namespace VRMusicStudio {
namespace UI {

class UIComponentBase : public Core::ComponentBase {
public:
    virtual ~UIComponentBase() = default;

    // Component identification
    std::string getName() const override = 0;
    std::string getVersion() const override = 0;
    std::string getCategory() const override { return "UI"; }

    // Lifecycle management
    bool initialize() override = 0;
    void shutdown() override = 0;
    void update() override = 0;

    // State management
    bool isEnabled() const override = 0;
    void setEnabled(bool enabled) override = 0;
    bool isInitialized() const override = 0;

    // Error handling
    std::string getLastError() const override = 0;
    void clearError() override = 0;

    // UI specific
    virtual void setPosition(const glm::vec3& position) = 0;
    virtual glm::vec3 getPosition() const = 0;
    virtual void setRotation(const glm::quat& rotation) = 0;
    virtual glm::quat getRotation() const = 0;
    virtual void setScale(const glm::vec3& scale) = 0;
    virtual glm::vec3 getScale() const = 0;
    virtual void setVisible(bool visible) = 0;
    virtual bool isVisible() const = 0;
    virtual void setInteractive(bool interactive) = 0;
    virtual bool isInteractive() const = 0;
    virtual void setHovered(bool hovered) = 0;
    virtual bool isHovered() const = 0;
    virtual void setSelected(bool selected) = 0;
    virtual bool isSelected() const = 0;
    virtual void setFocused(bool focused) = 0;
    virtual bool isFocused() const = 0;

    // Event handling
    virtual void onHover() = 0;
    virtual void onUnhover() = 0;
    virtual void onSelect() = 0;
    virtual void onDeselect() = 0;
    virtual void onFocus() = 0;
    virtual void onUnfocus() = 0;
    virtual void onClick() = 0;
    virtual void onDoubleClick() = 0;
    virtual void onDragStart() = 0;
    virtual void onDrag() = 0;
    virtual void onDragEnd() = 0;
    virtual void onKeyPress(int key) = 0;
    virtual void onKeyRelease(int key) = 0;
    virtual void onChar(unsigned int c) = 0;

    // Callback registration
    virtual void setOnHoverCallback(std::function<void()> callback) = 0;
    virtual void setOnUnhoverCallback(std::function<void()> callback) = 0;
    virtual void setOnSelectCallback(std::function<void()> callback) = 0;
    virtual void setOnDeselectCallback(std::function<void()> callback) = 0;
    virtual void setOnFocusCallback(std::function<void()> callback) = 0;
    virtual void setOnUnfocusCallback(std::function<void()> callback) = 0;
    virtual void setOnClickCallback(std::function<void()> callback) = 0;
    virtual void setOnDoubleClickCallback(std::function<void()> callback) = 0;
    virtual void setOnDragStartCallback(std::function<void()> callback) = 0;
    virtual void setOnDragCallback(std::function<void()> callback) = 0;
    virtual void setOnDragEndCallback(std::function<void()> callback) = 0;
    virtual void setOnKeyPressCallback(std::function<void(int)> callback) = 0;
    virtual void setOnKeyReleaseCallback(std::function<void(int)> callback) = 0;
    virtual void setOnCharCallback(std::function<void(unsigned int)> callback) = 0;

protected:
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    bool visible = true;
    bool interactive = true;
    bool hovered = false;
    bool selected = false;
    bool focused = false;

    std::function<void()> onHoverCallback;
    std::function<void()> onUnhoverCallback;
    std::function<void()> onSelectCallback;
    std::function<void()> onDeselectCallback;
    std::function<void()> onFocusCallback;
    std::function<void()> onUnfocusCallback;
    std::function<void()> onClickCallback;
    std::function<void()> onDoubleClickCallback;
    std::function<void()> onDragStartCallback;
    std::function<void()> onDragCallback;
    std::function<void()> onDragEndCallback;
    std::function<void(int)> onKeyPressCallback;
    std::function<void(int)> onKeyReleaseCallback;
    std::function<void(unsigned int)> onCharCallback;
};

} // namespace UI
} // namespace VRMusicStudio 