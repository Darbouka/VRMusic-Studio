#pragma once

#include "AudioPlugin.hpp"
#include <vector>
#include <memory>

namespace VRMusicStudio {
namespace Audio {

class DelayPlugin : public AudioPlugin {
public:
    DelayPlugin();
    ~DelayPlugin() override;

    bool initialize() override;
    void shutdown() override;
    void update() override;
    void process(float* input, float* output, size_t numFrames) override;

    // Delay-spezifische Parameter
    void setDelayTime(float timeInSeconds);
    void setFeedback(float feedback);
    void setMix(float mix);
    void setLowPassCutoff(float cutoff);
    void setHighPassCutoff(float cutoff);
    void setPingPong(bool enabled);

    float getDelayTime() const;
    float getFeedback() const;
    float getMix() const;
    float getLowPassCutoff() const;
    float getHighPassCutoff() const;
    bool isPingPongEnabled() const;

private:
    // Delay-Implementierung
    struct DelayImpl;
    std::unique_ptr<DelayImpl> impl;

    // Parameter
    float delayTime;
    float feedback;
    float mix;
    float lowPassCutoff;
    float highPassCutoff;
    bool pingPong;
};

} // namespace Audio
} // namespace VRMusicStudio 