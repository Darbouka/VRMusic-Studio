#pragma once

#include <vector>
#include <string>
#include <functional>
#include <torch/script.h>

namespace VRMusicStudio {

class DrumMachine {
public:
    // Elektronische Drums Struktur
    struct ElectronicDrums {
        float kickVolume;
        float snareVolume;
        float hihatVolume;
        float tomVolume;
        float cymbalVolume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Akustische Drums Struktur
    struct AcousticDrums {
        float kickVolume;
        float snareVolume;
        float hihatVolume;
        float tomVolume;
        float cymbalVolume;
        float reverb;
        float delay;
        bool enabled;
    };

    // World Drums Struktur
    struct WorldDrums {
        float djembeVolume;
        float tablaVolume;
        float congaVolume;
        float bongoVolume;
        float talkingDrumVolume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Hybrid Drums Struktur
    struct HybridDrums {
        float electronicVolume;
        float acousticVolume;
        float worldVolume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Custom Drums Struktur
    struct CustomDrums {
        std::vector<float> volumes;
        float reverb;
        float delay;
        bool enabled;
    };

    DrumMachine();
    ~DrumMachine();

    // Elektronische Drums Methoden
    void setElectronicDrums(const ElectronicDrums& params);
    void processElectronicDrums(float* buffer, int numSamples);
    void setElectronicDrumsCallback(std::function<void(const std::vector<float>&)> callback);

    // Akustische Drums Methoden
    void setAcousticDrums(const AcousticDrums& params);
    void processAcousticDrums(float* buffer, int numSamples);
    void setAcousticDrumsCallback(std::function<void(const std::vector<float>&)> callback);

    // World Drums Methoden
    void setWorldDrums(const WorldDrums& params);
    void processWorldDrums(float* buffer, int numSamples);
    void setWorldDrumsCallback(std::function<void(const std::vector<float>&)> callback);

    // Hybrid Drums Methoden
    void setHybridDrums(const HybridDrums& params);
    void processHybridDrums(float* buffer, int numSamples);
    void setHybridDrumsCallback(std::function<void(const std::vector<float>&)> callback);

    // Custom Drums Methoden
    void setCustomDrums(const CustomDrums& params);
    void processCustomDrums(float* buffer, int numSamples);
    void setCustomDrumsCallback(std::function<void(const std::vector<float>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace VRMusicStudio 