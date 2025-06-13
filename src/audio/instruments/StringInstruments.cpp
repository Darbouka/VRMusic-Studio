#include "StringInstruments.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>

namespace VRMusicStudio {

struct StringInstruments::Impl {
    // Streichinstrumente-Parameter
    Violin violin;
    Viola viola;
    Cello cello;
    DoubleBass doubleBass;
    StringOrchestra stringOrchestra;

    // KI-Modelle
    torch::jit::script::Module violinModel;
    torch::jit::script::Module violaModel;
    torch::jit::script::Module celloModel;
    torch::jit::script::Module doubleBassModel;
    torch::jit::script::Module stringOrchestraModel;

    // Buffer
    std::vector<float> violinBuffer;
    std::vector<float> violaBuffer;
    std::vector<float> celloBuffer;
    std::vector<float> doubleBassBuffer;
    std::vector<float> stringOrchestraBuffer;

    // Callbacks
    std::function<void(const std::vector<float>&)> violinCallback;
    std::function<void(const std::vector<float>&)> violaCallback;
    std::function<void(const std::vector<float>&)> celloCallback;
    std::function<void(const std::vector<float>&)> doubleBassCallback;
    std::function<void(const std::vector<float>&)> stringOrchestraCallback;

    Impl() {
        try {
            // Lade KI-Modelle
            violinModel = torch::jit::load("models/violin.pt");
            violaModel = torch::jit::load("models/viola.pt");
            celloModel = torch::jit::load("models/cello.pt");
            doubleBassModel = torch::jit::load("models/double_bass.pt");
            stringOrchestraModel = torch::jit::load("models/string_orchestra.pt");

            // Initialisiere Buffer
            violinBuffer.resize(1024);
            violaBuffer.resize(1024);
            celloBuffer.resize(1024);
            doubleBassBuffer.resize(1024);
            stringOrchestraBuffer.resize(1024);

            // Initialisiere Parameter
            violin = {0.5f, 0.5f, 0.2f, 5.0f, 1.0f, 0.3f, 0.2f, false};
            viola = {0.5f, 0.5f, 0.2f, 5.0f, 1.0f, 0.3f, 0.2f, false};
            cello = {0.5f, 0.5f, 0.2f, 5.0f, 1.0f, 0.3f, 0.2f, false};
            doubleBass = {0.5f, 0.5f, 0.2f, 5.0f, 1.0f, 0.3f, 0.2f, false};
            stringOrchestra = {1.0f, 1.0f, 1.0f, 1.0f, 0.3f, 0.2f, false};
        } catch (const std::exception& e) {
            spdlog::error("Fehler beim Laden der Streichinstrumente-Modelle: {}", e.what());
        }
    }
};

StringInstruments::StringInstruments() : pImpl(std::make_unique<Impl>()) {}
StringInstruments::~StringInstruments() = default;

// Violine
void StringInstruments::setViolin(const Violin& params) {
    pImpl->violin = params;
}

void StringInstruments::processViolin(float* buffer, int numSamples) {
    if (!pImpl->violin.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 7});
        input[0][0] = pImpl->violin.bowPressure;
        input[0][1] = pImpl->violin.bowSpeed;
        input[0][2] = pImpl->violin.vibratoDepth;
        input[0][3] = pImpl->violin.vibratoRate;
        input[0][4] = pImpl->violin.volume;
        input[0][5] = pImpl->violin.reverb;
        input[0][6] = pImpl->violin.delay;

        // Führe KI-Modell aus
        auto output = pImpl->violinModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->violinBuffer.begin());

        // Rufe Callback auf
        if (pImpl->violinCallback) {
            pImpl->violinCallback(pImpl->violinBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Violine-Verarbeitung: {}", e.what());
    }
}

// Viola
void StringInstruments::setViola(const Viola& params) {
    pImpl->viola = params;
}

void StringInstruments::processViola(float* buffer, int numSamples) {
    if (!pImpl->viola.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 7});
        input[0][0] = pImpl->viola.bowPressure;
        input[0][1] = pImpl->viola.bowSpeed;
        input[0][2] = pImpl->viola.vibratoDepth;
        input[0][3] = pImpl->viola.vibratoRate;
        input[0][4] = pImpl->viola.volume;
        input[0][5] = pImpl->viola.reverb;
        input[0][6] = pImpl->viola.delay;

        // Führe KI-Modell aus
        auto output = pImpl->violaModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->violaBuffer.begin());

        // Rufe Callback auf
        if (pImpl->violaCallback) {
            pImpl->violaCallback(pImpl->violaBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Viola-Verarbeitung: {}", e.what());
    }
}

// Cello
void StringInstruments::setCello(const Cello& params) {
    pImpl->cello = params;
}

void StringInstruments::processCello(float* buffer, int numSamples) {
    if (!pImpl->cello.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 7});
        input[0][0] = pImpl->cello.bowPressure;
        input[0][1] = pImpl->cello.bowSpeed;
        input[0][2] = pImpl->cello.vibratoDepth;
        input[0][3] = pImpl->cello.vibratoRate;
        input[0][4] = pImpl->cello.volume;
        input[0][5] = pImpl->cello.reverb;
        input[0][6] = pImpl->cello.delay;

        // Führe KI-Modell aus
        auto output = pImpl->celloModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->celloBuffer.begin());

        // Rufe Callback auf
        if (pImpl->celloCallback) {
            pImpl->celloCallback(pImpl->celloBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Cello-Verarbeitung: {}", e.what());
    }
}

// Kontrabass
void StringInstruments::setDoubleBass(const DoubleBass& params) {
    pImpl->doubleBass = params;
}

void StringInstruments::processDoubleBass(float* buffer, int numSamples) {
    if (!pImpl->doubleBass.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 7});
        input[0][0] = pImpl->doubleBass.bowPressure;
        input[0][1] = pImpl->doubleBass.bowSpeed;
        input[0][2] = pImpl->doubleBass.vibratoDepth;
        input[0][3] = pImpl->doubleBass.vibratoRate;
        input[0][4] = pImpl->doubleBass.volume;
        input[0][5] = pImpl->doubleBass.reverb;
        input[0][6] = pImpl->doubleBass.delay;

        // Führe KI-Modell aus
        auto output = pImpl->doubleBassModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->doubleBassBuffer.begin());

        // Rufe Callback auf
        if (pImpl->doubleBassCallback) {
            pImpl->doubleBassCallback(pImpl->doubleBassBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Kontrabass-Verarbeitung: {}", e.what());
    }
}

// Streichorchester
void StringInstruments::setStringOrchestra(const StringOrchestra& params) {
    pImpl->stringOrchestra = params;
}

void StringInstruments::processStringOrchestra(float* buffer, int numSamples) {
    if (!pImpl->stringOrchestra.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 6});
        input[0][0] = pImpl->stringOrchestra.violinVolume;
        input[0][1] = pImpl->stringOrchestra.violaVolume;
        input[0][2] = pImpl->stringOrchestra.celloVolume;
        input[0][3] = pImpl->stringOrchestra.bassVolume;
        input[0][4] = pImpl->stringOrchestra.reverb;
        input[0][5] = pImpl->stringOrchestra.delay;

        // Führe KI-Modell aus
        auto output = pImpl->stringOrchestraModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->stringOrchestraBuffer.begin());

        // Rufe Callback auf
        if (pImpl->stringOrchestraCallback) {
            pImpl->stringOrchestraCallback(pImpl->stringOrchestraBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Streichorchester-Verarbeitung: {}", e.what());
    }
}

// Callback-Setter
void StringInstruments::setViolinCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->violinCallback = callback;
}

void StringInstruments::setViolaCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->violaCallback = callback;
}

void StringInstruments::setCelloCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->celloCallback = callback;
}

void StringInstruments::setDoubleBassCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->doubleBassCallback = callback;
}

void StringInstruments::setStringOrchestraCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->stringOrchestraCallback = callback;
}

} // namespace VRMusicStudio 