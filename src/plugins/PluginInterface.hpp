#pragma once

#include <string>
#include <memory>
#include <vector>

namespace VRMusicStudio {

class PluginInterface {
public:
    virtual ~PluginInterface() = default;

    // Plugin-Informationen
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getVendor() const = 0;

    // Plugin-Lebenszyklus
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void process() = 0;

    // Parameter-Handling
    virtual void setParameter(int index, float value) = 0;
    virtual float getParameter(int index) const = 0;
    virtual int getParameterCount() const = 0;
    virtual std::string getParameterName(int index) const = 0;

    // Audio-Format
    virtual void setSampleRate(double sampleRate) = 0;
    virtual void setBlockSize(int blockSize) = 0;
    virtual void setChannelCount(int numChannels) = 0;
};

// Plugin-Factory
class PluginFactory {
public:
    virtual ~PluginFactory() = default;
    virtual std::unique_ptr<PluginInterface> createPlugin() = 0;
};

} // namespace VRMusicStudio 