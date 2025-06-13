#pragma once

#include <memory>
#include <vector>
#include "AudioEffect.hpp"

namespace VRMusicStudio {

class EffectEngine {
public:
    EffectEngine();
    
    void setSampleRate(double sampleRate);
    void setBlockSize(int blockSize);
    void setChannelCount(int numChannels);
    void process(float* buffer, int numFrames);
    
    void addEffect(std::shared_ptr<AudioEffect> effect);
    void removeEffect(std::shared_ptr<AudioEffect> effect);
    
    void startProcessing();
    void stopProcessing();
    bool isProcessing() const;

private:
    void updateEffects();
    void updateEffect(std::shared_ptr<AudioEffect> effect);
    
    double m_sampleRate;
    int m_blockSize;
    int m_numChannels;
    bool m_isProcessing;
    std::vector<std::shared_ptr<AudioEffect>> m_effects;
};

} // namespace VRMusicStudio 