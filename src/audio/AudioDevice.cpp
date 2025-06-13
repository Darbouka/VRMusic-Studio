#include "AudioDevice.hpp"
#include <stdexcept>
#include <portaudio.h>

namespace VRMusicStudio {

AudioDevice::AudioDevice()
    : m_sampleRate(44100.0)
    , m_blockSize(512)
    , m_numChannels(2)
    , m_isInitialized(false)
    , m_stream(nullptr)
{
    initialize();
}

AudioDevice::~AudioDevice()
{
    if (m_isInitialized) {
        Pa_Terminate();
    }
}

void AudioDevice::initialize()
{
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        throw std::runtime_error("Failed to initialize PortAudio: " + std::string(Pa_GetErrorText(err)));
    }
    m_isInitialized = true;
}

void AudioDevice::setSampleRate(double sampleRate)
{
    if (sampleRate <= 0.0) {
        throw std::invalid_argument("Sample rate must be positive");
    }
    m_sampleRate = sampleRate;
}

void AudioDevice::setBlockSize(int blockSize)
{
    if (blockSize <= 0) {
        throw std::invalid_argument("Block size must be positive");
    }
    m_blockSize = blockSize;
}

void AudioDevice::setChannelCount(int numChannels)
{
    if (numChannels <= 0) {
        throw std::invalid_argument("Channel count must be positive");
    }
    m_numChannels = numChannels;
}

void AudioDevice::start()
{
    if (!m_isInitialized) {
        throw std::runtime_error("Audio device not initialized");
    }

    PaStreamParameters inputParameters;
    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.channelCount = m_numChannels;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;

    PaStreamParameters outputParameters;
    outputParameters.device = Pa_GetDefaultOutputDevice();
    outputParameters.channelCount = m_numChannels;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;

    PaError err = Pa_OpenStream(&m_stream,
                              &inputParameters,
                              &outputParameters,
                              m_sampleRate,
                              m_blockSize,
                              paClipOff,
                              nullptr,
                              nullptr);

    if (err != paNoError) {
        throw std::runtime_error("Failed to open audio stream: " + std::string(Pa_GetErrorText(err)));
    }

    err = Pa_StartStream(m_stream);
    if (err != paNoError) {
        throw std::runtime_error("Failed to start audio stream: " + std::string(Pa_GetErrorText(err)));
    }
}

void AudioDevice::stop()
{
    if (m_stream) {
        PaError err = Pa_StopStream(m_stream);
        if (err != paNoError) {
            throw std::runtime_error("Failed to stop audio stream: " + std::string(Pa_GetErrorText(err)));
        }

        err = Pa_CloseStream(m_stream);
        if (err != paNoError) {
            throw std::runtime_error("Failed to close audio stream: " + std::string(Pa_GetErrorText(err)));
        }

        m_stream = nullptr;
    }
}

bool AudioDevice::isRunning() const
{
    return m_stream != nullptr && Pa_IsStreamActive(m_stream) == 1;
}

} // namespace VRMusicStudio 