#pragma once

#include <vector>
#include <string>
#include <functional>
#include <torch/script.h>

namespace VRMusicStudio {

class StringInstruments {
public:
    // Violine Struktur
    struct Violin {
        float bowPressure;
        float bowSpeed;
        float vibratoDepth;
        float vibratoRate;
        float volume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Viola Struktur
    struct Viola {
        float bowPressure;
        float bowSpeed;
        float vibratoDepth;
        float vibratoRate;
        float volume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Cello Struktur
    struct Cello {
        float bowPressure;
        float bowSpeed;
        float vibratoDepth;
        float vibratoRate;
        float volume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Kontrabass Struktur
    struct DoubleBass {
        float bowPressure;
        float bowSpeed;
        float vibratoDepth;
        float vibratoRate;
        float volume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Streichorchester Struktur
    struct StringOrchestra {
        float violinVolume;
        float violaVolume;
        float celloVolume;
        float bassVolume;
        float reverb;
        float delay;
        bool enabled;
    };

    StringInstruments();
    ~StringInstruments();

    // Violine Methoden
    void setViolin(const Violin& params);
    void processViolin(float* buffer, int numSamples);
    void setViolinCallback(std::function<void(const std::vector<float>&)> callback);

    // Viola Methoden
    void setViola(const Viola& params);
    void processViola(float* buffer, int numSamples);
    void setViolaCallback(std::function<void(const std::vector<float>&)> callback);

    // Cello Methoden
    void setCello(const Cello& params);
    void processCello(float* buffer, int numSamples);
    void setCelloCallback(std::function<void(const std::vector<float>&)> callback);

    // Kontrabass Methoden
    void setDoubleBass(const DoubleBass& params);
    void processDoubleBass(float* buffer, int numSamples);
    void setDoubleBassCallback(std::function<void(const std::vector<float>&)> callback);

    // Streichorchester Methoden
    void setStringOrchestra(const StringOrchestra& params);
    void processStringOrchestra(float* buffer, int numSamples);
    void setStringOrchestraCallback(std::function<void(const std::vector<float>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace VRMusicStudio 