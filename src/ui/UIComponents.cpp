#include "UIComponents.hpp"
#include <spdlog/spdlog.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace VRMusicStudio {

// Slider-Implementierung
void UISlider::render() {
    if (!visible) return;
    
    ImGui::SetCursorPos(ImVec2(position.x, position.y));
    if (ImGui::SliderFloat(label.c_str(), &value, min, max)) {
        if (onValueChanged) {
            onValueChanged(value);
        }
    }
}

// Toggle-Implementierung
void UIToggle::render() {
    if (!visible) return;
    
    ImGui::SetCursorPos(ImVec2(position.x, position.y));
    if (ImGui::Checkbox(label.c_str(), &value)) {
        if (onValueChanged) {
            onValueChanged(value);
        }
    }
}

// Input-Implementierung
void UIInput::render() {
    if (!visible) return;
    
    ImGui::SetCursorPos(ImVec2(position.x, position.y));
    if (ImGui::InputText(label.c_str(), buffer, bufferSize)) {
        if (onValueChanged) {
            onValueChanged(buffer);
        }
    }
}

// Button-Implementierung
void UIButton::render() {
    if (!visible) return;
    
    ImGui::SetCursorPos(ImVec2(position.x, position.y));
    if (ImGui::Button(label.c_str(), ImVec2(size.x, size.y))) {
        if (onClick) {
            onClick();
        }
    }
}

// Panel-Implementierung
void UIPanel::render() {
    if (!visible) return;
    
    ImGui::SetNextWindowPos(ImVec2(position.x, position.y));
    ImGui::SetNextWindowSize(ImVec2(size.x, size.y));
    
    ImGuiWindowFlags flags = 0;
    if (!draggable) flags |= ImGuiWindowFlags_NoMove;
    if (!resizable) flags |= ImGuiWindowFlags_NoResize;
    
    if (ImGui::Begin(title.c_str(), nullptr, flags)) {
        for (auto& element : elements) {
            element->render();
        }
    }
    ImGui::End();
}

// Menu-Implementierung
void UIMenu::render() {
    if (!visible) return;
    
    ImGui::SetCursorPos(ImVec2(position.x, position.y));
    if (ImGui::BeginCombo(label.c_str(), items[0].c_str())) {
        for (const auto& item : items) {
            if (ImGui::Selectable(item.c_str())) {
                if (onItemSelected) {
                    onItemSelected(item);
                }
            }
        }
        ImGui::EndCombo();
    }
}

// Waveform-Implementierung
void UIWaveform::render() {
    if (!visible || data.empty()) return;
    
    ImGui::SetCursorPos(ImVec2(position.x, position.y));
    ImGui::BeginChild(label.c_str(), ImVec2(size.x, size.y), true);
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    
    float maxValue = autoScale ? 
        *std::max_element(data.begin(), data.end()) : 1.0f;
    
    for (size_t i = 0; i < data.size() - 1; ++i) {
        float x1 = p.x + (i * size.x / (data.size() - 1));
        float x2 = p.x + ((i + 1) * size.x / (data.size() - 1));
        float y1 = p.y + size.y/2 + (data[i] * size.y/2 / maxValue);
        float y2 = p.y + size.y/2 + (data[i+1] * size.y/2 / maxValue);
        
        draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), 
                          ImGui::GetColorU32(ImVec4(1,1,1,1)));
    }
    
    ImGui::EndChild();
}

// Spectrum-Implementierung
void UISpectrum::render() {
    if (!visible || data.empty()) return;
    
    ImGui::SetCursorPos(ImVec2(position.x, position.y));
    ImGui::BeginChild(label.c_str(), ImVec2(size.x, size.y), true);
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    
    float maxValue = autoScale ? 
        *std::max_element(data.begin(), data.end()) : 1.0f;
    
    float barWidth = size.x / data.size();
    for (size_t i = 0; i < data.size(); ++i) {
        float height = (data[i] * size.y / maxValue);
        float x = p.x + (i * barWidth);
        float y = p.y + size.y - height;
        
        draw_list->AddRectFilled(
            ImVec2(x, y),
            ImVec2(x + barWidth - 1, p.y + size.y),
            ImGui::GetColorU32(ImVec4(1,1,1,1))
        );
    }
    
    ImGui::EndChild();
}

// Knob-Implementierung
void UIKnob::render() {
    if (!visible) return;
    
    ImGui::SetCursorPos(ImVec2(position.x, position.y));
    
    float knobSize = size.x;
    ImVec2 center = ImVec2(position.x + knobSize/2, position.y + knobSize/2);
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    // Knob-Kreis
    draw_list->AddCircle(center, knobSize/2, 
                        ImGui::GetColorU32(ImVec4(0.3f,0.3f,0.3f,1)));
    
    // Wert-Linie
    float angle = (value - min) / (max - min) * 2.0f * M_PI - M_PI/2;
    float x = center.x + cos(angle) * (knobSize/2 - 5);
    float y = center.y + sin(angle) * (knobSize/2 - 5);
    draw_list->AddLine(center, ImVec2(x,y), 
                      ImGui::GetColorU32(ImVec4(1,1,1,1)), 2);
    
    // Label
    ImGui::SetCursorPos(ImVec2(position.x, position.y + knobSize + 5));
    ImGui::Text("%s: %.2f", label.c_str(), value);
}

// Theme-Manager-Implementierung
void ThemeManager::setTheme(const std::string& theme) {
    if (theme == "dark") {
        backgroundColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
        textColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        accentColor = glm::vec4(0.2f, 0.6f, 1.0f, 1.0f);
    } else {
        backgroundColor = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
        textColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
        accentColor = glm::vec4(0.0f, 0.4f, 0.8f, 1.0f);
    }
}

void ThemeManager::applyTheme() {
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = 
        ImVec4(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    ImGui::GetStyle().Colors[ImGuiCol_Text] = 
        ImVec4(textColor.r, textColor.g, textColor.b, textColor.a);
    ImGui::GetStyle().Colors[ImGuiCol_Button] = 
        ImVec4(accentColor.r, accentColor.g, accentColor.b, accentColor.a);
}

// Layout-Manager-Implementierung
void LayoutManager::setLayout(const std::string& layout) {
    if (layout == "vertical") {
        arrangeVertical();
    } else if (layout == "horizontal") {
        arrangeHorizontal();
    } else if (layout == "grid") {
        arrangeGrid();
    }
}

void LayoutManager::arrangeVertical() {
    float currentY = position.y + padding;
    for (auto& element : elements) {
        element->setPosition(glm::vec2(position.x + padding, currentY));
        currentY += element->getSize().y + spacing;
    }
}

void LayoutManager::arrangeHorizontal() {
    float currentX = position.x + padding;
    for (auto& element : elements) {
        element->setPosition(glm::vec2(currentX, position.y + padding));
        currentX += element->getSize().x + spacing;
    }
}

void LayoutManager::arrangeGrid() {
    int columns = static_cast<int>(std::sqrt(elements.size()));
    float cellWidth = (size.x - (columns + 1) * padding) / columns;
    float cellHeight = (size.y - (elements.size() / columns + 1) * padding) / (elements.size() / columns);
    
    for (size_t i = 0; i < elements.size(); ++i) {
        int row = i / columns;
        int col = i % columns;
        float x = position.x + padding + col * (cellWidth + padding);
        float y = position.y + padding + row * (cellHeight + padding);
        elements[i]->setPosition(glm::vec2(x, y));
        elements[i]->setSize(glm::vec2(cellWidth, cellHeight));
    }
}

} // namespace VRMusicStudio 