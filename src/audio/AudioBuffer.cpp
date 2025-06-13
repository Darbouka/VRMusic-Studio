#include "audio/AudioBuffer.hpp"
#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace VRMusicStudio {

AudioBuffer::AudioBuffer(size_t numChannels, size_t numFrames)
    : m_numChannels(numChannels)
    , m_numFrames(numFrames)
{
    resize(numChannels, numFrames);
}

void AudioBuffer::resize(size_t numChannels, size_t numFrames)
{
    m_numChannels = numChannels;
    m_numFrames = numFrames;
    m_data.resize(numChannels * numFrames);
    clear();
}

void AudioBuffer::clear()
{
    std::fill(m_data.begin(), m_data.end(), 0.0f);
}

float* AudioBuffer::getChannelData(size_t channel)
{
    if (channel >= m_numChannels) {
        throw std::out_of_range("Channel index out of range");
    }
    return m_data.data() + channel * m_numFrames;
}

const float* AudioBuffer::getChannelData(size_t channel) const
{
    if (channel >= m_numChannels) {
        throw std::out_of_range("Channel index out of range");
    }
    return m_data.data() + channel * m_numFrames;
}

float AudioBuffer::getSample(size_t channel, size_t frame) const
{
    if (channel >= m_numChannels || frame >= m_numFrames) {
        throw std::out_of_range("Channel or frame index out of range");
    }
    return m_data[channel * m_numFrames + frame];
}

void AudioBuffer::setSample(size_t channel, size_t frame, float value)
{
    if (channel >= m_numChannels || frame >= m_numFrames) {
        throw std::out_of_range("Channel or frame index out of range");
    }
    m_data[channel * m_numFrames + frame] = value;
}

void AudioBuffer::copyFrom(const AudioBuffer& other)
{
    if (m_numChannels != other.m_numChannels || m_numFrames != other.m_numFrames) {
        resize(other.m_numChannels, other.m_numFrames);
    }
    std::memcpy(m_data.data(), other.m_data.data(), m_data.size() * sizeof(float));
}

void AudioBuffer::applyGain(float gain)
{
    for (auto& sample : m_data) {
        sample *= gain;
    }
}

void AudioBuffer::applyGainRamp(float startGain, float endGain)
{
    const float gainStep = (endGain - startGain) / static_cast<float>(m_numFrames);
    float currentGain = startGain;

    for (size_t frame = 0; frame < m_numFrames; ++frame) {
        for (size_t channel = 0; channel < m_numChannels; ++channel) {
            const size_t index = channel * m_numFrames + frame;
            m_data[index] *= currentGain;
        }
        currentGain += gainStep;
    }
}

} // namespace VRMusicStudio 