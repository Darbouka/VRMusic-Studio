#include "ui/components/Slider.hpp"
#include <stdexcept>

namespace VRMusicStudio {
namespace UI {

bool Slider::initialize() {
    if (isInitialized()) {
        return true;
    }

    try {
        // Initialize slider
        initialized = true;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        return false;
    }
}

void Slider::shutdown() {
    if (!isInitialized()) {
        return;
    }

    try {
        // Shutdown slider
        initialized = false;
    } catch (const std::exception& e) {
        lastError = e.what();
    }
}

void Slider::update() {
    if (!isInitialized() || !isEnabled()) {
        return;
    }

    try {
        // Update slider
    } catch (const std::exception& e) {
        lastError = e.what();
    }
}

// Slider specific
void Slider::setValue(float value) { this->value = value; }
float Slider::getValue() const { return value; }
void Slider::setMinValue(float value) { minValue = value; }
float Slider::getMinValue() const { return minValue; }
void Slider::setMaxValue(float value) { maxValue = value; }
float Slider::getMaxValue() const { return maxValue; }
void Slider::setStep(float value) { step = value; }
float Slider::getStep() const { return step; }
void Slider::setOrientation(bool horizontal) { this->horizontal = horizontal; }
bool Slider::isHorizontal() const { return horizontal; }
void Slider::setShowValue(bool show) { showValue = show; }
bool Slider::isShowValue() const { return showValue; }
void Slider::setValueFormat(const std::string& format) { valueFormat = format; }
std::string Slider::getValueFormat() const { return valueFormat; }
void Slider::setValuePosition(const glm::vec2& position) { valuePosition = position; }
glm::vec2 Slider::getValuePosition() const { return valuePosition; }
void Slider::setValueFont(const std::string& font) { valueFont = font; }
std::string Slider::getValueFont() const { return valueFont; }
void Slider::setValueFontSize(float size) { valueFontSize = size; }
float Slider::getValueFontSize() const { return valueFontSize; }
void Slider::setValueColor(const glm::vec4& color) { valueColor = color; }
glm::vec4 Slider::getValueColor() const { return valueColor; }
void Slider::setTrackColor(const glm::vec4& color) { trackColor = color; }
glm::vec4 Slider::getTrackColor() const { return trackColor; }
void Slider::setTrackHoverColor(const glm::vec4& color) { trackHoverColor = color; }
glm::vec4 Slider::getTrackHoverColor() const { return trackHoverColor; }
void Slider::setTrackPressedColor(const glm::vec4& color) { trackPressedColor = color; }
glm::vec4 Slider::getTrackPressedColor() const { return trackPressedColor; }
void Slider::setTrackBorderColor(const glm::vec4& color) { trackBorderColor = color; }
glm::vec4 Slider::getTrackBorderColor() const { return trackBorderColor; }
void Slider::setTrackBorderWidth(float width) { trackBorderWidth = width; }
float Slider::getTrackBorderWidth() const { return trackBorderWidth; }
void Slider::setTrackCornerRadius(float radius) { trackCornerRadius = radius; }
float Slider::getTrackCornerRadius() const { return trackCornerRadius; }
void Slider::setThumbColor(const glm::vec4& color) { thumbColor = color; }
glm::vec4 Slider::getThumbColor() const { return thumbColor; }
void Slider::setThumbHoverColor(const glm::vec4& color) { thumbHoverColor = color; }
glm::vec4 Slider::getThumbHoverColor() const { return thumbHoverColor; }
void Slider::setThumbPressedColor(const glm::vec4& color) { thumbPressedColor = color; }
glm::vec4 Slider::getThumbPressedColor() const { return thumbPressedColor; }
void Slider::setThumbBorderColor(const glm::vec4& color) { thumbBorderColor = color; }
glm::vec4 Slider::getThumbBorderColor() const { return thumbBorderColor; }
void Slider::setThumbBorderWidth(float width) { thumbBorderWidth = width; }
float Slider::getThumbBorderWidth() const { return thumbBorderWidth; }
void Slider::setThumbCornerRadius(float radius) { thumbCornerRadius = radius; }
float Slider::getThumbCornerRadius() const { return thumbCornerRadius; }
void Slider::setThumbSize(const glm::vec2& size) { thumbSize = size; }
glm::vec2 Slider::getThumbSize() const { return thumbSize; }
void Slider::setThumbIcon(const std::string& icon) { thumbIcon = icon; }
std::string Slider::getThumbIcon() const { return thumbIcon; }
void Slider::setThumbIconSize(const glm::vec2& size) { thumbIconSize = size; }
glm::vec2 Slider::getThumbIconSize() const { return thumbIconSize; }
void Slider::setThumbIconColor(const glm::vec4& color) { thumbIconColor = color; }
glm::vec4 Slider::getThumbIconColor() const { return thumbIconColor; }
void Slider::setThumbIconPosition(const glm::vec2& position) { thumbIconPosition = position; }
glm::vec2 Slider::getThumbIconPosition() const { return thumbIconPosition; }
void Slider::setTooltip(const std::string& tooltip) { this->tooltip = tooltip; }
std::string Slider::getTooltip() const { return tooltip; }
void Slider::setTooltipDelay(float delay) { tooltipDelay = delay; }
float Slider::getTooltipDelay() const { return tooltipDelay; }
void Slider::setTooltipColor(const glm::vec4& color) { tooltipColor = color; }
glm::vec4 Slider::getTooltipColor() const { return tooltipColor; }
void Slider::setTooltipBackgroundColor(const glm::vec4& color) { tooltipBackgroundColor = color; }
glm::vec4 Slider::getTooltipBackgroundColor() const { return tooltipBackgroundColor; }
void Slider::setTooltipBorderColor(const glm::vec4& color) { tooltipBorderColor = color; }
glm::vec4 Slider::getTooltipBorderColor() const { return tooltipBorderColor; }
void Slider::setTooltipBorderWidth(float width) { tooltipBorderWidth = width; }
float Slider::getTooltipBorderWidth() const { return tooltipBorderWidth; }
void Slider::setTooltipCornerRadius(float radius) { tooltipCornerRadius = radius; }
float Slider::getTooltipCornerRadius() const { return tooltipCornerRadius; }
void Slider::setTooltipPadding(const glm::vec2& padding) { tooltipPadding = padding; }
glm::vec2 Slider::getTooltipPadding() const { return tooltipPadding; }
void Slider::setTooltipFont(const std::string& font) { tooltipFont = font; }
std::string Slider::getTooltipFont() const { return tooltipFont; }
void Slider::setTooltipFontSize(float size) { tooltipFontSize = size; }
float Slider::getTooltipFontSize() const { return tooltipFontSize; }
void Slider::setTooltipTextColor(const glm::vec4& color) { tooltipTextColor = color; }
glm::vec4 Slider::getTooltipTextColor() const { return tooltipTextColor; }
void Slider::setTooltipIcon(const std::string& icon) { tooltipIcon = icon; }
std::string Slider::getTooltipIcon() const { return tooltipIcon; }
void Slider::setTooltipIconSize(const glm::vec2& size) { tooltipIconSize = size; }
glm::vec2 Slider::getTooltipIconSize() const { return tooltipIconSize; }
void Slider::setTooltipIconColor(const glm::vec4& color) { tooltipIconColor = color; }
glm::vec4 Slider::getTooltipIconColor() const { return tooltipIconColor; }
void Slider::setTooltipIconPosition(const glm::vec2& position) { tooltipIconPosition = position; }
glm::vec2 Slider::getTooltipIconPosition() const { return tooltipIconPosition; }

// Event handling
void Slider::onHover() {
    if (onHoverCallback) {
        onHoverCallback();
    }
}

void Slider::onUnhover() {
    if (onUnhoverCallback) {
        onUnhoverCallback();
    }
}

void Slider::onSelect() {
    if (onSelectCallback) {
        onSelectCallback();
    }
}

void Slider::onDeselect() {
    if (onDeselectCallback) {
        onDeselectCallback();
    }
}

void Slider::onFocus() {
    if (onFocusCallback) {
        onFocusCallback();
    }
}

void Slider::onUnfocus() {
    if (onUnfocusCallback) {
        onUnfocusCallback();
    }
}

void Slider::onClick() {
    if (onClickCallback) {
        onClickCallback();
    }
}

void Slider::onDoubleClick() {
    if (onDoubleClickCallback) {
        onDoubleClickCallback();
    }
}

void Slider::onDragStart() {
    if (onDragStartCallback) {
        onDragStartCallback();
    }
}

void Slider::onDrag() {
    if (onDragCallback) {
        onDragCallback();
    }
}

void Slider::onDragEnd() {
    if (onDragEndCallback) {
        onDragEndCallback();
    }
}

void Slider::onKeyPress(int key) {
    if (onKeyPressCallback) {
        onKeyPressCallback(key);
    }
}

void Slider::onKeyRelease(int key) {
    if (onKeyReleaseCallback) {
        onKeyReleaseCallback(key);
    }
}

void Slider::onChar(unsigned int c) {
    if (onCharCallback) {
        onCharCallback(c);
    }
}

// Callback registration
void Slider::setOnValueChangeCallback(std::function<void(float)> callback) { onValueChangeCallback = callback; }
void Slider::setOnValueChangeStartCallback(std::function<void(float)> callback) { onValueChangeStartCallback = callback; }
void Slider::setOnValueChangeEndCallback(std::function<void(float)> callback) { onValueChangeEndCallback = callback; }
void Slider::setOnHoverCallback(std::function<void()> callback) { onHoverCallback = callback; }
void Slider::setOnUnhoverCallback(std::function<void()> callback) { onUnhoverCallback = callback; }
void Slider::setOnSelectCallback(std::function<void()> callback) { onSelectCallback = callback; }
void Slider::setOnDeselectCallback(std::function<void()> callback) { onDeselectCallback = callback; }
void Slider::setOnFocusCallback(std::function<void()> callback) { onFocusCallback = callback; }
void Slider::setOnUnfocusCallback(std::function<void()> callback) { onUnfocusCallback = callback; }
void Slider::setOnDragStartCallback(std::function<void()> callback) { onDragStartCallback = callback; }
void Slider::setOnDragCallback(std::function<void()> callback) { onDragCallback = callback; }
void Slider::setOnDragEndCallback(std::function<void()> callback) { onDragEndCallback = callback; }
void Slider::setOnKeyPressCallback(std::function<void(int)> callback) { onKeyPressCallback = callback; }
void Slider::setOnKeyReleaseCallback(std::function<void(int)> callback) { onKeyReleaseCallback = callback; }
void Slider::setOnCharCallback(std::function<void(unsigned int)> callback) { onCharCallback = callback; }

} // namespace UI
} // namespace VRMusicStudio 