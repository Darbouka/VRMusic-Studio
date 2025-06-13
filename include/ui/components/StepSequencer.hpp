#pragma once

#include "ui/UIComponentBase.hpp"
#include <string>
#include <vector>
#include <functional>

namespace VRMusicStudio {
namespace UI {

class StepSequencer : public UIComponentBase {
public:
    StepSequencer() = default;
    ~StepSequencer() override = default;

    // Component identification
    std::string getName() const override { return "StepSequencer"; }
    std::string getVersion() const override { return "1.0.0"; }

    // Lifecycle management
    bool initialize() override;
    void shutdown() override;
    void update() override;

    // StepSequencer specific
    void setSteps(int steps);
    int getSteps() const;
    void setCurrentStep(int step);
    int getCurrentStep() const;
    void setStepValue(int step, float value);
    float getStepValue(int step) const;
    void setStepActive(int step, bool active);
    bool isStepActive(int step) const;
    void setStepColor(int step, const glm::vec4& color);
    glm::vec4 getStepColor(int step) const;
    void setStepHoverColor(int step, const glm::vec4& color);
    glm::vec4 getStepHoverColor(int step) const;
    void setStepPressedColor(int step, const glm::vec4& color);
    glm::vec4 getStepPressedColor(int step) const;
    void setStepBorderColor(int step, const glm::vec4& color);
    glm::vec4 getStepBorderColor(int step) const;
    void setStepBorderWidth(int step, float width);
    float getStepBorderWidth(int step) const;
    void setStepCornerRadius(int step, float radius);
    float getStepCornerRadius(int step) const;
    void setStepPadding(int step, const glm::vec2& padding);
    glm::vec2 getStepPadding(int step) const;
    void setStepIcon(int step, const std::string& icon);
    std::string getStepIcon(int step) const;
    void setStepIconSize(int step, const glm::vec2& size);
    glm::vec2 getStepIconSize(int step) const;
    void setStepIconColor(int step, const glm::vec4& color);
    glm::vec4 getStepIconColor(int step) const;
    void setStepIconPosition(int step, const glm::vec2& position);
    glm::vec2 getStepIconPosition(int step) const;
    void setStepTooltip(int step, const std::string& tooltip);
    std::string getStepTooltip(int step) const;
    void setStepTooltipDelay(int step, float delay);
    float getStepTooltipDelay(int step) const;
    void setStepTooltipColor(int step, const glm::vec4& color);
    glm::vec4 getStepTooltipColor(int step) const;
    void setStepTooltipBackgroundColor(int step, const glm::vec4& color);
    glm::vec4 getStepTooltipBackgroundColor(int step) const;
    void setStepTooltipBorderColor(int step, const glm::vec4& color);
    glm::vec4 getStepTooltipBorderColor(int step) const;
    void setStepTooltipBorderWidth(int step, float width);
    float getStepTooltipBorderWidth(int step) const;
    void setStepTooltipCornerRadius(int step, float radius);
    float getStepTooltipCornerRadius(int step) const;
    void setStepTooltipPadding(int step, const glm::vec2& padding);
    glm::vec2 getStepTooltipPadding(int step) const;
    void setStepTooltipFont(int step, const std::string& font);
    std::string getStepTooltipFont(int step) const;
    void setStepTooltipFontSize(int step, float size);
    float getStepTooltipFontSize(int step) const;
    void setStepTooltipTextColor(int step, const glm::vec4& color);
    glm::vec4 getStepTooltipTextColor(int step) const;
    void setStepTooltipIcon(int step, const std::string& icon);
    std::string getStepTooltipIcon(int step) const;
    void setStepTooltipIconSize(int step, const glm::vec2& size);
    glm::vec2 getStepTooltipIconSize(int step) const;
    void setStepTooltipIconColor(int step, const glm::vec4& color);
    glm::vec4 getStepTooltipIconColor(int step) const;
    void setStepTooltipIconPosition(int step, const glm::vec2& position);
    glm::vec2 getStepTooltipIconPosition(int step) const;

    // Event handling
    void onHover() override;
    void onUnhover() override;
    void onSelect() override;
    void onDeselect() override;
    void onFocus() override;
    void onUnfocus() override;
    void onClick() override;
    void onDoubleClick() override;
    void onDragStart() override;
    void onDrag() override;
    void onDragEnd() override;
    void onKeyPress(int key) override;
    void onKeyRelease(int key) override;
    void onChar(unsigned int c) override;

    // Callback registration
    void setOnStepChangeCallback(std::function<void(int, float)> callback);
    void setOnStepActiveChangeCallback(std::function<void(int, bool)> callback);
    void setOnStepHoverCallback(std::function<void(int)> callback);
    void setOnStepUnhoverCallback(std::function<void(int)> callback);
    void setOnStepSelectCallback(std::function<void(int)> callback);
    void setOnStepDeselectCallback(std::function<void(int)> callback);
    void setOnStepFocusCallback(std::function<void(int)> callback);
    void setOnStepUnfocusCallback(std::function<void(int)> callback);
    void setOnStepClickCallback(std::function<void(int)> callback);
    void setOnStepDoubleClickCallback(std::function<void(int)> callback);
    void setOnStepDragStartCallback(std::function<void(int)> callback);
    void setOnStepDragCallback(std::function<void(int)> callback);
    void setOnStepDragEndCallback(std::function<void(int)> callback);
    void setOnStepKeyPressCallback(std::function<void(int, int)> callback);
    void setOnStepKeyReleaseCallback(std::function<void(int, int)> callback);
    void setOnStepCharCallback(std::function<void(int, unsigned int)> callback);

private:
    int steps = 16;
    int currentStep = 0;
    std::vector<float> stepValues;
    std::vector<bool> stepActive;
    std::vector<glm::vec4> stepColors;
    std::vector<glm::vec4> stepHoverColors;
    std::vector<glm::vec4> stepPressedColors;
    std::vector<glm::vec4> stepBorderColors;
    std::vector<float> stepBorderWidths;
    std::vector<float> stepCornerRadii;
    std::vector<glm::vec2> stepPaddings;
    std::vector<std::string> stepIcons;
    std::vector<glm::vec2> stepIconSizes;
    std::vector<glm::vec4> stepIconColors;
    std::vector<glm::vec2> stepIconPositions;
    std::vector<std::string> stepTooltips;
    std::vector<float> stepTooltipDelays;
    std::vector<glm::vec4> stepTooltipColors;
    std::vector<glm::vec4> stepTooltipBackgroundColors;
    std::vector<glm::vec4> stepTooltipBorderColors;
    std::vector<float> stepTooltipBorderWidths;
    std::vector<float> stepTooltipCornerRadii;
    std::vector<glm::vec2> stepTooltipPaddings;
    std::vector<std::string> stepTooltipFonts;
    std::vector<float> stepTooltipFontSizes;
    std::vector<glm::vec4> stepTooltipTextColors;
    std::vector<std::string> stepTooltipIcons;
    std::vector<glm::vec2> stepTooltipIconSizes;
    std::vector<glm::vec4> stepTooltipIconColors;
    std::vector<glm::vec2> stepTooltipIconPositions;
};

} // namespace UI
} // namespace VRMusicStudio 