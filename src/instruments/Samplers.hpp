#pragma once

#include <vector>
#include <string>
#include <functional>
#include <torch/script.h>

namespace VRMusicStudio {

class Samplers {
public:
    // Multi-Sampler Struktur
    struct MultiSampler {
        float sampleRate;
        std::vector<float> loopPoints;
        float pitch;
        float pan;
        float volume;
        bool enabled;
    };

    // Drum-Sampler Struktur
    struct DrumSampler {
        float sampleRate;
        float pitch;
        float pan;
        float volume;
        bool enabled;
    };

    // Loop-Sampler Struktur
    struct LoopSampler {
        float sampleRate;
        std::vector<float> loopPoints;
        float pitch;
        float pan;
        float volume;
        bool enabled;
    };

    // Granular-Sampler Struktur
    struct GranularSampler {
        float sampleRate;
        float grainSize;
        float grainDensity;
        float pitch;
        float pan;
        float volume;
        bool enabled;
    };

    // Time-Stretch-Sampler Struktur
    struct TimeStretchSampler {
        float sampleRate;
        float timeStretch;
        float pitch;
        float pan;
        float volume;
        bool enabled;
    };

    Samplers();
    ~Samplers();

    // Multi-Sampler Methoden
    void setMultiSampler(const MultiSampler& params);
    void processMultiSampler(float* buffer, int numSamples);
    void setMultiSamplerCallback(std::function<void(const std::vector<float>&)> callback);

    // Drum-Sampler Methoden
    void setDrumSampler(const DrumSampler& params);
    void processDrumSampler(float* buffer, int numSamples);
    void setDrumSamplerCallback(std::function<void(const std::vector<float>&)> callback);

    // Loop-Sampler Methoden
    void setLoopSampler(const LoopSampler& params);
    void processLoopSampler(float* buffer, int numSamples);
    void setLoopSamplerCallback(std::function<void(const std::vector<float>&)> callback);

    // Granular-Sampler Methoden
    void setGranularSampler(const GranularSampler& params);
    void processGranularSampler(float* buffer, int numSamples);
    void setGranularSamplerCallback(std::function<void(const std::vector<float>&)> callback);

    // Time-Stretch-Sampler Methoden
    void setTimeStretchSampler(const TimeStretchSampler& params);
    void processTimeStretchSampler(float* buffer, int numSamples);
    void setTimeStretchSamplerCallback(std::function<void(const std::vector<float>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace VRMusicStudio 