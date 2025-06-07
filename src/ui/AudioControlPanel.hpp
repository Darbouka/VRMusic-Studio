#pragma once

#include "VRUIComponent.hpp"
#include "../audio/AudioEngine.hpp"
#include <memory>
#include <string>
#include <map>

namespace VR_DAW {

class AudioControlPanel : public VRUIComponent {
public:
    AudioControlPanel(const std::string& name);
    ~AudioControlPanel();

    // Track-Management
    void addTrackControl(const std::string& trackId);
    void removeTrackControl(const std::string& trackId);
    void updateTrackControls();

    // Plugin-Management
    void addPluginControl(const std::string& trackId, const std::string& pluginId);
    void removePluginControl(const std::string& trackId, const std::string& pluginId);
    void updatePluginControls();

    // Rendering
    void render() override;
    void update() override;

private:
    // UI-Komponenten
    struct TrackControl {
        std::shared_ptr<VRUIComponent> volumeSlider;
        std::shared_ptr<VRUIComponent> muteButton;
        std::shared_ptr<VRUIComponent> soloButton;
        std::shared_ptr<VRUIComponent> panControl;
    };

    struct PluginControl {
        std::shared_ptr<VRUIComponent> bypassButton;
        std::shared_ptr<VRUIComponent> parameterControls;
    };

    std::map<std::string, TrackControl> trackControls;
    std::map<std::string, std::map<std::string, PluginControl>> pluginControls;

    // Hilfsfunktionen
    void createTrackControl(const std::string& trackId);
    void createPluginControl(const std::string& trackId, const std::string& pluginId);
    void updateTrackControl(const std::string& trackId);
    void updatePluginControl(const std::string& trackId, const std::string& pluginId);
};

} // namespace VR_DAW 