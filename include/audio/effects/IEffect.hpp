#pragma once

#include "audio/AudioBuffer.hpp"

namespace VRMusicStudio {

class IEffect {
public:
    virtual ~IEffect() = default;
    virtual void process(AudioBuffer& buffer) = 0;
};

} // namespace VRMusicStudio 