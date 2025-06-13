#pragma once

#include <portaudio.h>
#include <string>

namespace VRMusicStudio {

class AudioDevice {
public:
    AudioDevice();
    ~AudioDevice();
    
    void setSampleRate(double sampleRate);
    void setBlockSize(int blockSize);
    void setChannelCount(int numChannels);
    
    void start();
    void stop();
    bool isRunning() const;

private:
    void initialize();
    
    double m_sampleRate;
    int m_blockSize;
    int m_numChannels;
    bool m_isInitialized;
    PaStream* m_stream;
};

} // namespace VRMusicStudio 