#pragma once

#include <string>

namespace VRMusicStudio {

class AudioEffect {
public:
    virtual ~AudioEffect() = default;
    
    virtual void setSampleRate(double sampleRate) = 0;
    virtual void setBlockSize(int blockSize) = 0;
    virtual void setChannelCount(int numChannels) = 0;
    virtual void process(float* buffer, int numFrames) = 0;
    
    virtual void setParameter(int index, float value) = 0;
    virtual float getParameter(int index) const = 0;
    virtual int getParameterCount() const = 0;
    virtual std::string getParameterName(int index) const = 0;
};

} // namespace VRMusicStudio 