#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <torch/torch.h>

class PercussionInstruments {
public:
    struct DrumKit {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Timpani {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Cymbals {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Marimba {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Xylophone {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Vibraphone {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Glockenspiel {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct TubularBells {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    PercussionInstruments();
    ~PercussionInstruments();

    void setDrumKitParams(const DrumKit& params);
    void setTimpaniParams(const Timpani& params);
    void setCymbalsParams(const Cymbals& params);
    void setMarimbaParams(const Marimba& params);
    void setXylophoneParams(const Xylophone& params);
    void setVibraphoneParams(const Vibraphone& params);
    void setGlockenspielParams(const Glockenspiel& params);
    void setTubularBellsParams(const TubularBells& params);

    void processDrumKit(std::vector<float>& buffer);
    void processTimpani(std::vector<float>& buffer);
    void processCymbals(std::vector<float>& buffer);
    void processMarimba(std::vector<float>& buffer);
    void processXylophone(std::vector<float>& buffer);
    void processVibraphone(std::vector<float>& buffer);
    void processGlockenspiel(std::vector<float>& buffer);
    void processTubularBells(std::vector<float>& buffer);

    void setDrumKitCallback(std::function<void(const std::vector<float>&)> callback);
    void setTimpaniCallback(std::function<void(const std::vector<float>&)> callback);
    void setCymbalsCallback(std::function<void(const std::vector<float>&)> callback);
    void setMarimbaCallback(std::function<void(const std::vector<float>&)> callback);
    void setXylophoneCallback(std::function<void(const std::vector<float>&)> callback);
    void setVibraphoneCallback(std::function<void(const std::vector<float>&)> callback);
    void setGlockenspielCallback(std::function<void(const std::vector<float>&)> callback);
    void setTubularBellsCallback(std::function<void(const std::vector<float>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; 