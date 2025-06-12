#include "PercussionInstruments.hpp"
#include <spdlog/spdlog.h>

struct PercussionInstruments::Impl {
    DrumKit drumKit;
    Timpani timpani;
    Cymbals cymbals;
    Marimba marimba;
    Xylophone xylophone;
    Vibraphone vibraphone;
    Glockenspiel glockenspiel;
    TubularBells tubularBells;

    torch::jit::script::Module drumKitModel;
    torch::jit::script::Module timpaniModel;
    torch::jit::script::Module cymbalsModel;
    torch::jit::script::Module marimbaModel;
    torch::jit::script::Module xylophoneModel;
    torch::jit::script::Module vibraphoneModel;
    torch::jit::script::Module glockenspielModel;
    torch::jit::script::Module tubularBellsModel;

    std::vector<float> drumKitBuffer;
    std::vector<float> timpaniBuffer;
    std::vector<float> cymbalsBuffer;
    std::vector<float> marimbaBuffer;
    std::vector<float> xylophoneBuffer;
    std::vector<float> vibraphoneBuffer;
    std::vector<float> glockenspielBuffer;
    std::vector<float> tubularBellsBuffer;

    std::function<void(const std::vector<float>&)> drumKitCallback;
    std::function<void(const std::vector<float>&)> timpaniCallback;
    std::function<void(const std::vector<float>&)> cymbalsCallback;
    std::function<void(const std::vector<float>&)> marimbaCallback;
    std::function<void(const std::vector<float>&)> xylophoneCallback;
    std::function<void(const std::vector<float>&)> vibraphoneCallback;
    std::function<void(const std::vector<float>&)> glockenspielCallback;
    std::function<void(const std::vector<float>&)> tubularBellsCallback;

    Impl() {
        try {
            drumKitModel = torch::jit::load("models/drum_kit.pt");
            timpaniModel = torch::jit::load("models/timpani.pt");
            cymbalsModel = torch::jit::load("models/cymbals.pt");
            marimbaModel = torch::jit::load("models/marimba.pt");
            xylophoneModel = torch::jit::load("models/xylophone.pt");
            vibraphoneModel = torch::jit::load("models/vibraphone.pt");
            glockenspielModel = torch::jit::load("models/glockenspiel.pt");
            tubularBellsModel = torch::jit::load("models/tubular_bells.pt");

            drumKitBuffer.resize(1024);
            timpaniBuffer.resize(1024);
            cymbalsBuffer.resize(1024);
            marimbaBuffer.resize(1024);
            xylophoneBuffer.resize(1024);
            vibraphoneBuffer.resize(1024);
            glockenspielBuffer.resize(1024);
            tubularBellsBuffer.resize(1024);
        } catch (const c10::Error& e) {
            spdlog::error("Fehler beim Laden der KI-Modelle: {}", e.what());
        }
    }
};

PercussionInstruments::PercussionInstruments() : pImpl(std::make_unique<Impl>()) {}
PercussionInstruments::~PercussionInstruments() = default;

void PercussionInstruments::setDrumKitParams(const DrumKit& params) {
    pImpl->drumKit = params;
}

void PercussionInstruments::setTimpaniParams(const Timpani& params) {
    pImpl->timpani = params;
}

void PercussionInstruments::setCymbalsParams(const Cymbals& params) {
    pImpl->cymbals = params;
}

void PercussionInstruments::setMarimbaParams(const Marimba& params) {
    pImpl->marimba = params;
}

void PercussionInstruments::setXylophoneParams(const Xylophone& params) {
    pImpl->xylophone = params;
}

void PercussionInstruments::setVibraphoneParams(const Vibraphone& params) {
    pImpl->vibraphone = params;
}

void PercussionInstruments::setGlockenspielParams(const Glockenspiel& params) {
    pImpl->glockenspiel = params;
}

void PercussionInstruments::setTubularBellsParams(const TubularBells& params) {
    pImpl->tubularBells = params;
}

void PercussionInstruments::processDrumKit(std::vector<float>& buffer) {
    if (!pImpl->drumKit.enabled) return;

    try {
        std::vector<torch::jit::IValue> inputs;
        inputs.push_back(torch::tensor({
            pImpl->drumKit.volume,
            pImpl->drumKit.pan,
            pImpl->drumKit.reverb,
            pImpl->drumKit.delay
        }));

        auto output = pImpl->drumKitModel.forward(inputs).toTensor();
        auto outputData = output.data_ptr<float>();

        for (size_t i = 0; i < buffer.size(); ++i) {
            buffer[i] = outputData[i] * pImpl->drumKit.volume;
        }

        if (pImpl->drumKitCallback) {
            pImpl->drumKitCallback(buffer);
        }
    } catch (const c10::Error& e) {
        spdlog::error("Fehler bei der DrumKit-Verarbeitung: {}", e.what());
    }
}

void PercussionInstruments::processTimpani(std::vector<float>& buffer) {
    if (!pImpl->timpani.enabled) return;

    try {
        std::vector<torch::jit::IValue> inputs;
        inputs.push_back(torch::tensor({
            pImpl->timpani.volume,
            pImpl->timpani.pan,
            pImpl->timpani.reverb,
            pImpl->timpani.delay
        }));

        auto output = pImpl->timpaniModel.forward(inputs).toTensor();
        auto outputData = output.data_ptr<float>();

        for (size_t i = 0; i < buffer.size(); ++i) {
            buffer[i] = outputData[i] * pImpl->timpani.volume;
        }

        if (pImpl->timpaniCallback) {
            pImpl->timpaniCallback(buffer);
        }
    } catch (const c10::Error& e) {
        spdlog::error("Fehler bei der Timpani-Verarbeitung: {}", e.what());
    }
}

void PercussionInstruments::processCymbals(std::vector<float>& buffer) {
    if (!pImpl->cymbals.enabled) return;

    try {
        std::vector<torch::jit::IValue> inputs;
        inputs.push_back(torch::tensor({
            pImpl->cymbals.volume,
            pImpl->cymbals.pan,
            pImpl->cymbals.reverb,
            pImpl->cymbals.delay
        }));

        auto output = pImpl->cymbalsModel.forward(inputs).toTensor();
        auto outputData = output.data_ptr<float>();

        for (size_t i = 0; i < buffer.size(); ++i) {
            buffer[i] = outputData[i] * pImpl->cymbals.volume;
        }

        if (pImpl->cymbalsCallback) {
            pImpl->cymbalsCallback(buffer);
        }
    } catch (const c10::Error& e) {
        spdlog::error("Fehler bei der Cymbals-Verarbeitung: {}", e.what());
    }
}

void PercussionInstruments::processMarimba(std::vector<float>& buffer) {
    if (!pImpl->marimba.enabled) return;

    try {
        std::vector<torch::jit::IValue> inputs;
        inputs.push_back(torch::tensor({
            pImpl->marimba.volume,
            pImpl->marimba.pan,
            pImpl->marimba.reverb,
            pImpl->marimba.delay
        }));

        auto output = pImpl->marimbaModel.forward(inputs).toTensor();
        auto outputData = output.data_ptr<float>();

        for (size_t i = 0; i < buffer.size(); ++i) {
            buffer[i] = outputData[i] * pImpl->marimba.volume;
        }

        if (pImpl->marimbaCallback) {
            pImpl->marimbaCallback(buffer);
        }
    } catch (const c10::Error& e) {
        spdlog::error("Fehler bei der Marimba-Verarbeitung: {}", e.what());
    }
}

void PercussionInstruments::processXylophone(std::vector<float>& buffer) {
    if (!pImpl->xylophone.enabled) return;

    try {
        std::vector<torch::jit::IValue> inputs;
        inputs.push_back(torch::tensor({
            pImpl->xylophone.volume,
            pImpl->xylophone.pan,
            pImpl->xylophone.reverb,
            pImpl->xylophone.delay
        }));

        auto output = pImpl->xylophoneModel.forward(inputs).toTensor();
        auto outputData = output.data_ptr<float>();

        for (size_t i = 0; i < buffer.size(); ++i) {
            buffer[i] = outputData[i] * pImpl->xylophone.volume;
        }

        if (pImpl->xylophoneCallback) {
            pImpl->xylophoneCallback(buffer);
        }
    } catch (const c10::Error& e) {
        spdlog::error("Fehler bei der Xylophone-Verarbeitung: {}", e.what());
    }
}

void PercussionInstruments::processVibraphone(std::vector<float>& buffer) {
    if (!pImpl->vibraphone.enabled) return;

    try {
        std::vector<torch::jit::IValue> inputs;
        inputs.push_back(torch::tensor({
            pImpl->vibraphone.volume,
            pImpl->vibraphone.pan,
            pImpl->vibraphone.reverb,
            pImpl->vibraphone.delay
        }));

        auto output = pImpl->vibraphoneModel.forward(inputs).toTensor();
        auto outputData = output.data_ptr<float>();

        for (size_t i = 0; i < buffer.size(); ++i) {
            buffer[i] = outputData[i] * pImpl->vibraphone.volume;
        }

        if (pImpl->vibraphoneCallback) {
            pImpl->vibraphoneCallback(buffer);
        }
    } catch (const c10::Error& e) {
        spdlog::error("Fehler bei der Vibraphone-Verarbeitung: {}", e.what());
    }
}

void PercussionInstruments::processGlockenspiel(std::vector<float>& buffer) {
    if (!pImpl->glockenspiel.enabled) return;

    try {
        std::vector<torch::jit::IValue> inputs;
        inputs.push_back(torch::tensor({
            pImpl->glockenspiel.volume,
            pImpl->glockenspiel.pan,
            pImpl->glockenspiel.reverb,
            pImpl->glockenspiel.delay
        }));

        auto output = pImpl->glockenspielModel.forward(inputs).toTensor();
        auto outputData = output.data_ptr<float>();

        for (size_t i = 0; i < buffer.size(); ++i) {
            buffer[i] = outputData[i] * pImpl->glockenspiel.volume;
        }

        if (pImpl->glockenspielCallback) {
            pImpl->glockenspielCallback(buffer);
        }
    } catch (const c10::Error& e) {
        spdlog::error("Fehler bei der Glockenspiel-Verarbeitung: {}", e.what());
    }
}

void PercussionInstruments::processTubularBells(std::vector<float>& buffer) {
    if (!pImpl->tubularBells.enabled) return;

    try {
        std::vector<torch::jit::IValue> inputs;
        inputs.push_back(torch::tensor({
            pImpl->tubularBells.volume,
            pImpl->tubularBells.pan,
            pImpl->tubularBells.reverb,
            pImpl->tubularBells.delay
        }));

        auto output = pImpl->tubularBellsModel.forward(inputs).toTensor();
        auto outputData = output.data_ptr<float>();

        for (size_t i = 0; i < buffer.size(); ++i) {
            buffer[i] = outputData[i] * pImpl->tubularBells.volume;
        }

        if (pImpl->tubularBellsCallback) {
            pImpl->tubularBellsCallback(buffer);
        }
    } catch (const c10::Error& e) {
        spdlog::error("Fehler bei der TubularBells-Verarbeitung: {}", e.what());
    }
}

void PercussionInstruments::setDrumKitCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->drumKitCallback = callback;
}

void PercussionInstruments::setTimpaniCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->timpaniCallback = callback;
}

void PercussionInstruments::setCymbalsCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->cymbalsCallback = callback;
}

void PercussionInstruments::setMarimbaCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->marimbaCallback = callback;
}

void PercussionInstruments::setXylophoneCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->xylophoneCallback = callback;
}

void PercussionInstruments::setVibraphoneCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->vibraphoneCallback = callback;
}

void PercussionInstruments::setGlockenspielCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->glockenspielCallback = callback;
}

void PercussionInstruments::setTubularBellsCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->tubularBellsCallback = callback;
} 