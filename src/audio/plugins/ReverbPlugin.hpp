#pragma once

#include "AudioPlugin.hpp"
#include <vector>
#include <memory>

namespace VRMusicStudio {
namespace Audio {

class ReverbPlugin : public AudioPlugin {
public:
    ReverbPlugin();
    ~ReverbPlugin() override;

    bool initialize() override;
    void shutdown() override;
    void update() override;
    void process(float* input, float* output, size_t numFrames) override;

    // Reverb-spezifische Parameter
    void setRoomSize(float size);
    void setDamping(float damping);
    void setWetLevel(float level);
    void setDryLevel(float level);
    void setWidth(float width);
    void setFreeze(bool freeze);

    float getRoomSize() const;
    float getDamping() const;
    float getWetLevel() const;
    float getDryLevel() const;
    float getWidth() const;
    bool isFrozen() const;

private:
    // Reverb-Implementierung
    struct ReverbImpl;
    std::unique_ptr<ReverbImpl> impl;

    // Parameter
    float roomSize;
    float damping;
    float wetLevel;
    float dryLevel;
    float width;
    bool freeze;
};

} // namespace Audio
} // namespace VRMusicStudio 