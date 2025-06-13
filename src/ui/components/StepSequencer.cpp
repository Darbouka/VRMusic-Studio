#include "ui/components/StepSequencer.hpp"
#include <stdexcept>

namespace VRMusicStudio {
namespace UI {

bool StepSequencer::initialize() {
    if (isInitialized()) {
        return true;
    }

    try {
        // Initialize step sequencer
        stepValues.resize(steps, 0.0f);
        stepActive.resize(steps, false);
        stepColors.resize(steps, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
        stepHoverColors.resize(steps, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
        stepPressedColors.resize(steps, glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
        stepBorderColors.resize(steps, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
        stepBorderWidths.resize(steps, 1.0f);
        stepCornerRadii.resize(steps, 4.0f);
        stepPaddings.resize(steps, glm::vec2(8.0f));
        stepIcons.resize(steps);
        stepIconSizes.resize(steps, glm::vec2(16.0f));
        stepIconColors.resize(steps, glm::vec4(1.0f));
        stepIconPositions.resize(steps, glm::vec2(0.0f));
        stepTooltips.resize(steps);
        stepTooltipDelays.resize(steps, 0.5f);
        stepTooltipColors.resize(steps, glm::vec4(1.0f));
        stepTooltipBackgroundColors.resize(steps, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
        stepTooltipBorderColors.resize(steps, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
        stepTooltipBorderWidths.resize(steps, 1.0f);
        stepTooltipCornerRadii.resize(steps, 4.0f);
        stepTooltipPaddings.resize(steps, glm::vec2(8.0f));
        stepTooltipFonts.resize(steps);
        stepTooltipFontSizes.resize(steps, 14.0f);
        stepTooltipTextColors.resize(steps, glm::vec4(1.0f));
        stepTooltipIcons.resize(steps);
        stepTooltipIconSizes.resize(steps, glm::vec2(14.0f));
        stepTooltipIconColors.resize(steps, glm::vec4(1.0f));
        stepTooltipIconPositions.resize(steps, glm::vec2(0.0f));

        initialized = true;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        return false;
    }
}

void StepSequencer::shutdown() {
    if (!isInitialized()) {
        return;
    }

    try {
        // Shutdown step sequencer
        stepValues.clear();
        stepActive.clear();
        stepColors.clear();
        stepHoverColors.clear();
        stepPressedColors.clear();
        stepBorderColors.clear();
        stepBorderWidths.clear();
        stepCornerRadii.clear();
        stepPaddings.clear();
        stepIcons.clear();
        stepIconSizes.clear();
        stepIconColors.clear();
        stepIconPositions.clear();
        stepTooltips.clear();
        stepTooltipDelays.clear();
        stepTooltipColors.clear();
        stepTooltipBackgroundColors.clear();
        stepTooltipBorderColors.clear();
        stepTooltipBorderWidths.clear();
        stepTooltipCornerRadii.clear();
        stepTooltipPaddings.clear();
        stepTooltipFonts.clear();
        stepTooltipFontSizes.clear();
        stepTooltipTextColors.clear();
        stepTooltipIcons.clear();
        stepTooltipIconSizes.clear();
        stepTooltipIconColors.clear();
        stepTooltipIconPositions.clear();

        initialized = false;
    } catch (const std::exception& e) {
        lastError = e.what();
    }
}

void StepSequencer::update() {
    if (!isInitialized() || !isEnabled()) {
        return;
    }

    try {
        // Update step sequencer
    } catch (const std::exception& e) {
        lastError = e.what();
    }
}

// StepSequencer specific
void StepSequencer::setSteps(int value) { steps = value; }
int StepSequencer::getSteps() const { return steps; }
void StepSequencer::setCurrentStep(int value) { currentStep = value; }
int StepSequencer::getCurrentStep() const { return currentStep; }
void StepSequencer::setStepValue(int step, float value) { stepValues[step] = value; }
float StepSequencer::getStepValue(int step) const { return stepValues[step]; }
void StepSequencer::setStepActive(int step, bool value) { stepActive[step] = value; }
bool StepSequencer::isStepActive(int step) const { return stepActive[step]; }
void StepSequencer::setStepColor(int step, const glm::vec4& value) { stepColors[step] = value; }
glm::vec4 StepSequencer::getStepColor(int step) const { return stepColors[step]; }
void StepSequencer::setStepHoverColor(int step, const glm::vec4& value) { stepHoverColors[step] = value; }
glm::vec4 StepSequencer::getStepHoverColor(int step) const { return stepHoverColors[step]; }
void StepSequencer::setStepPressedColor(int step, const glm::vec4& value) { stepPressedColors[step] = value; }
glm::vec4 StepSequencer::getStepPressedColor(int step) const { return stepPressedColors[step]; }
void StepSequencer::setStepBorderColor(int step, const glm::vec4& value) { stepBorderColors[step] = value; }
glm::vec4 StepSequencer::getStepBorderColor(int step) const { return stepBorderColors[step]; }
void StepSequencer::setStepBorderWidth(int step, float value) { stepBorderWidths[step] = value; }
float StepSequencer::getStepBorderWidth(int step) const { return stepBorderWidths[step]; }
void StepSequencer::setStepCornerRadius(int step, float value) { stepCornerRadii[step] = value; }
float StepSequencer::getStepCornerRadius(int step) const { return stepCornerRadii[step]; }
void StepSequencer::setStepPadding(int step, const glm::vec2& value) { stepPaddings[step] = value; }
glm::vec2 StepSequencer::getStepPadding(int step) const { return stepPaddings[step]; }
void StepSequencer::setStepIcon(int step, const std::string& value) { stepIcons[step] = value; }
std::string StepSequencer::getStepIcon(int step) const { return stepIcons[step]; }
void StepSequencer::setStepIconSize(int step, const glm::vec2& value) { stepIconSizes[step] = value; }
glm::vec2 StepSequencer::getStepIconSize(int step) const { return stepIconSizes[step]; }
void StepSequencer::setStepIconColor(int step, const glm::vec4& value) { stepIconColors[step] = value; }
glm::vec4 StepSequencer::getStepIconColor(int step) const { return stepIconColors[step]; }
void StepSequencer::setStepIconPosition(int step, const glm::vec2& value) { stepIconPositions[step] = value; }
glm::vec2 StepSequencer::getStepIconPosition(int step) const { return stepIconPositions[step]; }
void StepSequencer::setStepTooltip(int step, const std::string& value) { stepTooltips[step] = value; }
std::string StepSequencer::getStepTooltip(int step) const { return stepTooltips[step]; }
void StepSequencer::setStepTooltipDelay(int step, float value) { stepTooltipDelays[step] = value; }
float StepSequencer::getStepTooltipDelay(int step) const { return stepTooltipDelays[step]; }
void StepSequencer::setStepTooltipColor(int step, const glm::vec4& value) { stepTooltipColors[step] = value; }
glm::vec4 StepSequencer::getStepTooltipColor(int step) const { return stepTooltipColors[step]; }
void StepSequencer::setStepTooltipBackgroundColor(int step, const glm::vec4& value) { stepTooltipBackgroundColors[step] = value; }
glm::vec4 StepSequencer::getStepTooltipBackgroundColor(int step) const { return stepTooltipBackgroundColors[step]; }
void StepSequencer::setStepTooltipBorderColor(int step, const glm::vec4& value) { stepTooltipBorderColors[step] = value; }
glm::vec4 StepSequencer::getStepTooltipBorderColor(int step) const { return stepTooltipBorderColors[step]; }
void StepSequencer::setStepTooltipBorderWidth(int step, float value) { stepTooltipBorderWidths[step] = value; }
float StepSequencer::getStepTooltipBorderWidth(int step) const { return stepTooltipBorderWidths[step]; }
void StepSequencer::setStepTooltipCornerRadius(int step, float value) { stepTooltipCornerRadii[step] = value; }
float StepSequencer::getStepTooltipCornerRadius(int step) const { return stepTooltipCornerRadii[step]; }
void StepSequencer::setStepTooltipPadding(int step, const glm::vec2& value) { stepTooltipPaddings[step] = value; }
glm::vec2 StepSequencer::getStepTooltipPadding(int step) const { return stepTooltipPaddings[step]; }
void StepSequencer::setStepTooltipFont(int step, const std::string& value) { stepTooltipFonts[step] = value; }
std::string StepSequencer::getStepTooltipFont(int step) const { return stepTooltipFonts[step]; }
void StepSequencer::setStepTooltipFontSize(int step, float value) { stepTooltipFontSizes[step] = value; }
float StepSequencer::getStepTooltipFontSize(int step) const { return stepTooltipFontSizes[step]; }
void StepSequencer::setStepTooltipTextColor(int step, const glm::vec4& value) { stepTooltipTextColors[step] = value; }
glm::vec4 StepSequencer::getStepTooltipTextColor(int step) const { return stepTooltipTextColors[step]; }
void StepSequencer::setStepTooltipIcon(int step, const std::string& value) { stepTooltipIcons[step] = value; }
std::string StepSequencer::getStepTooltipIcon(int step) const { return stepTooltipIcons[step]; }
void StepSequencer::setStepTooltipIconSize(int step, const glm::vec2& value) { stepTooltipIconSizes[step] = value; }
glm::vec2 StepSequencer::getStepTooltipIconSize(int step) const { return stepTooltipIconSizes[step]; }
void StepSequencer::setStepTooltipIconColor(int step, const glm::vec4& value) { stepTooltipIconColors[step] = value; }
glm::vec4 StepSequencer::getStepTooltipIconColor(int step) const { return stepTooltipIconColors[step]; }
void StepSequencer::setStepTooltipIconPosition(int step, const glm::vec2& value) { stepTooltipIconPositions[step] = value; }
glm::vec2 StepSequencer::getStepTooltipIconPosition(int step) const { return stepTooltipIconPositions[step]; }

// Event handling
void StepSequencer::onHover() {
    if (onHoverCallback) {
        onHoverCallback();
    }
}

void StepSequencer::onUnhover() {
    if (onUnhoverCallback) {
        onUnhoverCallback();
    }
}

void StepSequencer::onSelect() {
    if (onSelectCallback) {
        onSelectCallback();
    }
}

void StepSequencer::onDeselect() {
    if (onDeselectCallback) {
        onDeselectCallback();
    }
}

void StepSequencer::onFocus() {
    if (onFocusCallback) {
        onFocusCallback();
    }
}

void StepSequencer::onUnfocus() {
    if (onUnfocusCallback) {
        onUnfocusCallback();
    }
}

void StepSequencer::onClick() {
    if (onClickCallback) {
        onClickCallback();
    }
}

void StepSequencer::onDoubleClick() {
    if (onDoubleClickCallback) {
        onDoubleClickCallback();
    }
}

void StepSequencer::onDragStart() {
    if (onDragStartCallback) {
        onDragStartCallback();
    }
}

void StepSequencer::onDrag() {
    if (onDragCallback) {
        onDragCallback();
    }
}

void StepSequencer::onDragEnd() {
    if (onDragEndCallback) {
        onDragEndCallback();
    }
}

void StepSequencer::onKeyPress(int key) {
    if (onKeyPressCallback) {
        onKeyPressCallback(key);
    }
}

void StepSequencer::onKeyRelease(int key) {
    if (onKeyReleaseCallback) {
        onKeyReleaseCallback(key);
    }
}

void StepSequencer::onChar(unsigned int c) {
    if (onCharCallback) {
        onCharCallback(c);
    }
}

// Callback registration
void StepSequencer::setOnStepChangeCallback(std::function<void(int, float)> callback) { onStepChangeCallback = callback; }
void StepSequencer::setOnStepActiveChangeCallback(std::function<void(int, bool)> callback) { onStepActiveChangeCallback = callback; }
void StepSequencer::setOnStepHoverCallback(std::function<void(int)> callback) { onStepHoverCallback = callback; }
void StepSequencer::setOnStepUnhoverCallback(std::function<void(int)> callback) { onStepUnhoverCallback = callback; }
void StepSequencer::setOnStepSelectCallback(std::function<void(int)> callback) { onStepSelectCallback = callback; }
void StepSequencer::setOnStepDeselectCallback(std::function<void(int)> callback) { onStepDeselectCallback = callback; }
void StepSequencer::setOnStepFocusCallback(std::function<void(int)> callback) { onStepFocusCallback = callback; }
void StepSequencer::setOnStepUnfocusCallback(std::function<void(int)> callback) { onStepUnfocusCallback = callback; }
void StepSequencer::setOnStepClickCallback(std::function<void(int)> callback) { onStepClickCallback = callback; }
void StepSequencer::setOnStepDoubleClickCallback(std::function<void(int)> callback) { onStepDoubleClickCallback = callback; }
void StepSequencer::setOnStepDragStartCallback(std::function<void(int)> callback) { onStepDragStartCallback = callback; }
void StepSequencer::setOnStepDragCallback(std::function<void(int)> callback) { onStepDragCallback = callback; }
void StepSequencer::setOnStepDragEndCallback(std::function<void(int)> callback) { onStepDragEndCallback = callback; }
void StepSequencer::setOnStepKeyPressCallback(std::function<void(int, int)> callback) { onStepKeyPressCallback = callback; }
void StepSequencer::setOnStepKeyReleaseCallback(std::function<void(int, int)> callback) { onStepKeyReleaseCallback = callback; }
void StepSequencer::setOnStepCharCallback(std::function<void(int, unsigned int)> callback) { onStepCharCallback = callback; }

} // namespace UI
} // namespace VRMusicStudio 