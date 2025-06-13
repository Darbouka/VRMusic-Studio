#include "audio/AudioStream.hpp"
#include <spdlog/spdlog.h>
#include <portaudio.h>
#include <stdexcept>

namespace VRMusicStudio {

AudioStream::AudioStream()
    : m_stream(nullptr)
    , m_sampleRate(44100)
    , m_blockSize(1024)
    , m_numChannels(2)
    , m_isInitialized(false)
{
}

AudioStream::~AudioStream()
{
    if (m_isInitialized) {
        stop();
    }
}

bool AudioStream::initialize()
{
    if (m_isInitialized) {
        return true;
    }

    PaError err = Pa_Initialize();
    if (err != paNoError) {
        spdlog::error("Failed to initialize PortAudio: {}", Pa_GetErrorText(err));
        return false;
    }

    PaStreamParameters inputParameters;
    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.channelCount = m_numChannels;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    PaStreamParameters outputParameters;
    outputParameters.device = Pa_GetDefaultOutputDevice();
    outputParameters.channelCount = m_numChannels;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(&m_stream,
                       &inputParameters,
                       &outputParameters,
                       m_sampleRate,
                       m_blockSize,
                       paClipOff,
                       nullptr,
                       nullptr);

    if (err != paNoError) {
        spdlog::error("Failed to open audio stream: {}", Pa_GetErrorText(err));
        Pa_Terminate();
        return false;
    }

    m_isInitialized = true;
    spdlog::info("Audio stream initialized successfully");
    return true;
}

void AudioStream::shutdown()
{
    if (!m_isInitialized) {
        return;
    }

    stop();

    if (m_stream) {
        Pa_CloseStream(m_stream);
        m_stream = nullptr;
    }

    Pa_Terminate();
    m_isInitialized = false;
    spdlog::info("Audio stream shut down");
}

bool AudioStream::start()
{
    if (!m_isInitialized || !m_stream) {
        return false;
    }

    PaError err = Pa_StartStream(m_stream);
    if (err != paNoError) {
        spdlog::error("Failed to start audio stream: {}", Pa_GetErrorText(err));
        return false;
    }

    spdlog::info("Audio stream started");
    return true;
}

void AudioStream::stop()
{
    if (!m_isInitialized || !m_stream) {
        return;
    }

    if (Pa_IsStreamActive(m_stream)) {
        PaError err = Pa_StopStream(m_stream);
        if (err != paNoError) {
            spdlog::error("Failed to stop audio stream: {}", Pa_GetErrorText(err));
        }
    }
}

void AudioStream::setSampleRate(int sampleRate)
{
    if (m_isInitialized) {
        throw std::runtime_error("Cannot change sample rate while stream is initialized");
    }
    m_sampleRate = sampleRate;
}

void AudioStream::setBlockSize(int blockSize)
{
    if (m_isInitialized) {
        throw std::runtime_error("Cannot change block size while stream is initialized");
    }
    m_blockSize = blockSize;
}

void AudioStream::setChannelCount(int numChannels)
{
    if (m_isInitialized) {
        throw std::runtime_error("Cannot change channel count while stream is initialized");
    }
    m_numChannels = numChannels;
}

bool AudioStream::process(const float* input, float* output, unsigned long frameCount)
{
    if (!m_isInitialized || !m_stream) {
        return false;
    }

    // Hier können Sie die Audio-Verarbeitung implementieren
    // Für jetzt kopieren wir einfach die Eingabe in die Ausgabe
    std::memcpy(output, input, frameCount * m_numChannels * sizeof(float));
    return true;
}

} // namespace VRMusicStudio 