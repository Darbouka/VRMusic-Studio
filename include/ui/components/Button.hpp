#pragma once

#include "ui/UIComponentBase.hpp"
#include <string>
#include <functional>

namespace VRMusicStudio {
namespace UI {

class Button : public UIComponentBase {
public:
    Button() = default;
    ~Button() override = default;

    // Component identification
    std::string getName() const override { return "Button"; }
    std::string getVersion() const override { return "1.0.0"; }

    // Lifecycle management
    bool initialize() override;
    void shutdown() override;
    void update() override;

    // Button specific
    void setText(const std::string& text);
    std::string getText() const;
    void setFont(const std::string& font);
    std::string getFont() const;
    void setFontSize(float size);
    float getFontSize() const;
    void setTextColor(const glm::vec4& color);
    glm::vec4 getTextColor() const;
    void setBackgroundColor(const glm::vec4& color);
    glm::vec4 getBackgroundColor() const;
    void setHoverColor(const glm::vec4& color);
    glm::vec4 getHoverColor() const;
    void setPressedColor(const glm::vec4& color);
    glm::vec4 getPressedColor() const;
    void setBorderColor(const glm::vec4& color);
    glm::vec4 getBorderColor() const;
    void setBorderWidth(float width);
    float getBorderWidth() const;
    void setCornerRadius(float radius);
    float getCornerRadius() const;
    void setPadding(const glm::vec2& padding);
    glm::vec2 getPadding() const;
    void setIcon(const std::string& icon);
    std::string getIcon() const;
    void setIconSize(const glm::vec2& size);
    glm::vec2 getIconSize() const;
    void setIconColor(const glm::vec4& color);
    glm::vec4 getIconColor() const;
    void setIconPosition(const glm::vec2& position);
    glm::vec2 getIconPosition() const;
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
    void setOnClickCallback(std::function<void()> callback);
    void setOnDoubleClickCallback(std::function<void()> callback);
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
    std::string text;
    std::string font;
    float fontSize = 16.0f;
    glm::vec4 textColor = glm::vec4(1.0f);
    glm::vec4 backgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glm::vec4 hoverColor = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
    glm::vec4 pressedColor = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    glm::vec4 borderColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    float borderWidth = 1.0f;
    float cornerRadius = 4.0f;
    glm::vec2 padding = glm::vec2(8.0f);
    std::string icon;
    glm::vec2 iconSize = glm::vec2(16.0f);
    glm::vec4 iconColor = glm::vec4(1.0f);
    glm::vec2 iconPosition = glm::vec2(0.0f);
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