#pragma once

#include "audio/AudioComponentBase.hpp"

namespace VRMusicStudio {
namespace Audio {

class EffectBase : public AudioComponentBase {
public:
    virtual ~EffectBase() = default;

    // Plugin identification
    virtual std::string getName() const = 0;
    virtual std::string getVendor() const = 0;
    virtual std::string getCategory() const = 0;
    virtual std::string getVersion() const = 0;

    // Lifecycle management
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;

    // Parameter management
    virtual std::vector<std::string> getParameters() const = 0;
    virtual bool setParameter(const std::string& name, float value) = 0;
    virtual float getParameter(const std::string& name) const = 0;

    // Parameter automation
    virtual void setParameterAutomated(const std::string& name, bool automated) = 0;
    virtual bool isParameterAutomated(const std::string& name) const = 0;

    // Audio processing
    virtual void processAudio(float* input, float* output, int numFrames, int numChannels) = 0;

    // Preset management
    virtual bool loadPreset(const std::string& name) = 0;
    virtual bool savePreset(const std::string& name) = 0;
    virtual std::vector<std::string> getAvailablePresets() const = 0;

    // Effect specific
    virtual std::string getEffectType() const = 0;
    virtual bool supportsMono() const = 0;
    virtual bool supportsStereo() const = 0;
    virtual bool supportsMultiChannel() const = 0;

    // Processing
    virtual void processMono(float* input, float* output, int numFrames) = 0;
    virtual void processStereo(float* input, float* output, int numFrames) = 0;
    virtual void processMultiChannel(float* input, float* output, int numFrames, int numChannels) = 0;

    // Quality settings
    virtual void setQuality(int quality) = 0;
    virtual int getQuality() const = 0;
    virtual std::vector<int> getAvailableQualities() const = 0;

    // Bypass
    virtual void setBypass(bool bypass) = 0;
    virtual bool isBypassed() const = 0;

protected:
    // Common parameters
    float mix = 1.0f;
    int quality = 1;
    bool bypass = false;

    // Parameter automation states
    std::map<std::string, bool> parameterAutomation;
};

} // namespace Audio
} // namespace VRMusicStudio 