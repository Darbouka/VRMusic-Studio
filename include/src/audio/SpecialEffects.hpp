#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

namespace VRMusicStudio {

class SpecialEffects {
public:
    // Quanten-Resonanz-Effekt
    struct QuantumResonance {
        float entanglement;
        float superposition;
        float decoherence;
        float quantumTunneling;
        bool enabled;
    };

    // Neural-Space-Effekt
    struct NeuralSpace {
        float spatialDepth;
        float neuralDensity;
        float consciousness;
        float dreamState;
        bool enabled;
    };

    // Bio-Feedback-Effekt
    struct BioFeedback {
        float heartRate;
        float brainWaves;
        float emotionalState;
        float physicalResponse;
        bool enabled;
    };

    // Holographic-Sound-Effekt
    struct HolographicSound {
        float dimension;
        float projection;
        float interference;
        float stability;
        bool enabled;
    };

    // Time-Warp-Effekt
    struct TimeWarp {
        float timeDilation;
        float causality;
        float paradox;
        float temporalShift;
        bool enabled;
    };

    // Reality-Bend-Effekt
    struct RealityBend {
        float realityDistortion;
        float dimensionShift;
        float perception;
        float consciousness;
        bool enabled;
    };

    SpecialEffects();
    ~SpecialEffects();

    // Quanten-Resonanz
    void setQuantumResonance(const QuantumResonance& params);
    void processQuantumResonance(float* buffer, int numSamples);

    // Neural-Space
    void setNeuralSpace(const NeuralSpace& params);
    void processNeuralSpace(float* buffer, int numSamples);

    // Bio-Feedback
    void setBioFeedback(const BioFeedback& params);
    void processBioFeedback(float* buffer, int numSamples);

    // Holographic-Sound
    void setHolographicSound(const HolographicSound& params);
    void processHolographicSound(float* buffer, int numSamples);

    // Time-Warp
    void setTimeWarp(const TimeWarp& params);
    void processTimeWarp(float* buffer, int numSamples);

    // Reality-Bend
    void setRealityBend(const RealityBend& params);
    void processRealityBend(float* buffer, int numSamples);

    // Callback-Setter
    void setQuantumCallback(std::function<void(const std::vector<float>&)> callback);
    void setNeuralCallback(std::function<void(const std::vector<float>&)> callback);
    void setBioCallback(std::function<void(const std::vector<float>&)> callback);
    void setHolographicCallback(std::function<void(const std::vector<float>&)> callback);
    void setTimeWarpCallback(std::function<void(const std::vector<float>&)> callback);
    void setRealityBendCallback(std::function<void(const std::vector<float>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; 