#pragma once

#include <vector>
#include <cstddef>

namespace VRMusicStudio {

class AudioBuffer {
public:
    AudioBuffer(size_t numChannels = 0, size_t numFrames = 0);
    
    void resize(size_t numChannels, size_t numFrames);
    void clear();
    
    float* getChannelData(size_t channel);
    const float* getChannelData(size_t channel) const;
    
    float getSample(size_t channel, size_t frame) const;
    void setSample(size_t channel, size_t frame, float value);
    
    void copyFrom(const AudioBuffer& other);
    
    void applyGain(float gain);
    void applyGainRamp(float startGain, float endGain);
    
    size_t getNumChannels() const { return m_numChannels; }
    size_t getNumFrames() const { return m_numFrames; }
    size_t getTotalSize() const { return m_data.size(); }

private:
    std::vector<float> m_data;
    size_t m_numChannels;
    size_t m_numFrames;
};

} // namespace VRMusicStudio 