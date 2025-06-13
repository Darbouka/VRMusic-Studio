#include "EffectEngine.hpp"
#include <algorithm>
#include <stdexcept>

namespace VRMusicStudio {

EffectEngine::EffectEngine()
    : m_sampleRate(44100.0)
    , m_blockSize(512)
    , m_numChannels(2)
    , m_isProcessing(false)
{
}

void EffectEngine::setSampleRate(double sampleRate)
{
    if (sampleRate <= 0.0) {
        throw std::invalid_argument("Sample rate must be positive");
    }
    m_sampleRate = sampleRate;
    updateEffects();
}

void EffectEngine::setBlockSize(int blockSize)
{
    if (blockSize <= 0) {
        throw std::invalid_argument("Block size must be positive");
    }
    m_blockSize = blockSize;
    updateEffects();
}

void EffectEngine::setChannelCount(int numChannels)
{
    if (numChannels <= 0) {
        throw std::invalid_argument("Channel count must be positive");
    }
    m_numChannels = numChannels;
    updateEffects();
}

void EffectEngine::process(float* buffer, int numFrames)
{
    if (!m_isProcessing) {
        return;
    }

    // Process through effect chain
    for (auto& effect : m_effects) {
        effect->process(buffer, numFrames);
    }
}

void EffectEngine::addEffect(std::shared_ptr<AudioEffect> effect)
{
    if (!effect) {
        throw std::invalid_argument("Effect cannot be null");
    }
    m_effects.push_back(effect);
    updateEffect(effect);
}

void EffectEngine::removeEffect(std::shared_ptr<AudioEffect> effect)
{
    auto it = std::find(m_effects.begin(), m_effects.end(), effect);
    if (it != m_effects.end()) {
        m_effects.erase(it);
    }
}

void EffectEngine::startProcessing()
{
    m_isProcessing = true;
}

void EffectEngine::stopProcessing()
{
    m_isProcessing = false;
}

bool EffectEngine::isProcessing() const
{
    return m_isProcessing;
}

void EffectEngine::updateEffects()
{
    for (auto& effect : m_effects) {
        updateEffect(effect);
    }
}

void EffectEngine::updateEffect(std::shared_ptr<AudioEffect> effect)
{
    if (effect) {
        effect->setSampleRate(m_sampleRate);
        effect->setBlockSize(m_blockSize);
        effect->setChannelCount(m_numChannels);
    }
}

} // namespace VRMusicStudio 