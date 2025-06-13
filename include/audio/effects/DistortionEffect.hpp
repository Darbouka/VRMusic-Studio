#pragma once

#include "audio/AudioBuffer.hpp"

namespace VRMusicStudio {

class DistortionEffect {
public:
    DistortionEffect();

    void setDrive(float drive);
    void setMix(float mix);
    void setEnabled(bool enabled);

    void process(AudioBuffer& buffer);

    float getDrive() const;
    float getMix() const;
    bool isEnabled() const;

private:
    float m_drive;
    float m_mix;
    bool m_isEnabled;
};

} // namespace VRMusicStudio 