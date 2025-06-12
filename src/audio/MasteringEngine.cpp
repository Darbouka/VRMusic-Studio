#include "MasteringEngine.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>

namespace VRMusicStudio {

struct MasteringEngine::Impl {
    // Mastering-Parameter
    float inputGain;
    float outputGain;
    float threshold;
    float ratio;
    float attack;
    float release;
    float knee;
    float makeupGain;
    
    // EQ-Parameter
    struct EQBand {
        float frequency;
        float gain;
        float q;
        bool enabled;
    };
    std::vector<EQBand> eqBands;
    
    // Limiter-Parameter
    float limiterThreshold;
    float limiterRelease;
    bool limiterEnabled;
    
    // Stereo-Parameter
    float stereoWidth;
    float midSideBalance;
    
    // Zustandsvariablen
    float currentLevel;
    float envelope;
    float lastSample;
    
    // Callbacks
    std::function<void(float)> levelCallback;
    std::function<void(const std::vector<float>&)> spectrumCallback;
    
    Impl() : inputGain(0.0f), outputGain(0.0f),
             threshold(-20.0f), ratio(4.0f),
             attack(0.01f), release(0.1f),
             knee(0.0f), makeupGain(0.0f),
             limiterThreshold(-1.0f), limiterRelease(0.01f),
             limiterEnabled(true), stereoWidth(1.0f),
             midSideBalance(0.0f), currentLevel(0.0f),
             envelope(0.0f), lastSample(0.0f) {
        // Initialisiere EQ-Bänder
        eqBands = {
            {60.0f, 0.0f, 1.0f, true},    // Sub
            {120.0f, 0.0f, 1.0f, true},   // Bass
            {250.0f, 0.0f, 1.0f, true},   // Low-Mid
            {500.0f, 0.0f, 1.0f, true},   // Mid
            {1000.0f, 0.0f, 1.0f, true},  // High-Mid
            {2000.0f, 0.0f, 1.0f, true},  // Presence
            {4000.0f, 0.0f, 1.0f, true},  // Brilliance
            {8000.0f, 0.0f, 1.0f, true}   // Air
        };
    }
};

MasteringEngine::MasteringEngine() : pImpl(std::make_unique<Impl>()) {}

MasteringEngine::~MasteringEngine() = default;

void MasteringEngine::setInputGain(float gain) {
    pImpl->inputGain = gain;
}

void MasteringEngine::setOutputGain(float gain) {
    pImpl->outputGain = gain;
}

void MasteringEngine::setCompressorParameters(float threshold, float ratio, float attack, float release, float knee) {
    pImpl->threshold = threshold;
    pImpl->ratio = ratio;
    pImpl->attack = attack;
    pImpl->release = release;
    pImpl->knee = knee;
}

void MasteringEngine::setMakeupGain(float gain) {
    pImpl->makeupGain = gain;
}

void MasteringEngine::setEQBand(int band, float frequency, float gain, float q) {
    if (band >= 0 && band < pImpl->eqBands.size()) {
        pImpl->eqBands[band].frequency = frequency;
        pImpl->eqBands[band].gain = gain;
        pImpl->eqBands[band].q = q;
    }
}

void MasteringEngine::setEQBandEnabled(int band, bool enabled) {
    if (band >= 0 && band < pImpl->eqBands.size()) {
        pImpl->eqBands[band].enabled = enabled;
    }
}

void MasteringEngine::setLimiterParameters(float threshold, float release) {
    pImpl->limiterThreshold = threshold;
    pImpl->limiterRelease = release;
}

void MasteringEngine::setLimiterEnabled(bool enabled) {
    pImpl->limiterEnabled = enabled;
}

void MasteringEngine::setStereoWidth(float width) {
    pImpl->stereoWidth = std::clamp(width, 0.0f, 2.0f);
}

void MasteringEngine::setMidSideBalance(float balance) {
    pImpl->midSideBalance = std::clamp(balance, -1.0f, 1.0f);
}

void MasteringEngine::processAudio(float* buffer, int numSamples) {
    for (int i = 0; i < numSamples; i += 2) {
        // Input Gain
        float left = buffer[i] * std::pow(10.0f, pImpl->inputGain / 20.0f);
        float right = buffer[i + 1] * std::pow(10.0f, pImpl->inputGain / 20.0f);
        
        // Stereo Processing
        float mid = (left + right) * 0.5f;
        float side = (left - right) * 0.5f;
        
        // Apply stereo width
        side *= pImpl->stereoWidth;
        
        // Apply mid-side balance
        mid *= (1.0f + pImpl->midSideBalance);
        side *= (1.0f - pImpl->midSideBalance);
        
        // Reconstruct stereo
        left = mid + side;
        right = mid - side;
        
        // EQ Processing
        for (const auto& band : pImpl->eqBands) {
            if (band.enabled) {
                // Implementiere hier die EQ-Verarbeitung
                // Dies ist eine vereinfachte Version
                float gain = std::pow(10.0f, band.gain / 20.0f);
                left *= gain;
                right *= gain;
            }
        }
        
        // Compressor
        float level = std::max(std::abs(left), std::abs(right));
        float gain = 1.0f;
        
        if (level > std::pow(10.0f, pImpl->threshold / 20.0f)) {
            float compression = 1.0f - (1.0f / pImpl->ratio);
            gain = 1.0f - compression;
        }
        
        // Attack/Release
        float attackCoeff = std::exp(-1.0f / (pImpl->attack * 44100.0f));
        float releaseCoeff = std::exp(-1.0f / (pImpl->release * 44100.0f));
        
        pImpl->envelope = std::max(level * (1.0f - attackCoeff) + pImpl->envelope * attackCoeff,
                                 level * (1.0f - releaseCoeff) + pImpl->envelope * releaseCoeff);
        
        // Apply compression
        left *= gain;
        right *= gain;
        
        // Limiter
        if (pImpl->limiterEnabled) {
            float limitGain = 1.0f;
            if (std::abs(left) > std::pow(10.0f, pImpl->limiterThreshold / 20.0f)) {
                limitGain = std::pow(10.0f, pImpl->limiterThreshold / 20.0f) / std::abs(left);
            }
            if (std::abs(right) > std::pow(10.0f, pImpl->limiterThreshold / 20.0f)) {
                limitGain = std::min(limitGain,
                                   std::pow(10.0f, pImpl->limiterThreshold / 20.0f) / std::abs(right));
            }
            
            left *= limitGain;
            right *= limitGain;
        }
        
        // Makeup Gain
        float makeup = std::pow(10.0f, pImpl->makeupGain / 20.0f);
        left *= makeup;
        right *= makeup;
        
        // Output Gain
        left *= std::pow(10.0f, pImpl->outputGain / 20.0f);
        right *= std::pow(10.0f, pImpl->outputGain / 20.0f);
        
        // Update buffer
        buffer[i] = left;
        buffer[i + 1] = right;
        
        // Update level meter
        pImpl->currentLevel = std::max(std::abs(left), std::abs(right));
        if (pImpl->levelCallback) {
            pImpl->levelCallback(pImpl->currentLevel);
        }
    }
    
    // Update spectrum analyzer
    if (pImpl->spectrumCallback) {
        // Implementiere hier die Spektrumanalyse
        // Dies ist eine vereinfachte Version
        std::vector<float> spectrum(1024, 0.0f);
        for (int i = 0; i < numSamples; i += 2) {
            int bin = static_cast<int>((i / 2) * 1024.0f / (numSamples / 2));
            if (bin < 1024) {
                spectrum[bin] = std::max(spectrum[bin],
                                       std::max(std::abs(buffer[i]),
                                               std::abs(buffer[i + 1])));
            }
        }
        pImpl->spectrumCallback(spectrum);
    }
}

void MasteringEngine::setLevelCallback(std::function<void(float)> callback) {
    pImpl->levelCallback = callback;
}

void MasteringEngine::setSpectrumCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->spectrumCallback = callback;
}

} // namespace VRMusicStudio 