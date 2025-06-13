#pragma once

#include "core/ComponentBase.hpp"
#include <vector>

namespace VRMusicStudio {
namespace Audio {

class AudioComponentBase : public Core::ComponentBase {
public:
    virtual ~AudioComponentBase() = default;

    // Audio processing
    virtual void processAudio(float* input, float* output, int numFrames, int numChannels) = 0;
    virtual void setSampleRate(float sampleRate) = 0;
    virtual float getSampleRate() const = 0;
    virtual void setBufferSize(int bufferSize) = 0;
    virtual int getBufferSize() const = 0;

    // Channel management
    virtual void setNumChannels(int numChannels) = 0;
    virtual int getNumChannels() const = 0;
    virtual bool supportsChannelCount(int numChannels) const = 0;

    // Parameter management
    virtual std::vector<std::string> getParameters() const = 0;
    virtual bool setParameter(const std::string& name, float value) = 0;
    virtual float getParameter(const std::string& name) const = 0;
    virtual void setParameterAutomated(const std::string& name, bool automated) = 0;
    virtual bool isParameterAutomated(const std::string& name) const = 0;

    // Preset management
    virtual bool loadPreset(const std::string& name) = 0;
    virtual bool savePreset(const std::string& name) = 0;
    virtual std::vector<std::string> getAvailablePresets() const = 0;

protected:
    float sampleRate = 44100.0f;
    int bufferSize = 1024;
    int numChannels = 2;
    std::map<std::string, bool> parameterAutomation;
};

} // namespace Audio
} // namespace VRMusicStudio 