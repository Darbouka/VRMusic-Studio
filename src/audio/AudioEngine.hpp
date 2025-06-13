#pragma once

#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <portaudio.h>
#include <jack/jack.h>
#include <functional>

namespace VRMusicStudio {
namespace Audio {

class AudioEngine {
public:
    static AudioEngine& getInstance();

    AudioEngine(const AudioEngine&) = delete;
    AudioEngine& operator=(const AudioEngine&) = delete;
    AudioEngine(AudioEngine&&) = delete;
    AudioEngine& operator=(AudioEngine&&) = delete;

    bool initialize();
    void shutdown();
    void update();

    bool startStream();
    void stopStream();
    bool isStreamActive() const;

    void setSampleRate(double sampleRate);
    double getSampleRate() const;

    void setBufferSize(unsigned int bufferSize);
    unsigned int getBufferSize() const;

    void setInputDevice(int deviceId);
    void setOutputDevice(int deviceId);
    int getInputDevice() const;
    int getOutputDevice() const;

    std::vector<std::string> getAvailableInputDevices() const;
    std::vector<std::string> getAvailableOutputDevices() const;

    // Audio-Stream-Management
    bool createStream(const std::string& name, int sampleRate, int channels);
    void destroyStream(const std::string& name);
    
    // Audio-Verarbeitung
    void processAudio(float* buffer, int numFrames);
    void setMasterVolume(float volume);
    float getMasterVolume() const;

    // Effekt-Management
    bool addEffect(const std::string& streamName, const std::string& effectName);
    void removeEffect(const std::string& streamName, const std::string& effectName);

    // MIDI-Integration
    bool handleMIDIEvent(const uint8_t* data, size_t length);
    void setMIDICallback(std::function<void(const uint8_t*, size_t)> callback);

private:
    AudioEngine();
    ~AudioEngine();

    static int paCallback(const void* inputBuffer, void* outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void* userData);

    static int jackCallback(jack_nframes_t nframes, void* arg);

    void initializePortAudio();
    void initializeJACK();
    void shutdownPortAudio();
    void shutdownJACK();

    bool initialized;
    bool streamActive;
    double sampleRate;
    unsigned int bufferSize;
    int inputDevice;
    int outputDevice;

    PaStream* paStream;
    jack_client_t* jackClient;
    jack_port_t* jackInputPort;
    jack_port_t* jackOutputPort;

    mutable std::mutex mutex;

    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace Audio
} // namespace VRMusicStudio 