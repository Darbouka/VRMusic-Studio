#include "VRUIComponent.hpp"
#include <spdlog/spdlog.h>
#include <random>
#include <sstream>
#include <iomanip>

namespace VR_DAW {

VRUIComponent::VRUIComponent(const std::string& name)
    : name(name)
    , position(0.0f)
    , orientation(1.0f, 0.0f, 0.0f, 0.0f)
    , scale(1.0f)
    , visible(true)
    , interactive(true)
{
    generateId();
}

VRUIComponent::~VRUIComponent() {
    children.clear();
}

void VRUIComponent::generateId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    const char* hex = "0123456789abcdef";
    
    std::stringstream ss;
    for (int i = 0; i < 32; ++i) {
        ss << hex[dis(gen)];
    }
    id = ss.str();
}

void VRUIComponent::setPosition(const glm::vec3& newPosition) {
    position = newPosition;
}

void VRUIComponent::setOrientation(const glm::quat& newOrientation) {
    orientation = newOrientation;
}

void VRUIComponent::setScale(const glm::vec3& newScale) {
    scale = newScale;
}

void VRUIComponent::setVisible(bool newVisible) {
    visible = newVisible;
}

void VRUIComponent::setInteractive(bool newInteractive) {
    interactive = newInteractive;
}

bool VRUIComponent::handlePointerEnter(const glm::vec3& pointerPosition) {
    if (!interactive || !visible) return false;
    
    if (onPointerEnter) {
        onPointerEnter();
    }
    return true;
}

bool VRUIComponent::handlePointerExit() {
    if (!interactive || !visible) return false;
    
    if (onPointerExit) {
        onPointerExit();
    }
    return true;
}

bool VRUIComponent::handlePointerClick(const glm::vec3& pointerPosition) {
    if (!interactive || !visible) return false;
    
    if (onPointerClick) {
        onPointerClick();
    }
    return true;
}

bool VRUIComponent::handlePointerDrag(const glm::vec3& pointerPosition) {
    if (!interactive || !visible) return false;
    
    if (onPointerDrag) {
        onPointerDrag();
    }
    return true;
}

bool VRUIComponent::handlePointerRelease() {
    if (!interactive || !visible) return false;
    
    if (onPointerRelease) {
        onPointerRelease();
    }
    return true;
}

void VRUIComponent::addChild(std::shared_ptr<VRUIComponent> child) {
    children.push_back(child);
}

void VRUIComponent::removeChild(const std::string& childId) {
    children.erase(
        std::remove_if(children.begin(), children.end(),
            [&](const auto& child) { return child->getId() == childId; }),
        children.end()
    );
}

} // namespace VR_DAW 