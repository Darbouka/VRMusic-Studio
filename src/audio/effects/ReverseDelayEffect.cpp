#include "ReverseDelayEffect.hpp"
#include <algorithm>
#include <cmath>

namespace VRMusicStudio {

ReverseDelayEffect::ReverseDelayEffect()
    : m_delayTime(0.5f)
    , m_feedback(0.3f)
    , m_mix(0.5f)
    , m_sampleRate(44100.0)
    , m_blockSize(512)
    , m_numChannels(2)
{
    resizeBuffer();
}

void ReverseDelayEffect::setSampleRate(double sampleRate)
{
    m_sampleRate = sampleRate;
    resizeBuffer();
}

void ReverseDelayEffect::setBlockSize(int blockSize)
{
    m_blockSize = blockSize;
    resizeBuffer();
}

void ReverseDelayEffect::setChannelCount(int numChannels)
{
    m_numChannels = numChannels;
    resizeBuffer();
}

void ReverseDelayEffect::resizeBuffer()
{
    int bufferSize = static_cast<int>(m_delayTime * m_sampleRate * m_numChannels);
    m_buffer.resize(bufferSize, 0.0f);
    m_writeIndex = 0;
}

void ReverseDelayEffect::process(float* buffer, int numFrames)
{
    for (int i = 0; i < numFrames * m_numChannels; ++i) {
        float input = buffer[i];
        float delayed = m_buffer[m_writeIndex];
        
        // Reverse delay processing
        float output = input + delayed * m_mix;
        m_buffer[m_writeIndex] = input + delayed * m_feedback;
        
        buffer[i] = output;
        
        m_writeIndex = (m_writeIndex + 1) % m_buffer.size();
    }
}

void ReverseDelayEffect::setParameter(int index, float value)
{
    switch (index) {
        case 0: 
            m_delayTime = value * 2.0f;  // 0-2 seconds
            resizeBuffer();
            break;
        case 1: m_feedback = value; break;  // 0-1 range
        case 2: m_mix = value; break;      // 0-1 range
    }
}

float ReverseDelayEffect::getParameter(int index) const
{
    switch (index) {
        case 0: return m_delayTime / 2.0f;
        case 1: return m_feedback;
        case 2: return m_mix;
        default: return 0.0f;
    }
}

int ReverseDelayEffect::getParameterCount() const
{
    return 3;
}

std::string ReverseDelayEffect::getParameterName(int index) const
{
    switch (index) {
        case 0: return "Delay Time";
        case 1: return "Feedback";
        case 2: return "Mix";
        default: return "";
    }
}

} // namespace VRMusicStudio 