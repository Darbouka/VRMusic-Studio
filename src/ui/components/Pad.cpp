#include "ui/components/Pad.hpp"
#include <stdexcept>

namespace VRMusicStudio {
namespace UI {

bool Pad::initialize() {
    if (isInitialized()) {
        return true;
    }

    try {
        // Initialize pad
        initialized = true;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        return false;
    }
}

void Pad::shutdown() {
    if (!isInitialized()) {
        return;
    }

    try {
        // Shutdown pad
        initialized = false;
    } catch (const std::exception& e) {
        lastError = e.what();
    }
}

void Pad::update() {
    if (!isInitialized() || !isEnabled()) {
        return;
    }

    try {
        // Update pad
    } catch (const std::exception& e) {
        lastError = e.what();
    }
}

// Pad specific
void Pad::setText(const std::string& value) { text = value; }
std::string Pad::getText() const { return text; }
void Pad::setFont(const std::string& value) { font = value; }
std::string Pad::getFont() const { return font; }
void Pad::setFontSize(float value) { fontSize = value; }
float Pad::getFontSize() const { return fontSize; }
void Pad::setTextColor(const glm::vec4& value) { textColor = value; }
glm::vec4 Pad::getTextColor() const { return textColor; }
void Pad::setBackgroundColor(const glm::vec4& value) { backgroundColor = value; }
glm::vec4 Pad::getBackgroundColor() const { return backgroundColor; }
void Pad::setHoverColor(const glm::vec4& value) { hoverColor = value; }
glm::vec4 Pad::getHoverColor() const { return hoverColor; }
void Pad::setPressedColor(const glm::vec4& value) { pressedColor = value; }
glm::vec4 Pad::getPressedColor() const { return pressedColor; }
void Pad::setBorderColor(const glm::vec4& value) { borderColor = value; }
glm::vec4 Pad::getBorderColor() const { return borderColor; }
void Pad::setBorderWidth(float value) { borderWidth = value; }
float Pad::getBorderWidth() const { return borderWidth; }
void Pad::setCornerRadius(float value) { cornerRadius = value; }
float Pad::getCornerRadius() const { return cornerRadius; }
void Pad::setPadding(const glm::vec2& value) { padding = value; }
glm::vec2 Pad::getPadding() const { return padding; }
void Pad::setIcon(const std::string& value) { icon = value; }
std::string Pad::getIcon() const { return icon; }
void Pad::setIconSize(const glm::vec2& value) { iconSize = value; }
glm::vec2 Pad::getIconSize() const { return iconSize; }
void Pad::setIconColor(const glm::vec4& value) { iconColor = value; }
glm::vec4 Pad::getIconColor() const { return iconColor; }
void Pad::setIconPosition(const glm::vec2& value) { iconPosition = value; }
glm::vec2 Pad::getIconPosition() const { return iconPosition; }
void Pad::setTooltip(const std::string& value) { tooltip = value; }
std::string Pad::getTooltip() const { return tooltip; }
void Pad::setTooltipDelay(float value) { tooltipDelay = value; }
float Pad::getTooltipDelay() const { return tooltipDelay; }
void Pad::setTooltipColor(const glm::vec4& value) { tooltipColor = value; }
glm::vec4 Pad::getTooltipColor() const { return tooltipColor; }
void Pad::setTooltipBackgroundColor(const glm::vec4& value) { tooltipBackgroundColor = value; }
glm::vec4 Pad::getTooltipBackgroundColor() const { return tooltipBackgroundColor; }
void Pad::setTooltipBorderColor(const glm::vec4& value) { tooltipBorderColor = value; }
glm::vec4 Pad::getTooltipBorderColor() const { return tooltipBorderColor; }
void Pad::setTooltipBorderWidth(float value) { tooltipBorderWidth = value; }
float Pad::getTooltipBorderWidth() const { return tooltipBorderWidth; }
void Pad::setTooltipCornerRadius(float value) { tooltipCornerRadius = value; }
float Pad::getTooltipCornerRadius() const { return tooltipCornerRadius; }
void Pad::setTooltipPadding(const glm::vec2& value) { tooltipPadding = value; }
glm::vec2 Pad::getTooltipPadding() const { return tooltipPadding; }
void Pad::setTooltipFont(const std::string& value) { tooltipFont = value; }
std::string Pad::getTooltipFont() const { return tooltipFont; }
void Pad::setTooltipFontSize(float value) { tooltipFontSize = value; }
float Pad::getTooltipFontSize() const { return tooltipFontSize; }
void Pad::setTooltipTextColor(const glm::vec4& value) { tooltipTextColor = value; }
glm::vec4 Pad::getTooltipTextColor() const { return tooltipTextColor; }
void Pad::setTooltipIcon(const std::string& value) { tooltipIcon = value; }
std::string Pad::getTooltipIcon() const { return tooltipIcon; }
void Pad::setTooltipIconSize(const glm::vec2& value) { tooltipIconSize = value; }
glm::vec2 Pad::getTooltipIconSize() const { return tooltipIconSize; }
void Pad::setTooltipIconColor(const glm::vec4& value) { tooltipIconColor = value; }
glm::vec4 Pad::getTooltipIconColor() const { return tooltipIconColor; }
void Pad::setTooltipIconPosition(const glm::vec2& value) { tooltipIconPosition = value; }
glm::vec2 Pad::getTooltipIconPosition() const { return tooltipIconPosition; }

// Event handling
void Pad::onHover() {
    if (onHoverCallback) {
        onHoverCallback();
    }
}

void Pad::onUnhover() {
    if (onUnhoverCallback) {
        onUnhoverCallback();
    }
}

void Pad::onSelect() {
    if (onSelectCallback) {
        onSelectCallback();
    }
}

void Pad::onDeselect() {
    if (onDeselectCallback) {
        onDeselectCallback();
    }
}

void Pad::onFocus() {
    if (onFocusCallback) {
        onFocusCallback();
    }
}

void Pad::onUnfocus() {
    if (onUnfocusCallback) {
        onUnfocusCallback();
    }
}

void Pad::onClick() {
    if (onClickCallback) {
        onClickCallback();
    }
}

void Pad::onDoubleClick() {
    if (onDoubleClickCallback) {
        onDoubleClickCallback();
    }
}

void Pad::onDragStart() {
    if (onDragStartCallback) {
        onDragStartCallback();
    }
}

void Pad::onDrag() {
    if (onDragCallback) {
        onDragCallback();
    }
}

void Pad::onDragEnd() {
    if (onDragEndCallback) {
        onDragEndCallback();
    }
}

void Pad::onKeyPress(int key) {
    if (onKeyPressCallback) {
        onKeyPressCallback(key);
    }
}

void Pad::onKeyRelease(int key) {
    if (onKeyReleaseCallback) {
        onKeyReleaseCallback(key);
    }
}

void Pad::onChar(unsigned int c) {
    if (onCharCallback) {
        onCharCallback(c);
    }
}

// Callback registration
void Pad::setOnClickCallback(std::function<void()> callback) { onClickCallback = callback; }
void Pad::setOnDoubleClickCallback(std::function<void()> callback) { onDoubleClickCallback = callback; }
void Pad::setOnHoverCallback(std::function<void()> callback) { onHoverCallback = callback; }
void Pad::setOnUnhoverCallback(std::function<void()> callback) { onUnhoverCallback = callback; }
void Pad::setOnSelectCallback(std::function<void()> callback) { onSelectCallback = callback; }
void Pad::setOnDeselectCallback(std::function<void()> callback) { onDeselectCallback = callback; }
void Pad::setOnFocusCallback(std::function<void()> callback) { onFocusCallback = callback; }
void Pad::setOnUnfocusCallback(std::function<void()> callback) { onUnfocusCallback = callback; }
void Pad::setOnDragStartCallback(std::function<void()> callback) { onDragStartCallback = callback; }
void Pad::setOnDragCallback(std::function<void()> callback) { onDragCallback = callback; }
void Pad::setOnDragEndCallback(std::function<void()> callback) { onDragEndCallback = callback; }
void Pad::setOnKeyPressCallback(std::function<void(int)> callback) { onKeyPressCallback = callback; }
void Pad::setOnKeyReleaseCallback(std::function<void(int)> callback) { onKeyReleaseCallback = callback; }
void Pad::setOnCharCallback(std::function<void(unsigned int)> callback) { onCharCallback = callback; }

} // namespace UI
} // namespace VRMusicStudio 