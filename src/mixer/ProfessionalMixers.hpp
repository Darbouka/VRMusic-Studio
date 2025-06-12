#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <torch/torch.h>

class ProfessionalMixers {
public:
    // SSL Mischpulte
    struct SSL4000G {
        float volume;
        float pan;
        float eqLow;
        float eqMid;
        float eqHigh;
        float compression;
        float reverb;
        float delay;
        bool enabled;
    };

    struct SSL9000J {
        float volume;
        float pan;
        float eqLow;
        float eqMid;
        float eqHigh;
        float compression;
        float reverb;
        float delay;
        bool enabled;
    };

    // Neve Mischpulte
    struct Neve88RS {
        float volume;
        float pan;
        float eqLow;
        float eqMid;
        float eqHigh;
        float compression;
        float reverb;
        float delay;
        bool enabled;
    };

    struct NeveVR {
        float volume;
        float pan;
        float eqLow;
        float eqMid;
        float eqHigh;
        float compression;
        float reverb;
        float delay;
        bool enabled;
    };

    // API Mischpulte
    struct API1608 {
        float volume;
        float pan;
        float eqLow;
        float eqMid;
        float eqHigh;
        float compression;
        float reverb;
        float delay;
        bool enabled;
    };

    struct API2448 {
        float volume;
        float pan;
        float eqLow;
        float eqMid;
        float eqHigh;
        float compression;
        float reverb;
        float delay;
        bool enabled;
    };

    // Studer Mischpulte
    struct StuderA800 {
        float volume;
        float pan;
        float eqLow;
        float eqMid;
        float eqHigh;
        float compression;
        float reverb;
        float delay;
        bool enabled;
    };

    struct StuderJ37 {
        float volume;
        float pan;
        float eqLow;
        float eqMid;
        float eqHigh;
        float compression;
        float reverb;
        float delay;
        bool enabled;
    };

    // Midas Mischpulte
    struct MidasXL4 {
        float volume;
        float pan;
        float eqLow;
        float eqMid;
        float eqHigh;
        float compression;
        float reverb;
        float delay;
        bool enabled;
    };

    struct MidasHeritage {
        float volume;
        float pan;
        float eqLow;
        float eqMid;
        float eqHigh;
        float compression;
        float reverb;
        float delay;
        bool enabled;
    };

    ProfessionalMixers();
    ~ProfessionalMixers();

    // Parameter-Setter für alle Mischpulte
    void setSSL4000GParams(const SSL4000G& params);
    void setSSL9000JParams(const SSL9000J& params);
    void setNeve88RSParams(const Neve88RS& params);
    void setNeveVRParams(const NeveVR& params);
    void setAPI1608Params(const API1608& params);
    void setAPI2448Params(const API2448& params);
    void setStuderA800Params(const StuderA800& params);
    void setStuderJ37Params(const StuderJ37& params);
    void setMidasXL4Params(const MidasXL4& params);
    void setMidasHeritageParams(const MidasHeritage& params);

    // Audio-Verarbeitung für alle Mischpulte
    void processSSL4000G(std::vector<float>& buffer);
    void processSSL9000J(std::vector<float>& buffer);
    void processNeve88RS(std::vector<float>& buffer);
    void processNeveVR(std::vector<float>& buffer);
    void processAPI1608(std::vector<float>& buffer);
    void processAPI2448(std::vector<float>& buffer);
    void processStuderA800(std::vector<float>& buffer);
    void processStuderJ37(std::vector<float>& buffer);
    void processMidasXL4(std::vector<float>& buffer);
    void processMidasHeritage(std::vector<float>& buffer);

    // Callback-Setter für alle Mischpulte
    void setSSL4000GCallback(std::function<void(const std::vector<float>&)> callback);
    void setSSL900JCallback(std::function<void(const std::vector<float>&)> callback);
    void setNeve88RSCallback(std::function<void(const std::vector<float>&)> callback);
    void setNeveVRCallback(std::function<void(const std::vector<float>&)> callback);
    void setAPI1608Callback(std::function<void(const std::vector<float>&)> callback);
    void setAPI2448Callback(std::function<void(const std::vector<float>&)> callback);
    void setStuderA800Callback(std::function<void(const std::vector<float>&)> callback);
    void setStuderJ37Callback(std::function<void(const std::vector<float>&)> callback);
    void setMidasXL4Callback(std::function<void(const std::vector<float>&)> callback);
    void setMidasHeritageCallback(std::function<void(const std::vector<float>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; 