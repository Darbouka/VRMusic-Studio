#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace VR_DAW {

class VRUIComponent {
public:
    VRUIComponent(const std::string& name);
    virtual ~VRUIComponent();

    // Komponenten-Identifikation
    const std::string& getId() const { return id; }
    const std::string& getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }

    // Position und Ausrichtung
    void setPosition(const glm::vec3& position);
    void setOrientation(const glm::quat& orientation);
    void setScale(const glm::vec3& scale);
    const glm::vec3& getPosition() const { return position; }
    const glm::quat& getOrientation() const { return orientation; }
    const glm::vec3& getScale() const { return scale; }

    // Sichtbarkeit und Interaktivität
    void setVisible(bool visible);
    void setInteractive(bool interactive);
    bool isVisible() const { return visible; }
    bool isInteractive() const { return interactive; }

    // Event-Handling
    virtual bool handlePointerEnter(const glm::vec3& pointerPosition);
    virtual bool handlePointerExit();
    virtual bool handlePointerClick(const glm::vec3& pointerPosition);
    virtual bool handlePointerDrag(const glm::vec3& pointerPosition);
    virtual bool handlePointerRelease();

    // Rendering
    virtual void render() = 0;
    virtual void update() = 0;

    // Kind-Komponenten
    void addChild(std::shared_ptr<VRUIComponent> child);
    void removeChild(const std::string& childId);
    std::vector<std::shared_ptr<VRUIComponent>> getChildren() const { return children; }

protected:
    std::string id;
    std::string name;
    glm::vec3 position;
    glm::quat orientation;
    glm::vec3 scale;
    bool visible;
    bool interactive;
    std::vector<std::shared_ptr<VRUIComponent>> children;

    // Event-Callbacks
    std::function<void()> onPointerEnter;
    std::function<void()> onPointerExit;
    std::function<void()> onPointerClick;
    std::function<void()> onPointerDrag;
    std::function<void()> onPointerRelease;

private:
    void generateId();
};

} // namespace VR_DAW 