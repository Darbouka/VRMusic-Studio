#include "AudioControlPanel.hpp"
#include <spdlog/spdlog.h>

namespace VR_DAW {

AudioControlPanel::AudioControlPanel(const std::string& name)
    : VRUIComponent(name)
{
}

AudioControlPanel::~AudioControlPanel() {
    trackControls.clear();
    pluginControls.clear();
}

void AudioControlPanel::addTrackControl(const std::string& trackId) {
    createTrackControl(trackId);
    updateTrackControl(trackId);
}

void AudioControlPanel::removeTrackControl(const std::string& trackId) {
    trackControls.erase(trackId);
}

void AudioControlPanel::updateTrackControls() {
    for (const auto& [trackId, control] : trackControls) {
        updateTrackControl(trackId);
    }
}

void AudioControlPanel::addPluginControl(const std::string& trackId, const std::string& pluginId) {
    createPluginControl(trackId, pluginId);
    updatePluginControl(trackId, pluginId);
}

void AudioControlPanel::removePluginControl(const std::string& trackId, const std::string& pluginId) {
    auto it = pluginControls.find(trackId);
    if (it != pluginControls.end()) {
        it->second.erase(pluginId);
    }
}

void AudioControlPanel::updatePluginControls() {
    for (const auto& [trackId, plugins] : pluginControls) {
        for (const auto& [pluginId, control] : plugins) {
            updatePluginControl(trackId, pluginId);
        }
    }
}

void AudioControlPanel::render() {
    if (!visible) return;

    // Track-Controls rendern
    for (const auto& [trackId, control] : trackControls) {
        if (control.volumeSlider) control.volumeSlider->render();
        if (control.muteButton) control.muteButton->render();
        if (control.soloButton) control.soloButton->render();
        if (control.panControl) control.panControl->render();
    }

    // Plugin-Controls rendern
    for (const auto& [trackId, plugins] : pluginControls) {
        for (const auto& [pluginId, control] : plugins) {
            if (control.bypassButton) control.bypassButton->render();
            if (control.parameterControls) control.parameterControls->render();
        }
    }

    // Kind-Komponenten rendern
    for (const auto& child : children) {
        child->render();
    }
}

void AudioControlPanel::update() {
    if (!visible) return;

    // Track-Controls aktualisieren
    updateTrackControls();

    // Plugin-Controls aktualisieren
    updatePluginControls();

    // Kind-Komponenten aktualisieren
    for (const auto& child : children) {
        child->update();
    }
}

void AudioControlPanel::createTrackControl(const std::string& trackId) {
    TrackControl control;

    // Volume-Slider erstellen
    control.volumeSlider = std::make_shared<VRUIComponent>("VolumeSlider");
    control.volumeSlider->setPosition(position + glm::vec3(0.0f, 0.1f, 0.0f));
    control.volumeSlider->setScale(glm::vec3(0.1f, 0.3f, 0.01f));

    // Mute-Button erstellen
    control.muteButton = std::make_shared<VRUIComponent>("MuteButton");
    control.muteButton->setPosition(position + glm::vec3(0.0f, 0.0f, 0.0f));
    control.muteButton->setScale(glm::vec3(0.05f, 0.05f, 0.01f));

    // Solo-Button erstellen
    control.soloButton = std::make_shared<VRUIComponent>("SoloButton");
    control.soloButton->setPosition(position + glm::vec3(0.0f, -0.1f, 0.0f));
    control.soloButton->setScale(glm::vec3(0.05f, 0.05f, 0.01f));

    // Pan-Control erstellen
    control.panControl = std::make_shared<VRUIComponent>("PanControl");
    control.panControl->setPosition(position + glm::vec3(0.0f, -0.2f, 0.0f));
    control.panControl->setScale(glm::vec3(0.1f, 0.05f, 0.01f));

    trackControls[trackId] = control;
}

void AudioControlPanel::createPluginControl(const std::string& trackId, const std::string& pluginId) {
    PluginControl control;

    // Bypass-Button erstellen
    control.bypassButton = std::make_shared<VRUIComponent>("BypassButton");
    control.bypassButton->setPosition(position + glm::vec3(0.0f, 0.0f, 0.0f));
    control.bypassButton->setScale(glm::vec3(0.05f, 0.05f, 0.01f));

    // Parameter-Controls erstellen
    control.parameterControls = std::make_shared<VRUIComponent>("ParameterControls");
    control.parameterControls->setPosition(position + glm::vec3(0.0f, -0.1f, 0.0f));
    control.parameterControls->setScale(glm::vec3(0.1f, 0.2f, 0.01f));

    pluginControls[trackId][pluginId] = control;
}

void AudioControlPanel::updateTrackControl(const std::string& trackId) {
    auto it = trackControls.find(trackId);
    if (it == trackControls.end()) return;

    auto& control = it->second;
    auto track = AudioEngine::getInstance().getAudioTrack(trackId);
    if (!track) return;

    // Volume-Slider aktualisieren
    if (control.volumeSlider) {
        // Hier würde die Position des Sliders basierend auf dem Track-Volume aktualisiert
    }

    // Mute-Button aktualisieren
    if (control.muteButton) {
        // Hier würde der Button-Status basierend auf dem Track-Mute-Status aktualisiert
    }

    // Solo-Button aktualisieren
    if (control.soloButton) {
        // Hier würde der Button-Status basierend auf dem Track-Solo-Status aktualisiert
    }

    // Pan-Control aktualisieren
    if (control.panControl) {
        // Hier würde die Position des Pan-Controls basierend auf dem Track-Pan aktualisiert
    }
}

void AudioControlPanel::updatePluginControl(const std::string& trackId, const std::string& pluginId) {
    auto trackIt = pluginControls.find(trackId);
    if (trackIt == pluginControls.end()) return;

    auto pluginIt = trackIt->second.find(pluginId);
    if (pluginIt == trackIt->second.end()) return;

    auto& control = pluginIt->second;
    auto track = AudioEngine::getInstance().getAudioTrack(trackId);
    if (!track) return;

    // Bypass-Button aktualisieren
    if (control.bypassButton) {
        // Hier würde der Button-Status basierend auf dem Plugin-Bypass-Status aktualisiert
    }

    // Parameter-Controls aktualisieren
    if (control.parameterControls) {
        // Hier würden die Parameter-Controls basierend auf den Plugin-Parametern aktualisiert
    }
}

} // namespace VR_DAW 