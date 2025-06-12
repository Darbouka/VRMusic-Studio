#pragma once

#include <vector>
#include <string>
#include <functional>
#include <torch/script.h>

namespace VRMusicStudio {

class WindInstruments {
public:
    // Flöte Struktur
    struct Flute {
        float breathPressure;
        float vibratoDepth;
        float vibratoRate;
        float volume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Oboe Struktur
    struct Oboe {
        float breathPressure;
        float vibratoDepth;
        float vibratoRate;
        float volume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Klarinette Struktur
    struct Clarinet {
        float breathPressure;
        float vibratoDepth;
        float vibratoRate;
        float volume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Fagott Struktur
    struct Bassoon {
        float breathPressure;
        float vibratoDepth;
        float vibratoRate;
        float volume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Horn Struktur
    struct Horn {
        float breathPressure;
        float vibratoDepth;
        float vibratoRate;
        float volume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Trompete Struktur
    struct Trumpet {
        float breathPressure;
        float vibratoDepth;
        float vibratoRate;
        float volume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Posaune Struktur
    struct Trombone {
        float breathPressure;
        float vibratoDepth;
        float vibratoRate;
        float volume;
        float reverb;
        float delay;
        bool enabled;
    };

    // Tuba Struktur
    struct Tuba {
        float breathPressure;
        float vibratoDepth;
        float vibratoRate;
        float volume;
        float reverb;
        float delay;
        bool enabled;
    };

    WindInstruments();
    ~WindInstruments();

    // Flöte Methoden
    void setFlute(const Flute& params);
    void processFlute(float* buffer, int numSamples);
    void setFluteCallback(std::function<void(const std::vector<float>&)> callback);

    // Oboe Methoden
    void setOboe(const Oboe& params);
    void processOboe(float* buffer, int numSamples);
    void setOboeCallback(std::function<void(const std::vector<float>&)> callback);

    // Klarinette Methoden
    void setClarinet(const Clarinet& params);
    void processClarinet(float* buffer, int numSamples);
    void setClarinetCallback(std::function<void(const std::vector<float>&)> callback);

    // Fagott Methoden
    void setBassoon(const Bassoon& params);
    void processBassoon(float* buffer, int numSamples);
    void setBassoonCallback(std::function<void(const std::vector<float>&)> callback);

    // Horn Methoden
    void setHorn(const Horn& params);
    void processHorn(float* buffer, int numSamples);
    void setHornCallback(std::function<void(const std::vector<float>&)> callback);

    // Trompete Methoden
    void setTrumpet(const Trumpet& params);
    void processTrumpet(float* buffer, int numSamples);
    void setTrumpetCallback(std::function<void(const std::vector<float>&)> callback);

    // Posaune Methoden
    void setTrombone(const Trombone& params);
    void processTrombone(float* buffer, int numSamples);
    void setTromboneCallback(std::function<void(const std::vector<float>&)> callback);

    // Tuba Methoden
    void setTuba(const Tuba& params);
    void processTuba(float* buffer, int numSamples);
    void setTubaCallback(std::function<void(const std::vector<float>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace VRMusicStudio 