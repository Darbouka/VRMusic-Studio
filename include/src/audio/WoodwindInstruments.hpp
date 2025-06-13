#pragma once

#include <vector>
#include <string>
#include <functional>
#include <torch/script.h>

namespace VRMusicStudio {

class WoodwindInstruments {
public:
    // Querflöte Struktur
    struct TransverseFlute {
        float breathPressure;
        float vibratoDepth;
        float vibratoRate;
        float volume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Blockflöte Struktur
    struct Recorder {
        float breathPressure;
        float vibratoDepth;
        float vibratoRate;
        float volume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Saxophon Struktur
    struct Saxophone {
        float breathPressure;
        float vibratoDepth;
        float vibratoRate;
        float volume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Bassklarinette Struktur
    struct BassClarinet {
        float breathPressure;
        float vibratoDepth;
        float vibratoRate;
        float volume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Englischhorn Struktur
    struct EnglishHorn {
        float breathPressure;
        float vibratoDepth;
        float vibratoRate;
        float volume;
        float reverb;
        float delay;
        bool enabled;
    };

    WoodwindInstruments();
    ~WoodwindInstruments();

    // Querflöte Methoden
    void setTransverseFlute(const TransverseFlute& params);
    void processTransverseFlute(float* buffer, int numSamples);
    void setTransverseFluteCallback(std::function<void(const std::vector<float>&)> callback);

    // Blockflöte Methoden
    void setRecorder(const Recorder& params);
    void processRecorder(float* buffer, int numSamples);
    void setRecorderCallback(std::function<void(const std::vector<float>&)> callback);

    // Saxophon Methoden
    void setSaxophone(const Saxophone& params);
    void processSaxophone(float* buffer, int numSamples);
    void setSaxophoneCallback(std::function<void(const std::vector<float>&)> callback);

    // Bassklarinette Methoden
    void setBassClarinet(const BassClarinet& params);
    void processBassClarinet(float* buffer, int numSamples);
    void setBassClarinetCallback(std::function<void(const std::vector<float>&)> callback);

    // Englischhorn Methoden
    void setEnglishHorn(const EnglishHorn& params);
    void processEnglishHorn(float* buffer, int numSamples);
    void setEnglishHornCallback(std::function<void(const std::vector<float>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace VRMusicStudio 