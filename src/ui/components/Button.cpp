#include "ui/components/Button.hpp"
#include <stdexcept>

namespace VRMusicStudio {
namespace UI {

bool Button::initialize() {
    if (isInitialized()) {
        return true;
    }

    try {
        // Initialize button
        initialized = true;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        return false;
    }
}

void Button::shutdown() {
    if (!isInitialized()) {
        return;
    }

    try {
        // Shutdown button
        initialized = false;
    } catch (const std::exception& e) {
        lastError = e.what();
    }
}

void Button::update() {
    if (!isInitialized() || !isEnabled()) {
        return;
    }

    try {
        // Update button
    } catch (const std::exception& e) {
        lastError = e.what();
    }
}

// Button specific
void Button::setText(const std::string& value) { text = value; }
std::string Button::getText() const { return text; }
void Button::setFont(const std::string& value) { font = value; }
std::string Button::getFont() const { return font; }
void Button::setFontSize(float value) { fontSize = value; }
float Button::getFontSize() const { return fontSize; }
void Button::setTextColor(const glm::vec4& value) { textColor = value; }
glm::vec4 Button::getTextColor() const { return textColor; }
void Button::setBackgroundColor(const glm::vec4& value) { backgroundColor = value; }
glm::vec4 Button::getBackgroundColor() const { return backgroundColor; }
void Button::setHoverColor(const glm::vec4& value) { hoverColor = value; }
glm::vec4 Button::getHoverColor() const { return hoverColor; }
void Button::setPressedColor(const glm::vec4& value) { pressedColor = value; }
glm::vec4 Button::getPressedColor() const { return pressedColor; }
void Button::setBorderColor(const glm::vec4& value) { borderColor = value; }
glm::vec4 Button::getBorderColor() const { return borderColor; }
void Button::setBorderWidth(float value) { borderWidth = value; }
float Button::getBorderWidth() const { return borderWidth; }
void Button::setCornerRadius(float value) { cornerRadius = value; }
float Button::getCornerRadius() const { return cornerRadius; }
void Button::setPadding(const glm::vec2& value) { padding = value; }
glm::vec2 Button::getPadding() const { return padding; }
void Button::setIcon(const std::string& value) { icon = value; }
std::string Button::getIcon() const { return icon; }
void Button::setIconSize(const glm::vec2& value) { iconSize = value; }
glm::vec2 Button::getIconSize() const { return iconSize; }
void Button::setIconColor(const glm::vec4& value) { iconColor = value; }
glm::vec4 Button::getIconColor() const { return iconColor; }
void Button::setIconPosition(const glm::vec2& value) { iconPosition = value; }
glm::vec2 Button::getIconPosition() const { return iconPosition; }
void Button::setTooltip(const std::string& value) { tooltip = value; }
std::string Button::getTooltip() const { return tooltip; }
void Button::setTooltipDelay(float value) { tooltipDelay = value; }
float Button::getTooltipDelay() const { return tooltipDelay; }
void Button::setTooltipColor(const glm::vec4& value) { tooltipColor = value; }
glm::vec4 Button::getTooltipColor() const { return tooltipColor; }
void Button::setTooltipBackgroundColor(const glm::vec4& value) { tooltipBackgroundColor = value; }
glm::vec4 Button::getTooltipBackgroundColor() const { return tooltipBackgroundColor; }
void Button::setTooltipBorderColor(const glm::vec4& value) { tooltipBorderColor = value; }
glm::vec4 Button::getTooltipBorderColor() const { return tooltipBorderColor; }
void Button::setTooltipBorderWidth(float value) { tooltipBorderWidth = value; }
float Button::getTooltipBorderWidth() const { return tooltipBorderWidth; }
void Button::setTooltipCornerRadius(float value) { tooltipCornerRadius = value; }
float Button::getTooltipCornerRadius() const { return tooltipCornerRadius; }
void Button::setTooltipPadding(const glm::vec2& value) { tooltipPadding = value; }
glm::vec2 Button::getTooltipPadding() const { return tooltipPadding; }
void Button::setTooltipFont(const std::string& value) { tooltipFont = value; }
std::string Button::getTooltipFont() const { return tooltipFont; }
void Button::setTooltipFontSize(float value) { tooltipFontSize = value; }
float Button::getTooltipFontSize() const { return tooltipFontSize; }
void Button::setTooltipTextColor(const glm::vec4& value) { tooltipTextColor = value; }
glm::vec4 Button::getTooltipTextColor() const { return tooltipTextColor; }
void Button::setTooltipIcon(const std::string& value) { tooltipIcon = value; }
std::string Button::getTooltipIcon() const { return tooltipIcon; }
void Button::setTooltipIconSize(const glm::vec2& value) { tooltipIconSize = value; }
glm::vec2 Button::getTooltipIconSize() const { return tooltipIconSize; }
void Button::setTooltipIconColor(const glm::vec4& value) { tooltipIconColor = value; }
glm::vec4 Button::getTooltipIconColor() const { return tooltipIconColor; }
void Button::setTooltipIconPosition(const glm::vec2& value) { tooltipIconPosition = value; }
glm::vec2 Button::getTooltipIconPosition() const { return tooltipIconPosition; }

// Event handling
void Button::onHover() {
    if (onHoverCallback) {
        onHoverCallback();
    }
}

void Button::onUnhover() {
    if (onUnhoverCallback) {
        onUnhoverCallback();
    }
}

void Button::onSelect() {
    if (onSelectCallback) {
        onSelectCallback();
    }
}

void Button::onDeselect() {
    if (onDeselectCallback) {
        onDeselectCallback();
    }
}

void Button::onFocus() {
    if (onFocusCallback) {
        onFocusCallback();
    }
}

void Button::onUnfocus() {
    if (onUnfocusCallback) {
        onUnfocusCallback();
    }
}

void Button::onClick() {
    if (onClickCallback) {
        onClickCallback();
    }
}

void Button::onDoubleClick() {
    if (onDoubleClickCallback) {
        onDoubleClickCallback();
    }
}

void Button::onDragStart() {
    if (onDragStartCallback) {
        onDragStartCallback();
    }
}

void Button::onDrag() {
    if (onDragCallback) {
        onDragCallback();
    }
}

void Button::onDragEnd() {
    if (onDragEndCallback) {
        onDragEndCallback();
    }
}

void Button::onKeyPress(int key) {
    if (onKeyPressCallback) {
        onKeyPressCallback(key);
    }
}

void Button::onKeyRelease(int key) {
    if (onKeyReleaseCallback) {
        onKeyReleaseCallback(key);
    }
}

void Button::onChar(unsigned int c) {
    if (onCharCallback) {
        onCharCallback(c);
    }
}

// Callback registration
void Button::setOnClickCallback(std::function<void()> callback) { onClickCallback = callback; }
void Button::setOnDoubleClickCallback(std::function<void()> callback) { onDoubleClickCallback = callback; }
void Button::setOnHoverCallback(std::function<void()> callback) { onHoverCallback = callback; }
void Button::setOnUnhoverCallback(std::function<void()> callback) { onUnhoverCallback = callback; }
void Button::setOnSelectCallback(std::function<void()> callback) { onSelectCallback = callback; }
void Button::setOnDeselectCallback(std::function<void()> callback) { onDeselectCallback = callback; }
void Button::setOnFocusCallback(std::function<void()> callback) { onFocusCallback = callback; }
void Button::setOnUnfocusCallback(std::function<void()> callback) { onUnfocusCallback = callback; }
void Button::setOnDragStartCallback(std::function<void()> callback) { onDragStartCallback = callback; }
void Button::setOnDragCallback(std::function<void()> callback) { onDragCallback = callback; }
void Button::setOnDragEndCallback(std::function<void()> callback) { onDragEndCallback = callback; }
void Button::setOnKeyPressCallback(std::function<void(int)> callback) { onKeyPressCallback = callback; }
void Button::setOnKeyReleaseCallback(std::function<void(int)> callback) { onKeyReleaseCallback = callback; }
void Button::setOnCharCallback(std::function<void(unsigned int)> callback) { onCharCallback = callback; }

} // namespace UI
} // namespace VRMusicStudio 