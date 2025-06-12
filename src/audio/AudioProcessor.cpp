<<<<<<< HEAD
 
=======
#include "AudioProcessor.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>

namespace VRMusicStudio {

struct AudioProcessor::Impl {
    // Audio-Parameter
    float sampleRate;
    int bufferSize;
    int numChannels;
    
    // Effekt-Parameter
    float volume;
    float pan;
    float pitch;
    float reverb;
    float delay;
    float distortion;
    
    // Zustand
    bool isProcessing;
    std::vector<float> buffer;
    
    Impl() : sampleRate(44100.0f), bufferSize(1024), numChannels(2),
             volume(1.0f), pan(0.0f), pitch(1.0f), reverb(0.0f),
             delay(0.0f), distortion(0.0f), isProcessing(false) {
        buffer.resize(bufferSize * numChannels);
    }
};

AudioProcessor::AudioProcessor() : pImpl(std::make_unique<Impl>()) {}

AudioProcessor::~AudioProcessor() = default;

bool AudioProcessor::initialize(float sampleRate, int bufferSize, int numChannels) {
    pImpl->sampleRate = sampleRate;
    pImpl->bufferSize = bufferSize;
    pImpl->numChannels = numChannels;
    pImpl->buffer.resize(bufferSize * numChannels);
    
    return true;
}

void AudioProcessor::process(float* input, float* output, int numFrames) {
    if (!pImpl->isProcessing) {
        return;
    }
    
    // Kopiere Input in Buffer
    std::copy(input, input + numFrames * pImpl->numChannels, pImpl->buffer.begin());
    
    // Wende Effekte an
    applyVolume(pImpl->buffer.data(), numFrames);
    applyPan(pImpl->buffer.data(), numFrames);
    applyPitch(pImpl->buffer.data(), numFrames);
    applyReverb(pImpl->buffer.data(), numFrames);
    applyDelay(pImpl->buffer.data(), numFrames);
    applyDistortion(pImpl->buffer.data(), numFrames);
    
    // Kopiere verarbeitete Daten in Output
    std::copy(pImpl->buffer.begin(), pImpl->buffer.begin() + numFrames * pImpl->numChannels, output);
}

void AudioProcessor::setVolume(float volume) {
    pImpl->volume = std::clamp(volume, 0.0f, 1.0f);
}

void AudioProcessor::setPan(float pan) {
    pImpl->pan = std::clamp(pan, -1.0f, 1.0f);
}

void AudioProcessor::setPitch(float pitch) {
    pImpl->pitch = std::clamp(pitch, 0.5f, 2.0f);
}

void AudioProcessor::setReverb(float reverb) {
    pImpl->reverb = std::clamp(reverb, 0.0f, 1.0f);
}

void AudioProcessor::setDelay(float delay) {
    pImpl->delay = std::clamp(delay, 0.0f, 1.0f);
}

void AudioProcessor::setDistortion(float distortion) {
    pImpl->distortion = std::clamp(distortion, 0.0f, 1.0f);
}

void AudioProcessor::startProcessing() {
    pImpl->isProcessing = true;
}

void AudioProcessor::stopProcessing() {
    pImpl->isProcessing = false;
}

void AudioProcessor::applyVolume(float* buffer, int numFrames) {
    for (int i = 0; i < numFrames * pImpl->numChannels; ++i) {
        buffer[i] *= pImpl->volume;
    }
}

void AudioProcessor::applyPan(float* buffer, int numFrames) {
    for (int frame = 0; frame < numFrames; ++frame) {
        float leftGain = 1.0f - std::max(0.0f, pImpl->pan);
        float rightGain = 1.0f + std::min(0.0f, pImpl->pan);
        
        buffer[frame * pImpl->numChannels] *= leftGain;
        buffer[frame * pImpl->numChannels + 1] *= rightGain;
    }
}

void AudioProcessor::applyPitch(float* buffer, int numFrames) {
    // Einfache Pitch-Shifting-Implementierung
    std::vector<float> tempBuffer(buffer, buffer + numFrames * pImpl->numChannels);
    
    for (int frame = 0; frame < numFrames; ++frame) {
        float sourceFrame = frame * pImpl->pitch;
        int sourceFrameInt = static_cast<int>(sourceFrame);
        float fraction = sourceFrame - sourceFrameInt;
        
        if (sourceFrameInt + 1 < numFrames) {
            for (int channel = 0; channel < pImpl->numChannels; ++channel) {
                float a = tempBuffer[sourceFrameInt * pImpl->numChannels + channel];
                float b = tempBuffer[(sourceFrameInt + 1) * pImpl->numChannels + channel];
                buffer[frame * pImpl->numChannels + channel] = a + fraction * (b - a);
            }
        }
    }
}

void AudioProcessor::applyReverb(float* buffer, int numFrames) {
    // Einfache Reverb-Implementierung
    static std::vector<float> delayBuffer(44100); // 1 Sekunde Verzögerung
    static int delayIndex = 0;
    
    for (int frame = 0; frame < numFrames; ++frame) {
        for (int channel = 0; channel < pImpl->numChannels; ++channel) {
            int index = frame * pImpl->numChannels + channel;
            float delayed = delayBuffer[delayIndex];
            buffer[index] = buffer[index] + pImpl->reverb * delayed;
            delayBuffer[delayIndex] = buffer[index];
        }
        delayIndex = (delayIndex + 1) % delayBuffer.size();
    }
}

void AudioProcessor::applyDelay(float* buffer, int numFrames) {
    // Einfache Delay-Implementierung
    static std::vector<float> delayBuffer(44100); // 1 Sekunde Verzögerung
    static int delayIndex = 0;
    
    for (int frame = 0; frame < numFrames; ++frame) {
        for (int channel = 0; channel < pImpl->numChannels; ++channel) {
            int index = frame * pImpl->numChannels + channel;
            float delayed = delayBuffer[delayIndex];
            buffer[index] = buffer[index] + pImpl->delay * delayed;
            delayBuffer[delayIndex] = buffer[index];
        }
        delayIndex = (delayIndex + 1) % delayBuffer.size();
    }
}

void AudioProcessor::applyDistortion(float* buffer, int numFrames) {
    // Einfache Distortion-Implementierung
    for (int i = 0; i < numFrames * pImpl->numChannels; ++i) {
        float sample = buffer[i];
        sample = std::tanh(sample * (1.0f + pImpl->distortion * 10.0f));
        buffer[i] = sample;
    }
}

} // namespace VRMusicStudio 
>>>>>>> 0dff1c4 (init 2)
