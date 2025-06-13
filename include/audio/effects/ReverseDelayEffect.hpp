#pragma once

#include <vector>
#include <string>
#include "AudioEffect.hpp"

namespace VRMusicStudio {

class ReverseDelayEffect : public AudioEffect {
public:
    ReverseDelayEffect();
    
    void setSampleRate(double sampleRate) override;
    void setBlockSize(int blockSize) override;
    void setChannelCount(int numChannels) override;
    void process(float* buffer, int numFrames) override;
    
    void setParameter(int index, float value) override;
    float getParameter(int index) const override;
    int getParameterCount() const override;
    std::string getParameterName(int index) const override;

private:
    void resizeBuffer();
    
    float m_delayTime;
    float m_feedback;
    float m_mix;
    double m_sampleRate;
    int m_blockSize;
    int m_numChannels;
    std::vector<float> m_buffer;
    size_t m_writeIndex;
};

} // namespace VRMusicStudio 