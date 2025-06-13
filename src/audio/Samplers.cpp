#include "Samplers.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>

namespace VRMusicStudio {

struct Samplers::Impl {
    // Sampler-Parameter
    MultiSampler multiSampler;
    DrumSampler drumSampler;
    LoopSampler loopSampler;
    GranularSampler granularSampler;
    TimeStretchSampler timeStretchSampler;

    // KI-Modelle
    torch::jit::script::Module multiModel;
    torch::jit::script::Module drumModel;
    torch::jit::script::Module loopModel;
    torch::jit::script::Module granularModel;
    torch::jit::script::Module timeStretchModel;

    // Buffer
    std::vector<float> multiBuffer;
    std::vector<float> drumBuffer;
    std::vector<float> loopBuffer;
    std::vector<float> granularBuffer;
    std::vector<float> timeStretchBuffer;

    // Callbacks
    std::function<void(const std::vector<float>&)> multiCallback;
    std::function<void(const std::vector<float>&)> drumCallback;
    std::function<void(const std::vector<float>&)> loopCallback;
    std::function<void(const std::vector<float>&)> granularCallback;
    std::function<void(const std::vector<float>&)> timeStretchCallback;

    Impl() {
        try {
            // Lade KI-Modelle
            multiModel = torch::jit::load("models/multi_sampler.pt");
            drumModel = torch::jit::load("models/drum_sampler.pt");
            loopModel = torch::jit::load("models/loop_sampler.pt");
            granularModel = torch::jit::load("models/granular_sampler.pt");
            timeStretchModel = torch::jit::load("models/time_stretch_sampler.pt");

            // Initialisiere Buffer
            multiBuffer.resize(1024);
            drumBuffer.resize(1024);
            loopBuffer.resize(1024);
            granularBuffer.resize(1024);
            timeStretchBuffer.resize(1024);

            // Initialisiere Parameter
            multiSampler = {44100.0f, {}, 1.0f, 0.0f, 1.0f, false};
            drumSampler = {44100.0f, 1.0f, 0.0f, 1.0f, false};
            loopSampler = {44100.0f, {}, 1.0f, 0.0f, 1.0f, false};
            granularSampler = {44100.0f, 0.1f, 0.5f, 1.0f, 0.0f, 1.0f, false};
            timeStretchSampler = {44100.0f, 1.0f, 1.0f, 0.0f, 1.0f, false};
        } catch (const std::exception& e) {
            spdlog::error("Fehler beim Laden der Sampler-Modelle: {}", e.what());
        }
    }
};

Samplers::Samplers() : pImpl(std::make_unique<Impl>()) {}
Samplers::~Samplers() = default;

// Multi-Sampler
void Samplers::setMultiSampler(const MultiSampler& params) {
    pImpl->multiSampler = params;
}

void Samplers::processMultiSampler(float* buffer, int numSamples) {
    if (!pImpl->multiSampler.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 5});
        input[0][0] = pImpl->multiSampler.sampleRate;
        input[0][1] = pImpl->multiSampler.pitch;
        input[0][2] = pImpl->multiSampler.pan;
        input[0][3] = pImpl->multiSampler.volume;

        // Führe KI-Modell aus
        auto output = pImpl->multiModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->multiBuffer.begin());

        // Rufe Callback auf
        if (pImpl->multiCallback) {
            pImpl->multiCallback(pImpl->multiBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Multi-Sampler-Verarbeitung: {}", e.what());
    }
}

// Drum-Sampler
void Samplers::setDrumSampler(const DrumSampler& params) {
    pImpl->drumSampler = params;
}

void Samplers::processDrumSampler(float* buffer, int numSamples) {
    if (!pImpl->drumSampler.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 4});
        input[0][0] = pImpl->drumSampler.sampleRate;
        input[0][1] = pImpl->drumSampler.pitch;
        input[0][2] = pImpl->drumSampler.pan;
        input[0][3] = pImpl->drumSampler.volume;

        // Führe KI-Modell aus
        auto output = pImpl->drumModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->drumBuffer.begin());

        // Rufe Callback auf
        if (pImpl->drumCallback) {
            pImpl->drumCallback(pImpl->drumBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Drum-Sampler-Verarbeitung: {}", e.what());
    }
}

// Loop-Sampler
void Samplers::setLoopSampler(const LoopSampler& params) {
    pImpl->loopSampler = params;
}

void Samplers::processLoopSampler(float* buffer, int numSamples) {
    if (!pImpl->loopSampler.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 5});
        input[0][0] = pImpl->loopSampler.sampleRate;
        input[0][1] = pImpl->loopSampler.pitch;
        input[0][2] = pImpl->loopSampler.pan;
        input[0][3] = pImpl->loopSampler.volume;

        // Führe KI-Modell aus
        auto output = pImpl->loopModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->loopBuffer.begin());

        // Rufe Callback auf
        if (pImpl->loopCallback) {
            pImpl->loopCallback(pImpl->loopBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Loop-Sampler-Verarbeitung: {}", e.what());
    }
}

// Granular-Sampler
void Samplers::setGranularSampler(const GranularSampler& params) {
    pImpl->granularSampler = params;
}

void Samplers::processGranularSampler(float* buffer, int numSamples) {
    if (!pImpl->granularSampler.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 6});
        input[0][0] = pImpl->granularSampler.sampleRate;
        input[0][1] = pImpl->granularSampler.grainSize;
        input[0][2] = pImpl->granularSampler.grainDensity;
        input[0][3] = pImpl->granularSampler.pitch;
        input[0][4] = pImpl->granularSampler.pan;
        input[0][5] = pImpl->granularSampler.volume;

        // Führe KI-Modell aus
        auto output = pImpl->granularModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->granularBuffer.begin());

        // Rufe Callback auf
        if (pImpl->granularCallback) {
            pImpl->granularCallback(pImpl->granularBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Granular-Sampler-Verarbeitung: {}", e.what());
    }
}

// Time-Stretch-Sampler
void Samplers::setTimeStretchSampler(const TimeStretchSampler& params) {
    pImpl->timeStretchSampler = params;
}

void Samplers::processTimeStretchSampler(float* buffer, int numSamples) {
    if (!pImpl->timeStretchSampler.enabled) return;

    try {
        // Erstelle Input-Tensor
        auto input = torch::zeros({1, 5});
        input[0][0] = pImpl->timeStretchSampler.sampleRate;
        input[0][1] = pImpl->timeStretchSampler.timeStretch;
        input[0][2] = pImpl->timeStretchSampler.pitch;
        input[0][3] = pImpl->timeStretchSampler.pan;
        input[0][4] = pImpl->timeStretchSampler.volume;

        // Führe KI-Modell aus
        auto output = pImpl->timeStretchModel.forward({input}).toTensor();

        // Verarbeite Output
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = output[0][i].item<float>();
        }

        // Aktualisiere Buffer
        std::copy(buffer, buffer + numSamples, pImpl->timeStretchBuffer.begin());

        // Rufe Callback auf
        if (pImpl->timeStretchCallback) {
            pImpl->timeStretchCallback(pImpl->timeStretchBuffer);
        }
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Time-Stretch-Sampler-Verarbeitung: {}", e.what());
    }
}

// Callback-Setter
void Samplers::setMultiSamplerCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->multiCallback = callback;
}

void Samplers::setDrumSamplerCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->drumCallback = callback;
}

void Samplers::setLoopSamplerCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->loopCallback = callback;
}

void Samplers::setGranularSamplerCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->granularCallback = callback;
}

void Samplers::setTimeStretchSamplerCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->timeStretchCallback = callback;
}

} // namespace VRMusicStudio 