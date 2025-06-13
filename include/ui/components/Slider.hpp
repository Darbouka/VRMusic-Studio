#pragma once

#include "ui/UIComponentBase.hpp"
#include <string>
#include <functional>

namespace VRMusicStudio {
namespace UI {

class Slider : public UIComponentBase {
public:
    Slider() = default;
    ~Slider() override = default;

    // Component identification
    std::string getName() const override { return "Slider"; }
    std::string getVersion() const override { return "1.0.0"; }

    // Lifecycle management
    bool initialize() override;
    void shutdown() override;
    void update() override;

    // Slider specific
    void setValue(float value);
    float getValue() const;
    void setMinValue(float value);
    float getMinValue() const;
    void setMaxValue(float value);
    float getMaxValue() const;
    void setStep(float value);
    float getStep() const;
    void setOrientation(bool horizontal);
    bool isHorizontal() const;
    void setShowValue(bool show);
    bool isShowValue() const;
    void setValueFormat(const std::string& format);
    std::string getValueFormat() const;
    void setValuePosition(const glm::vec2& position);
    glm::vec2 getValuePosition() const;
    void setValueFont(const std::string& font);
    std::string getValueFont() const;
    void setValueFontSize(float size);
    float getValueFontSize() const;
    void setValueColor(const glm::vec4& color);
    glm::vec4 getValueColor() const;
    void setTrackColor(const glm::vec4& color);
    glm::vec4 getTrackColor() const;
    void setTrackHoverColor(const glm::vec4& color);
    glm::vec4 getTrackHoverColor() const;
    void setTrackPressedColor(const glm::vec4& color);
    glm::vec4 getTrackPressedColor() const;
    void setTrackBorderColor(const glm::vec4& color);
    glm::vec4 getTrackBorderColor() const;
    void setTrackBorderWidth(float width);
    float getTrackBorderWidth() const;
    void setTrackCornerRadius(float radius);
    float getTrackCornerRadius() const;
    void setThumbColor(const glm::vec4& color);
    glm::vec4 getThumbColor() const;
    void setThumbHoverColor(const glm::vec4& color);
    glm::vec4 getThumbHoverColor() const;
    void setThumbPressedColor(const glm::vec4& color);
    glm::vec4 getThumbPressedColor() const;
    void setThumbBorderColor(const glm::vec4& color);
    glm::vec4 getThumbBorderColor() const;
    void setThumbBorderWidth(float width);
    float getThumbBorderWidth() const;
    void setThumbCornerRadius(float radius);
    float getThumbCornerRadius() const;
    void setThumbSize(const glm::vec2& size);
    glm::vec2 getThumbSize() const;
    void setThumbIcon(const std::string& icon);
    std::string getThumbIcon() const;
    void setThumbIconSize(const glm::vec2& size);
    glm::vec2 getThumbIconSize() const;
    void setThumbIconColor(const glm::vec4& color);
    glm::vec4 getThumbIconColor() const;
    void setThumbIconPosition(const glm::vec2& position);
    glm::vec2 getThumbIconPosition() const;
    void setTooltip(const std::string& tooltip);
    std::string getTooltip() const;
    void setTooltipDelay(float delay);
    float getTooltipDelay() const;
    void setTooltipColor(const glm::vec4& color);
    glm::vec4 getTooltipColor() const;
    void setTooltipBackgroundColor(const glm::vec4& color);
    glm::vec4 getTooltipBackgroundColor() const;
    void setTooltipBorderColor(const glm::vec4& color);
    glm::vec4 getTooltipBorderColor() const;
    void setTooltipBorderWidth(float width);
    float getTooltipBorderWidth() const;
    void setTooltipCornerRadius(float radius);
    float getTooltipCornerRadius() const;
    void setTooltipPadding(const glm::vec2& padding);
    glm::vec2 getTooltipPadding() const;
    void setTooltipFont(const std::string& font);
    std::string getTooltipFont() const;
    void setTooltipFontSize(float size);
    float getTooltipFontSize() const;
    void setTooltipTextColor(const glm::vec4& color);
    glm::vec4 getTooltipTextColor() const;
    void setTooltipIcon(const std::string& icon);
    std::string getTooltipIcon() const;
    void setTooltipIconSize(const glm::vec2& size);
    glm::vec2 getTooltipIconSize() const;
    void setTooltipIconColor(const glm::vec4& color);
    glm::vec4 getTooltipIconColor() const;
    void setTooltipIconPosition(const glm::vec2& position);
    glm::vec2 getTooltipIconPosition() const;

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
    void setOnValueChangeCallback(std::function<void(float)> callback);
    void setOnValueChangeStartCallback(std::function<void(float)> callback);
    void setOnValueChangeEndCallback(std::function<void(float)> callback);
    void setOnHoverCallback(std::function<void()> callback);
    void setOnUnhoverCallback(std::function<void()> callback);
    void setOnSelectCallback(std::function<void()> callback);
    void setOnDeselectCallback(std::function<void()> callback);
    void setOnFocusCallback(std::function<void()> callback);
    void setOnUnfocusCallback(std::function<void()> callback);
    void setOnDragStartCallback(std::function<void()> callback);
    void setOnDragCallback(std::function<void()> callback);
    void setOnDragEndCallback(std::function<void()> callback);
    void setOnKeyPressCallback(std::function<void(int)> callback);
    void setOnKeyReleaseCallback(std::function<void(int)> callback);
    void setOnCharCallback(std::function<void(unsigned int)> callback);

private:
    float value = 0.0f;
    float minValue = 0.0f;
    float maxValue = 1.0f;
    float step = 0.01f;
    bool horizontal = true;
    bool showValue = true;
    std::string valueFormat = "%.2f";
    glm::vec2 valuePosition = glm::vec2(0.0f);
    std::string valueFont;
    float valueFontSize = 14.0f;
    glm::vec4 valueColor = glm::vec4(1.0f);
    glm::vec4 trackColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glm::vec4 trackHoverColor = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
    glm::vec4 trackPressedColor = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    glm::vec4 trackBorderColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    float trackBorderWidth = 1.0f;
    float trackCornerRadius = 4.0f;
    glm::vec4 thumbColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    glm::vec4 thumbHoverColor = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    glm::vec4 thumbPressedColor = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
    glm::vec4 thumbBorderColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    float thumbBorderWidth = 1.0f;
    float thumbCornerRadius = 4.0f;
    glm::vec2 thumbSize = glm::vec2(16.0f);
    std::string thumbIcon;
    glm::vec2 thumbIconSize = glm::vec2(14.0f);
    glm::vec4 thumbIconColor = glm::vec4(1.0f);
    glm::vec2 thumbIconPosition = glm::vec2(0.0f);
    std::string tooltip;
    float tooltipDelay = 0.5f;
    glm::vec4 tooltipColor = glm::vec4(1.0f);
    glm::vec4 tooltipBackgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glm::vec4 tooltipBorderColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    float tooltipBorderWidth = 1.0f;
    float tooltipCornerRadius = 4.0f;
    glm::vec2 tooltipPadding = glm::vec2(8.0f);
    std::string tooltipFont;
    float tooltipFontSize = 14.0f;
    glm::vec4 tooltipTextColor = glm::vec4(1.0f);
    std::string tooltipIcon;
    glm::vec2 tooltipIconSize = glm::vec2(14.0f);
    glm::vec4 tooltipIconColor = glm::vec4(1.0f);
    glm::vec2 tooltipIconPosition = glm::vec2(0.0f);
};

} // namespace UI
} // namespace VRMusicStudio 