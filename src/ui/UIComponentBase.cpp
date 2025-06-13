#include "ui/UIComponentBase.hpp"
#include <stdexcept>

namespace VRMusicStudio {
namespace UI {

class UIComponentBaseImpl : public UIComponentBase {
public:
    UIComponentBaseImpl() = default;
    ~UIComponentBaseImpl() override = default;

    // Component identification
    std::string getName() const override { return "UIComponentBase"; }
    std::string getVersion() const override { return "1.0.0"; }

    // Lifecycle management
    bool initialize() override {
        if (isInitialized()) {
            return true;
        }

        try {
            // Initialize component
            initialized = true;
            return true;
        } catch (const std::exception& e) {
            lastError = e.what();
            return false;
        }
    }

    void shutdown() override {
        if (!isInitialized()) {
            return;
        }

        try {
            // Shutdown component
            initialized = false;
        } catch (const std::exception& e) {
            lastError = e.what();
        }
    }

    void update() override {
        if (!isInitialized() || !isEnabled()) {
            return;
        }

        try {
            // Update component
        } catch (const std::exception& e) {
            lastError = e.what();
        }
    }

    // State management
    bool isEnabled() const override { return enabled; }
    void setEnabled(bool value) override { enabled = value; }
    bool isInitialized() const override { return initialized; }

    // Error handling
    std::string getLastError() const override { return lastError; }
    void clearError() override { lastError.clear(); }

    // UI specific
    void setPosition(const glm::vec3& value) override { position = value; }
    glm::vec3 getPosition() const override { return position; }
    void setRotation(const glm::quat& value) override { rotation = value; }
    glm::quat getRotation() const override { return rotation; }
    void setScale(const glm::vec3& value) override { scale = value; }
    glm::vec3 getScale() const override { return scale; }
    void setVisible(bool value) override { visible = value; }
    bool isVisible() const override { return visible; }
    void setInteractive(bool value) override { interactive = value; }
    bool isInteractive() const override { return interactive; }
    void setHovered(bool value) override { hovered = value; }
    bool isHovered() const override { return hovered; }
    void setSelected(bool value) override { selected = value; }
    bool isSelected() const override { return selected; }
    void setFocused(bool value) override { focused = value; }
    bool isFocused() const override { return focused; }

    // Event handling
    void onHover() override {
        if (onHoverCallback) {
            onHoverCallback();
        }
    }

    void onUnhover() override {
        if (onUnhoverCallback) {
            onUnhoverCallback();
        }
    }

    void onSelect() override {
        if (onSelectCallback) {
            onSelectCallback();
        }
    }

    void onDeselect() override {
        if (onDeselectCallback) {
            onDeselectCallback();
        }
    }

    void onFocus() override {
        if (onFocusCallback) {
            onFocusCallback();
        }
    }

    void onUnfocus() override {
        if (onUnfocusCallback) {
            onUnfocusCallback();
        }
    }

    void onClick() override {
        if (onClickCallback) {
            onClickCallback();
        }
    }

    void onDoubleClick() override {
        if (onDoubleClickCallback) {
            onDoubleClickCallback();
        }
    }

    void onDragStart() override {
        if (onDragStartCallback) {
            onDragStartCallback();
        }
    }

    void onDrag() override {
        if (onDragCallback) {
            onDragCallback();
        }
    }

    void onDragEnd() override {
        if (onDragEndCallback) {
            onDragEndCallback();
        }
    }

    void onKeyPress(int key) override {
        if (onKeyPressCallback) {
            onKeyPressCallback(key);
        }
    }

    void onKeyRelease(int key) override {
        if (onKeyReleaseCallback) {
            onKeyReleaseCallback(key);
        }
    }

    void onChar(unsigned int c) override {
        if (onCharCallback) {
            onCharCallback(c);
        }
    }

    // Callback registration
    void setOnHoverCallback(std::function<void()> callback) override { onHoverCallback = callback; }
    void setOnUnhoverCallback(std::function<void()> callback) override { onUnhoverCallback = callback; }
    void setOnSelectCallback(std::function<void()> callback) override { onSelectCallback = callback; }
    void setOnDeselectCallback(std::function<void()> callback) override { onDeselectCallback = callback; }
    void setOnFocusCallback(std::function<void()> callback) override { onFocusCallback = callback; }
    void setOnUnfocusCallback(std::function<void()> callback) override { onUnfocusCallback = callback; }
    void setOnClickCallback(std::function<void()> callback) override { onClickCallback = callback; }
    void setOnDoubleClickCallback(std::function<void()> callback) override { onDoubleClickCallback = callback; }
    void setOnDragStartCallback(std::function<void()> callback) override { onDragStartCallback = callback; }
    void setOnDragCallback(std::function<void()> callback) override { onDragCallback = callback; }
    void setOnDragEndCallback(std::function<void()> callback) override { onDragEndCallback = callback; }
    void setOnKeyPressCallback(std::function<void(int)> callback) override { onKeyPressCallback = callback; }
    void setOnKeyReleaseCallback(std::function<void(int)> callback) override { onKeyReleaseCallback = callback; }
    void setOnCharCallback(std::function<void(unsigned int)> callback) override { onCharCallback = callback; }

private:
    bool enabled = true;
    bool initialized = false;
    std::string lastError;
};

} // namespace UI
} // namespace VRMusicStudio 