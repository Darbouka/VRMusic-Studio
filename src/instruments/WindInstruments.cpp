#include "WindInstruments.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>

namespace VRMusicStudio {

struct WindInstruments::Impl {
    // Blasinstrumente-Parameter
    Flute flute;
    Oboe oboe;
    Clarinet clarinet;
    Bassoon bassoon;
    Horn horn;
    Trumpet trumpet;
    Trombone trombone;
    Tuba tuba;

    // KI-Modelle
    torch::jit::script::Module fluteModel;
    torch::jit::script::Module oboeModel;
    torch::jit::script::Module clarinetModel;
    torch::jit::script::Module bassoonModel;
    torch::jit::script::Module hornModel;
    torch::jit::script::Module trumpetModel;
    torch::jit::script::Module tromboneModel;
    torch::jit::script::Module tubaModel;

    // Buffer
    std::vector<float> fluteBuffer;
    std::vector<float> oboeBuffer;
    std::vector<float> clarinetBuffer;
    std::vector<float> bassoonBuffer;
    std::vector<float> hornBuffer;
    std::vector<float> trumpetBuffer;
    std::vector<float> tromboneBuffer;
    std::vector<float> tubaBuffer;

    // Callbacks
    std::function<void(const std::vector<float>&)> fluteCallback;
    std::function<void(const std::vector<float>&)> oboeCallback;
    std::function<void(const std::vector<float>&)> clarinetCallback;
    std::function<void(const std::vector<float>&)> bassoonCallback;
    std::function<void(const std::vector<float>&)> hornCallback;
    std::function<void(const std::vector<float>&)> trumpetCallback;
    std::function<void(const std::vector<float>&)> tromboneCallback;
    std::function<void(const std::vector<float>&)> tubaCallback;

    Impl() {
        try {
            // Lade KI-Modelle
            fluteModel = torch::jit::load("models/flute.pt");
            oboeModel = torch::jit::load("models/oboe.pt");
            clarinetModel = torch::jit::load("models/clarinet.pt");
            bassoonModel = torch::jit::load("models/bassoon.pt");
            hornModel = torch::jit::load("models/horn.pt");
            trumpetModel = torch::jit::load("models/trumpet.pt");
            tromboneModel = torch::jit::load("models/trombone.pt");
            tubaModel = torch::jit::load("models/tuba.pt");

            // Initialisiere Buffer
            fluteBuffer.resize(1024);
            oboeBuffer.resize(1024);
            clarinetBuffer.resize(1024);
            bassoonBuffer.resize(1024);
            hornBuffer.resize(1024);
            trumpetBuffer.resize(1024);
            tromboneBuffer.resize(1024);
            tubaBuffer.resize(1024);

            // Initialisiere Parameter
            flute = {0.5f, 0.2f, 5.0f, 1.0f, 0.3f, 0.2f, false};
            oboe = {0.5f, 0.2f, 5.0f, 1.0f, 0.3f, 0.2f, false};
            clarinet = {0.5f, 0.2f, 5.0f, 1.0f, 0.3f, 0.2f, false};
            bassoon = {0.5f, 0.2f, 5.0f, 1.0f, 0.3f, 0.2f, false};
            horn = {0.5f, 0.2f, 5.0f, 1.0f, 0.3f, 0.2f, false};
            trumpet = {0.5f, 0.2f, 5.0f, 1.0f, 0.3f, 0.2f, false};
            trombone = {0.5f, 0.2f, 5.0f, 1.0f, 0.3f, 0.2f, false};
            tuba = {0.5f, 0.2f, 5.0f, 1.0f, 0.3f, 0.2f, false};
        } catch (const std::exception& e) {
            spdlog::error("Fehler beim Laden der Blasinstrumente-Modelle: {}", e.what());
        }
    }
};

WindInstruments::WindInstruments() : pImpl(std::make_unique<Impl>()) {}
WindInstruments::~WindInstruments() = default;

// Flöte
void WindInstruments::setFlute(const Flute& params) {
    pImpl->flute = params;
}

void WindInstruments::processFlute(float* buffer, int numSamples) {
    if (!pImpl->flute.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 6});
        input[0][0] = pImpl->flute.breathPressure;
        input[0][1] = pImpl->flute.vibratoDepth;
        input[0][2] = pImpl->flute.vibratoRate;
        input[0][3] = pImpl->flute.volume;
        input[0][4] = pImpl->flute.reverb;
        input[0][5] = pImpl->flute.delay;

        // Führe KI-Modell aus
        auto output = pImpl->fluteModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->fluteBuffer.begin());

        // Rufe Callback auf
        if (pImpl->fluteCallback) {
            pImpl->fluteCallback(pImpl->fluteBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Flöte-Verarbeitung: {}", e.what());
    }
}

// Oboe
void WindInstruments::setOboe(const Oboe& params) {
    pImpl->oboe = params;
}

void WindInstruments::processOboe(float* buffer, int numSamples) {
    if (!pImpl->oboe.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 6});
        input[0][0] = pImpl->oboe.breathPressure;
        input[0][1] = pImpl->oboe.vibratoDepth;
        input[0][2] = pImpl->oboe.vibratoRate;
        input[0][3] = pImpl->oboe.volume;
        input[0][4] = pImpl->oboe.reverb;
        input[0][5] = pImpl->oboe.delay;

        // Führe KI-Modell aus
        auto output = pImpl->oboeModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->oboeBuffer.begin());

        // Rufe Callback auf
        if (pImpl->oboeCallback) {
            pImpl->oboeCallback(pImpl->oboeBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Oboe-Verarbeitung: {}", e.what());
    }
}

// Klarinette
void WindInstruments::setClarinet(const Clarinet& params) {
    pImpl->clarinet = params;
}

void WindInstruments::processClarinet(float* buffer, int numSamples) {
    if (!pImpl->clarinet.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 6});
        input[0][0] = pImpl->clarinet.breathPressure;
        input[0][1] = pImpl->clarinet.vibratoDepth;
        input[0][2] = pImpl->clarinet.vibratoRate;
        input[0][3] = pImpl->clarinet.volume;
        input[0][4] = pImpl->clarinet.reverb;
        input[0][5] = pImpl->clarinet.delay;

        // Führe KI-Modell aus
        auto output = pImpl->clarinetModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->clarinetBuffer.begin());

        // Rufe Callback auf
        if (pImpl->clarinetCallback) {
            pImpl->clarinetCallback(pImpl->clarinetBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Klarinette-Verarbeitung: {}", e.what());
    }
}

// Fagott
void WindInstruments::setBassoon(const Bassoon& params) {
    pImpl->bassoon = params;
}

void WindInstruments::processBassoon(float* buffer, int numSamples) {
    if (!pImpl->bassoon.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 6});
        input[0][0] = pImpl->bassoon.breathPressure;
        input[0][1] = pImpl->bassoon.vibratoDepth;
        input[0][2] = pImpl->bassoon.vibratoRate;
        input[0][3] = pImpl->bassoon.volume;
        input[0][4] = pImpl->bassoon.reverb;
        input[0][5] = pImpl->bassoon.delay;

        // Führe KI-Modell aus
        auto output = pImpl->bassoonModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->bassoonBuffer.begin());

        // Rufe Callback auf
        if (pImpl->bassoonCallback) {
            pImpl->bassoonCallback(pImpl->bassoonBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Fagott-Verarbeitung: {}", e.what());
    }
}

// Horn
void WindInstruments::setHorn(const Horn& params) {
    pImpl->horn = params;
}

void WindInstruments::processHorn(float* buffer, int numSamples) {
    if (!pImpl->horn.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 6});
        input[0][0] = pImpl->horn.breathPressure;
        input[0][1] = pImpl->horn.vibratoDepth;
        input[0][2] = pImpl->horn.vibratoRate;
        input[0][3] = pImpl->horn.volume;
        input[0][4] = pImpl->horn.reverb;
        input[0][5] = pImpl->horn.delay;

        // Führe KI-Modell aus
        auto output = pImpl->hornModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->hornBuffer.begin());

        // Rufe Callback auf
        if (pImpl->hornCallback) {
            pImpl->hornCallback(pImpl->hornBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Horn-Verarbeitung: {}", e.what());
    }
}

// Trompete
void WindInstruments::setTrumpet(const Trumpet& params) {
    pImpl->trumpet = params;
}

void WindInstruments::processTrumpet(float* buffer, int numSamples) {
    if (!pImpl->trumpet.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 6});
        input[0][0] = pImpl->trumpet.breathPressure;
        input[0][1] = pImpl->trumpet.vibratoDepth;
        input[0][2] = pImpl->trumpet.vibratoRate;
        input[0][3] = pImpl->trumpet.volume;
        input[0][4] = pImpl->trumpet.reverb;
        input[0][5] = pImpl->trumpet.delay;

        // Führe KI-Modell aus
        auto output = pImpl->trumpetModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->trumpetBuffer.begin());

        // Rufe Callback auf
        if (pImpl->trumpetCallback) {
            pImpl->trumpetCallback(pImpl->trumpetBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Trompete-Verarbeitung: {}", e.what());
    }
}

// Posaune
void WindInstruments::setTrombone(const Trombone& params) {
    pImpl->trombone = params;
}

void WindInstruments::processTrombone(float* buffer, int numSamples) {
    if (!pImpl->trombone.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 6});
        input[0][0] = pImpl->trombone.breathPressure;
        input[0][1] = pImpl->trombone.vibratoDepth;
        input[0][2] = pImpl->trombone.vibratoRate;
        input[0][3] = pImpl->trombone.volume;
        input[0][4] = pImpl->trombone.reverb;
        input[0][5] = pImpl->trombone.delay;

        // Führe KI-Modell aus
        auto output = pImpl->tromboneModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->tromboneBuffer.begin());

        // Rufe Callback auf
        if (pImpl->tromboneCallback) {
            pImpl->tromboneCallback(pImpl->tromboneBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Posaune-Verarbeitung: {}", e.what());
    }
}

// Tuba
void WindInstruments::setTuba(const Tuba& params) {
    pImpl->tuba = params;
}

void WindInstruments::processTuba(float* buffer, int numSamples) {
    if (!pImpl->tuba.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 6});
        input[0][0] = pImpl->tuba.breathPressure;
        input[0][1] = pImpl->tuba.vibratoDepth;
        input[0][2] = pImpl->tuba.vibratoRate;
        input[0][3] = pImpl->tuba.volume;
        input[0][4] = pImpl->tuba.reverb;
        input[0][5] = pImpl->tuba.delay;

        // Führe KI-Modell aus
        auto output = pImpl->tubaModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->tubaBuffer.begin());

        // Rufe Callback auf
        if (pImpl->tubaCallback) {
            pImpl->tubaCallback(pImpl->tubaBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Tuba-Verarbeitung: {}", e.what());
    }
}

// Callback-Setter
void WindInstruments::setFluteCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->fluteCallback = callback;
}

void WindInstruments::setOboeCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->oboeCallback = callback;
}

void WindInstruments::setClarinetCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->clarinetCallback = callback;
}

void WindInstruments::setBassoonCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->bassoonCallback = callback;
}

void WindInstruments::setHornCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->hornCallback = callback;
}

void WindInstruments::setTrumpetCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->trumpetCallback = callback;
}

void WindInstruments::setTromboneCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->tromboneCallback = callback;
}

void WindInstruments::setTubaCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->tubaCallback = callback;
}

} // namespace VRMusicStudio 