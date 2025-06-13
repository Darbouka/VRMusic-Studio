#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../vr/VREngine.hpp"

namespace VRMusicStudio {

class VRUIComponent {
public:
    VRUIComponent(const std::string& name);
    virtual ~VRUIComponent();

    // Basis-Funktionen
    virtual void initialize() = 0;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;

    // VR-Interaktion
    virtual void setPosition(const glm::vec3& position);
    virtual void setRotation(const glm::quat& rotation);
    virtual void setScale(const glm::vec3& scale);
    virtual void setInteraction(const std::string& interaction);
    virtual void setFeedback(const std::string& feedback);

    // UI-Status
    virtual bool isVisible() const;
    virtual void setVisible(bool visible);
    virtual bool isInteractive() const;
    virtual void setInteractive(bool interactive);

protected:
    std::string name;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    bool visible;
    bool interactive;
    std::string currentInteraction;
    std::string currentFeedback;
};

// Spezifische UI-Komponenten
class VRButton : public VRUIComponent {
public:
    VRButton(const std::string& name);
    void initialize() override;
    void update() override;
    void render() override;
    void shutdown() override;
    void setCallback(std::function<void()> callback);
    void trigger();

private:
    std::function<void()> onClickCallback;
};

class VRSlider : public VRUIComponent {
public:
    VRSlider(const std::string& name);
    void initialize() override;
    void update() override;
    void render() override;
    void shutdown() override;
    void setValue(float value);
    float getValue() const;
    void setCallback(std::function<void(float)> callback);

private:
    float value;
    float minValue;
    float maxValue;
    std::function<void(float)> onChangeCallback;
};

class VRPad : public VRUIComponent {
public:
    VRPad(const std::string& name);
    void initialize() override;
    void update() override;
    void render() override;
    void shutdown() override;
    void setActive(bool active);
    bool isActive() const;
    void setCallback(std::function<void(bool)> callback);

private:
    bool active;
    std::function<void(bool)> onStateChangeCallback;
};

class VRStepSequencer : public VRUIComponent {
public:
    VRStepSequencer(const std::string& name);
    void initialize() override;
    void update() override;
    void render() override;
    void shutdown() override;
    void setPattern(const std::vector<std::vector<bool>>& pattern);
    const std::vector<std::vector<bool>>& getPattern() const;
    void setStep(unsigned int step, unsigned int track, bool value);
    bool getStep(unsigned int step, unsigned int track) const;
    void setCallback(std::function<void(unsigned int, unsigned int, bool)> callback);

private:
    std::vector<std::vector<bool>> pattern;
    unsigned int numSteps;
    unsigned int numTracks;
    std::function<void(unsigned int, unsigned int, bool)> onStepChangeCallback;
};

class VRAudioControlPanel : public VRUIComponent {
public:
    VRAudioControlPanel(const std::string& name);
    void initialize() override;
    void update() override;
    void render() override;
    void shutdown() override;
    void addTrack(const std::string& trackId);
    void removeTrack(const std::string& trackId);
    void setTrackVolume(const std::string& trackId, float volume);
    void setTrackPan(const std::string& trackId, float pan);
    void setTrackMute(const std::string& trackId, bool mute);
    void setTrackSolo(const std::string& trackId, bool solo);

private:
    struct TrackControl {
        std::shared_ptr<VRSlider> volumeSlider;
        std::shared_ptr<VRSlider> panSlider;
        std::shared_ptr<VRButton> muteButton;
        std::shared_ptr<VRButton> soloButton;
    };
    std::map<std::string, TrackControl> trackControls;
};

class VRVisualization : public VRUIComponent {
public:
    VRVisualization(const std::string& name);
    void initialize() override;
    void update() override;
    void render() override;
    void shutdown() override;
    void setData(const std::vector<float>& data);
    void setType(const std::string& type);
    void setColor(const glm::vec3& color);
    void setScale(const glm::vec3& scale);

private:
    std::vector<float> visualizationData;
    std::string visualizationType;
    glm::vec3 color;
    glm::vec3 visualizationScale;
};

} // namespace VRMusicStudio 