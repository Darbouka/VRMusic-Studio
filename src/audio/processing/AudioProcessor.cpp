#include "audio/processing/AudioProcessor.hpp"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace VRMusicStudio {

AudioProcessor::AudioProcessor()
    : m_sampleRate(44100)
    , m_blockSize(1024)
    , m_numChannels(2)
    , m_isInitialized(false)
{
}

AudioProcessor::~AudioProcessor()
{
    if (m_isInitialized) {
        shutdown();
    }
}

bool AudioProcessor::initialize(int sampleRate, int blockSize, int numChannels)
{
    if (m_isInitialized) {
        return true;
    }

    m_sampleRate = sampleRate;
    m_blockSize = blockSize;
    m_numChannels = numChannels;

    m_inputBuffer.resize(numChannels, blockSize);
    m_outputBuffer.resize(numChannels, blockSize);

    m_isInitialized = true;
    spdlog::info("Audio processor initialized with sample rate: {}, block size: {}, channels: {}",
                 sampleRate, blockSize, numChannels);
    return true;
}

void AudioProcessor::shutdown()
{
    if (!m_isInitialized) {
        return;
    }

    m_effects.clear();
    m_isInitialized = false;
    spdlog::info("Audio processor shut down");
}

void AudioProcessor::addEffect(std::shared_ptr<IEffect> effect)
{
    if (!effect) {
        throw std::invalid_argument("Effect cannot be null");
    }
    m_effects.push_back(effect);
}

void AudioProcessor::removeEffect(std::shared_ptr<IEffect> effect)
{
    auto it = std::find(m_effects.begin(), m_effects.end(), effect);
    if (it != m_effects.end()) {
        m_effects.erase(it);
    }
}

void AudioProcessor::process(const float* input, float* output, unsigned long frameCount)
{
    if (!m_isInitialized) {
        throw std::runtime_error("Audio processor not initialized");
    }

    if (frameCount != static_cast<unsigned long>(m_blockSize)) {
        throw std::invalid_argument("Frame count must match block size");
    }

    // Kopiere Eingabedaten in den Eingabepuffer
    for (int channel = 0; channel < m_numChannels; ++channel) {
        float* channelData = m_inputBuffer.getChannelData(channel);
        for (int frame = 0; frame < m_blockSize; ++frame) {
            channelData[frame] = input[channel * m_blockSize + frame];
        }
    }

    // Wende Effekte an
    for (auto& effect : m_effects) {
        effect->process(m_inputBuffer);
    }

    // Kopiere verarbeitete Daten in den Ausgabepuffer
    for (int channel = 0; channel < m_numChannels; ++channel) {
        const float* channelData = m_inputBuffer.getChannelData(channel);
        for (int frame = 0; frame < m_blockSize; ++frame) {
            output[channel * m_blockSize + frame] = channelData[frame];
        }
    }
}

int AudioProcessor::getSampleRate() const
{
    return m_sampleRate;
}

int AudioProcessor::getBlockSize() const
{
    return m_blockSize;
}

int AudioProcessor::getChannelCount() const
{
    return m_numChannels;
}

bool AudioProcessor::isInitialized() const
{
    return m_isInitialized;
}

} // namespace VRMusicStudio 