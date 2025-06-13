#include "WoodwindInstruments.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>

namespace VRMusicStudio {

struct WoodwindInstruments::Impl {
    // Holzblasinstrumente-Parameter
    TransverseFlute transverseFlute;
    Recorder recorder;
    Saxophone saxophone;
    BassClarinet bassClarinet;
    EnglishHorn englishHorn;

    // KI-Modelle
    torch::jit::script::Module transverseFluteModel;
    torch::jit::script::Module recorderModel;
    torch::jit::script::Module saxophoneModel;
    torch::jit::script::Module bassClarinetModel;
    torch::jit::script::Module englishHornModel;

    // Buffer
    std::vector<float> transverseFluteBuffer;
    std::vector<float> recorderBuffer;
    std::vector<float> saxophoneBuffer;
    std::vector<float> bassClarinetBuffer;
    std::vector<float> englishHornBuffer;

    // Callbacks
    std::function<void(const std::vector<float>&)> transverseFluteCallback;
    std::function<void(const std::vector<float>&)> recorderCallback;
    std::function<void(const std::vector<float>&)> saxophoneCallback;
    std::function<void(const std::vector<float>&)> bassClarinetCallback;
    std::function<void(const std::vector<float>&)> englishHornCallback;

    Impl() {
        try {
            // Lade KI-Modelle
            transverseFluteModel = torch::jit::load("models/transverse_flute.pt");
            recorderModel = torch::jit::load("models/recorder.pt");
            saxophoneModel = torch::jit::load("models/saxophone.pt");
            bassClarinetModel = torch::jit::load("models/bass_clarinet.pt");
            englishHornModel = torch::jit::load("models/english_horn.pt");

            // Initialisiere Buffer
            transverseFluteBuffer.resize(1024);
            recorderBuffer.resize(1024);
            saxophoneBuffer.resize(1024);
            bassClarinetBuffer.resize(1024);
            englishHornBuffer.resize(1024);

            // Initialisiere Parameter
            transverseFlute = {0.5f, 0.2f, 5.0f, 1.0f, 0.3f, 0.2f, false};
            recorder = {0.5f, 0.2f, 5.0f, 1.0f, 0.3f, 0.2f, false};
            saxophone = {0.5f, 0.2f, 5.0f, 1.0f, 0.3f, 0.2f, false};
            bassClarinet = {0.5f, 0.2f, 5.0f, 1.0f, 0.3f, 0.2f, false};
            englishHorn = {0.5f, 0.2f, 5.0f, 1.0f, 0.3f, 0.2f, false};
        } catch (const std::exception& e) {
            spdlog::error("Fehler beim Laden der Holzblasinstrumente-Modelle: {}", e.what());
        }
    }
};

WoodwindInstruments::WoodwindInstruments() : pImpl(std::make_unique<Impl>()) {}
WoodwindInstruments::~WoodwindInstruments() = default;

// Querflöte
void WoodwindInstruments::setTransverseFlute(const TransverseFlute& params) {
    pImpl->transverseFlute = params;
}

void WoodwindInstruments::processTransverseFlute(float* buffer, int numSamples) {
    if (!pImpl->transverseFlute.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 6});
        input[0][0] = pImpl->transverseFlute.breathPressure;
        input[0][1] = pImpl->transverseFlute.vibratoDepth;
        input[0][2] = pImpl->transverseFlute.vibratoRate;
        input[0][3] = pImpl->transverseFlute.volume;
        input[0][4] = pImpl->transverseFlute.reverb;
        input[0][5] = pImpl->transverseFlute.delay;

        // Führe KI-Modell aus
        auto output = pImpl->transverseFluteModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->transverseFluteBuffer.begin());

        // Rufe Callback auf
        if (pImpl->transverseFluteCallback) {
            pImpl->transverseFluteCallback(pImpl->transverseFluteBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Querflöte-Verarbeitung: {}", e.what());
    }
}

// Blockflöte
void WoodwindInstruments::setRecorder(const Recorder& params) {
    pImpl->recorder = params;
}

void WoodwindInstruments::processRecorder(float* buffer, int numSamples) {
    if (!pImpl->recorder.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 6});
        input[0][0] = pImpl->recorder.breathPressure;
        input[0][1] = pImpl->recorder.vibratoDepth;
        input[0][2] = pImpl->recorder.vibratoRate;
        input[0][3] = pImpl->recorder.volume;
        input[0][4] = pImpl->recorder.reverb;
        input[0][5] = pImpl->recorder.delay;

        // Führe KI-Modell aus
        auto output = pImpl->recorderModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->recorderBuffer.begin());

        // Rufe Callback auf
        if (pImpl->recorderCallback) {
            pImpl->recorderCallback(pImpl->recorderBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Blockflöte-Verarbeitung: {}", e.what());
    }
}

// Saxophon
void WoodwindInstruments::setSaxophone(const Saxophone& params) {
    pImpl->saxophone = params;
}

void WoodwindInstruments::processSaxophone(float* buffer, int numSamples) {
    if (!pImpl->saxophone.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 6});
        input[0][0] = pImpl->saxophone.breathPressure;
        input[0][1] = pImpl->saxophone.vibratoDepth;
        input[0][2] = pImpl->saxophone.vibratoRate;
        input[0][3] = pImpl->saxophone.volume;
        input[0][4] = pImpl->saxophone.reverb;
        input[0][5] = pImpl->saxophone.delay;

        // Führe KI-Modell aus
        auto output = pImpl->saxophoneModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->saxophoneBuffer.begin());

        // Rufe Callback auf
        if (pImpl->saxophoneCallback) {
            pImpl->saxophoneCallback(pImpl->saxophoneBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Saxophon-Verarbeitung: {}", e.what());
    }
}

// Bassklarinette
void WoodwindInstruments::setBassClarinet(const BassClarinet& params) {
    pImpl->bassClarinet = params;
}

void WoodwindInstruments::processBassClarinet(float* buffer, int numSamples) {
    if (!pImpl->bassClarinet.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 6});
        input[0][0] = pImpl->bassClarinet.breathPressure;
        input[0][1] = pImpl->bassClarinet.vibratoDepth;
        input[0][2] = pImpl->bassClarinet.vibratoRate;
        input[0][3] = pImpl->bassClarinet.volume;
        input[0][4] = pImpl->bassClarinet.reverb;
        input[0][5] = pImpl->bassClarinet.delay;

        // Führe KI-Modell aus
        auto output = pImpl->bassClarinetModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->bassClarinetBuffer.begin());

        // Rufe Callback auf
        if (pImpl->bassClarinetCallback) {
            pImpl->bassClarinetCallback(pImpl->bassClarinetBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Bassklarinette-Verarbeitung: {}", e.what());
    }
}

// Englischhorn
void WoodwindInstruments::setEnglishHorn(const EnglishHorn& params) {
    pImpl->englishHorn = params;
}

void WoodwindInstruments::processEnglishHorn(float* buffer, int numSamples) {
    if (!pImpl->englishHorn.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 6});
        input[0][0] = pImpl->englishHorn.breathPressure;
        input[0][1] = pImpl->englishHorn.vibratoDepth;
        input[0][2] = pImpl->englishHorn.vibratoRate;
        input[0][3] = pImpl->englishHorn.volume;
        input[0][4] = pImpl->englishHorn.reverb;
        input[0][5] = pImpl->englishHorn.delay;

        // Führe KI-Modell aus
        auto output = pImpl->englishHornModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->englishHornBuffer.begin());

        // Rufe Callback auf
        if (pImpl->englishHornCallback) {
            pImpl->englishHornCallback(pImpl->englishHornBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Englischhorn-Verarbeitung: {}", e.what());
    }
}

// Callback-Setter
void WoodwindInstruments::setTransverseFluteCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->transverseFluteCallback = callback;
}

void WoodwindInstruments::setRecorderCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->recorderCallback = callback;
}

void WoodwindInstruments::setSaxophoneCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->saxophoneCallback = callback;
}

void WoodwindInstruments::setBassClarinetCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->bassClarinetCallback = callback;
}

void WoodwindInstruments::setEnglishHornCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->englishHornCallback = callback;
}

} // namespace VRMusicStudio 