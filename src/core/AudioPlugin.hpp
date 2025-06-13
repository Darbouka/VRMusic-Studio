#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>

namespace VRMusicStudio {
namespace Core {

class AudioPlugin {
public:
    virtual ~AudioPlugin() = default;

    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void update() = 0;

    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getAuthor() const = 0;
    virtual std::string getDescription() const = 0;

    virtual bool loadPreset(const std::string& presetName) = 0;
    virtual bool savePreset(const std::string& presetName) = 0;
    virtual std::vector<std::string> getAvailablePresets() const = 0;

    virtual void setParameter(const std::string& paramName, float value) = 0;
    virtual float getParameter(const std::string& paramName) const = 0;
    virtual std::vector<std::string> getParameterNames() const = 0;

    virtual void processAudio(float* input, float* output, int numFrames) = 0;
};

using AudioPluginPtr = std::shared_ptr<AudioPlugin>;
using AudioPluginFactory = std::function<AudioPluginPtr()>;

} // namespace Core
} // namespace VRMusicStudio 