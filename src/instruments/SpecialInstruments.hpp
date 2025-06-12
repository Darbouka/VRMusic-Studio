#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <torch/torch.h>

class SpecialInstruments {
public:
    // Hang Drum
    struct HangDrum {
        float volume;
        float pan;
        float reverb;
        float delay;
        float resonance;
        float sustain;
        bool enabled;
    };

    // Handpan
    struct Handpan {
        float volume;
        float pan;
        float reverb;
        float delay;
        float resonance;
        float sustain;
        bool enabled;
    };

    // Rav Vast
    struct RavVast {
        float volume;
        float pan;
        float reverb;
        float delay;
        float resonance;
        float sustain;
        bool enabled;
    };

    // Halo Handpan
    struct HaloHandpan {
        float volume;
        float pan;
        float reverb;
        float delay;
        float resonance;
        float sustain;
        bool enabled;
    };

    // Aura Handpan
    struct AuraHandpan {
        float volume;
        float pan;
        float reverb;
        float delay;
        float resonance;
        float sustain;
        bool enabled;
    };

    SpecialInstruments();
    ~SpecialInstruments();

    // Parameter-Setter für alle Instrumente
    void setHangDrumParams(const HangDrum& params);
    void setHandpanParams(const Handpan& params);
    void setRavVastParams(const RavVast& params);
    void setHaloHandpanParams(const HaloHandpan& params);
    void setAuraHandpanParams(const AuraHandpan& params);

    // Audio-Verarbeitung für alle Instrumente
    void processHangDrum(std::vector<float>& buffer);
    void processHandpan(std::vector<float>& buffer);
    void processRavVast(std::vector<float>& buffer);
    void processHaloHandpan(std::vector<float>& buffer);
    void processAuraHandpan(std::vector<float>& buffer);

    // Callback-Setter für alle Instrumente
    void setHangDrumCallback(std::function<void(const std::vector<float>&)> callback);
    void setHandpanCallback(std::function<void(const std::vector<float>&)> callback);
    void setRavVastCallback(std::function<void(const std::vector<float>&)> callback);
    void setHaloHandpanCallback(std::function<void(const std::vector<float>&)> callback);
    void setAuraHandpanCallback(std::function<void(const std::vector<float>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; 