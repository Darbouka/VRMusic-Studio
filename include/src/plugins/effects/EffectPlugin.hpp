#pragma once

#include "../PluginInterface.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace VR_DAW {

class EffectPlugin : public PluginInterface {
public:
    EffectPlugin() = default;
    virtual ~EffectPlugin() = default;

    // Effekt-spezifische Parameter
    virtual std::vector<PluginParameter> getParameters() const override = 0;
    virtual void setParameter(const std::string& name, float value) override = 0;
    virtual float getParameter(const std::string& name) const override = 0;
    virtual void setParameterAutomated(const std::string& name, bool automated) override = 0;
    virtual bool isParameterAutomated(const std::string& name) const override = 0;

    // Audio-Verarbeitung
    virtual void processAudio(float* buffer, unsigned long framesPerBuffer) override = 0;

    // Preset-Management
    virtual void loadPreset(const std::string& presetName) = 0;
    virtual void savePreset(const std::string& presetName) = 0;
    virtual std::vector<std::string> getAvailablePresets() const = 0;
};

} // namespace VR_DAW 