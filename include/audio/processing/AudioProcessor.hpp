#pragma once

#include "audio/AudioBuffer.hpp"
#include "audio/effects/IEffect.hpp"
#include <memory>
#include <vector>

namespace VRMusicStudio {

class AudioProcessor {
public:
    AudioProcessor();
    ~AudioProcessor();

    bool initialize(int sampleRate, int blockSize, int numChannels);
    void shutdown();

    void addEffect(std::shared_ptr<IEffect> effect);
    void removeEffect(std::shared_ptr<IEffect> effect);

    void process(const float* input, float* output, unsigned long frameCount);

    int getSampleRate() const;
    int getBlockSize() const;
    int getChannelCount() const;
    bool isInitialized() const;

private:
    int m_sampleRate;
    int m_blockSize;
    int m_numChannels;
    bool m_isInitialized;

    AudioBuffer m_inputBuffer;
    AudioBuffer m_outputBuffer;
    std::vector<std::shared_ptr<IEffect>> m_effects;
};

} // namespace VRMusicStudio 