#pragma once

#include <portaudio.h>
#include <cstddef>

namespace VRMusicStudio {

class AudioStream {
public:
    AudioStream();
    ~AudioStream();

    bool initialize();
    void shutdown();
    bool start();
    void stop();

    void setSampleRate(int sampleRate);
    void setBlockSize(int blockSize);
    void setChannelCount(int numChannels);

    bool process(const float* input, float* output, unsigned long frameCount);

    bool isInitialized() const { return m_isInitialized; }
    int getSampleRate() const { return m_sampleRate; }
    int getBlockSize() const { return m_blockSize; }
    int getChannelCount() const { return m_numChannels; }

private:
    PaStream* m_stream;
    int m_sampleRate;
    int m_blockSize;
    int m_numChannels;
    bool m_isInitialized;
};

} // namespace VRMusicStudio 