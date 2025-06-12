#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <torch/torch.h>

class WorldInstruments {
public:
    // Asiatische Instrumente
    struct Koto {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Shakuhachi {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Erhu {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Pipa {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    // Afrikanische Instrumente
    struct Djembe {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Kora {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Balafon {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    // Indische Instrumente
    struct Sitar {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Tabla {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Sarod {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    // Arabische Instrumente
    struct Oud {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Ney {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Darbuka {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    // Lateinamerikanische Instrumente
    struct Charango {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Quena {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Cajon {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    // Keltische Instrumente
    struct UilleannPipes {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Bodhran {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct CelticHarp {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    WorldInstruments();
    ~WorldInstruments();

    // Parameter-Setter für alle Instrumente
    void setKotoParams(const Koto& params);
    void setShakuhachiParams(const Shakuhachi& params);
    void setErhuParams(const Erhu& params);
    void setPipaParams(const Pipa& params);
    void setDjembeParams(const Djembe& params);
    void setKoraParams(const Kora& params);
    void setBalafonParams(const Balafon& params);
    void setSitarParams(const Sitar& params);
    void setTablaParams(const Tabla& params);
    void setSarodParams(const Sarod& params);
    void setOudParams(const Oud& params);
    void setNeyParams(const Ney& params);
    void setDarbukaParams(const Darbuka& params);
    void setCharangoParams(const Charango& params);
    void setQuenaParams(const Quena& params);
    void setCajonParams(const Cajon& params);
    void setUilleannPipesParams(const UilleannPipes& params);
    void setBodhranParams(const Bodhran& params);
    void setCelticHarpParams(const CelticHarp& params);

    // Audio-Verarbeitung für alle Instrumente
    void processKoto(std::vector<float>& buffer);
    void processShakuhachi(std::vector<float>& buffer);
    void processErhu(std::vector<float>& buffer);
    void processPipa(std::vector<float>& buffer);
    void processDjembe(std::vector<float>& buffer);
    void processKora(std::vector<float>& buffer);
    void processBalafon(std::vector<float>& buffer);
    void processSitar(std::vector<float>& buffer);
    void processTabla(std::vector<float>& buffer);
    void processSarod(std::vector<float>& buffer);
    void processOud(std::vector<float>& buffer);
    void processNey(std::vector<float>& buffer);
    void processDarbuka(std::vector<float>& buffer);
    void processCharango(std::vector<float>& buffer);
    void processQuena(std::vector<float>& buffer);
    void processCajon(std::vector<float>& buffer);
    void processUilleannPipes(std::vector<float>& buffer);
    void processBodhran(std::vector<float>& buffer);
    void processCelticHarp(std::vector<float>& buffer);

    // Callback-Setter für alle Instrumente
    void setKotoCallback(std::function<void(const std::vector<float>&)> callback);
    void setShakuhachiCallback(std::function<void(const std::vector<float>&)> callback);
    void setErhuCallback(std::function<void(const std::vector<float>&)> callback);
    void setPipaCallback(std::function<void(const std::vector<float>&)> callback);
    void setDjembeCallback(std::function<void(const std::vector<float>&)> callback);
    void setKoraCallback(std::function<void(const std::vector<float>&)> callback);
    void setBalafonCallback(std::function<void(const std::vector<float>&)> callback);
    void setSitarCallback(std::function<void(const std::vector<float>&)> callback);
    void setTablaCallback(std::function<void(const std::vector<float>&)> callback);
    void setSarodCallback(std::function<void(const std::vector<float>&)> callback);
    void setOudCallback(std::function<void(const std::vector<float>&)> callback);
    void setNeyCallback(std::function<void(const std::vector<float>&)> callback);
    void setDarbukaCallback(std::function<void(const std::vector<float>&)> callback);
    void setCharangoCallback(std::function<void(const std::vector<float>&)> callback);
    void setQuenaCallback(std::function<void(const std::vector<float>&)> callback);
    void setCajonCallback(std::function<void(const std::vector<float>&)> callback);
    void setUilleannPipesCallback(std::function<void(const std::vector<float>&)> callback);
    void setBodhranCallback(std::function<void(const std::vector<float>&)> callback);
    void setCelticHarpCallback(std::function<void(const std::vector<float>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; 