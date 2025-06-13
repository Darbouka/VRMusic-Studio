#include "audio/effects/DistortionEffect.hpp"
#include <cmath>
#include <stdexcept>

namespace VRMusicStudio {

DistortionEffect::DistortionEffect()
    : m_drive(1.0f)
    , m_mix(1.0f)
    , m_isEnabled(false)
{
}

void DistortionEffect::setDrive(float drive)
{
    if (drive < 0.0f || drive > 10.0f) {
        throw std::invalid_argument("Drive must be between 0.0 and 10.0");
    }
    m_drive = drive;
}

void DistortionEffect::setMix(float mix)
{
    if (mix < 0.0f || mix > 1.0f) {
        throw std::invalid_argument("Mix must be between 0.0 and 1.0");
    }
    m_mix = mix;
}

void DistortionEffect::setEnabled(bool enabled)
{
    m_isEnabled = enabled;
}

void DistortionEffect::process(AudioBuffer& buffer)
{
    if (!m_isEnabled) {
        return;
    }

    const size_t numChannels = buffer.getNumChannels();
    const size_t numFrames = buffer.getNumFrames();

    for (size_t channel = 0; channel < numChannels; ++channel) {
        float* channelData = buffer.getChannelData(channel);
        
        for (size_t frame = 0; frame < numFrames; ++frame) {
            float input = channelData[frame];
            float distorted = std::tanh(input * m_drive);
            channelData[frame] = input * (1.0f - m_mix) + distorted * m_mix;
        }
    }
}

float DistortionEffect::getDrive() const
{
    return m_drive;
}

float DistortionEffect::getMix() const
{
    return m_mix;
}

bool DistortionEffect::isEnabled() const
{
    return m_isEnabled;
}

} // namespace VRMusicStudio 