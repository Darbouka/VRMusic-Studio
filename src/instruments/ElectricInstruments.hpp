#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <torch/torch.h>

class ElectricInstruments {
public:
    // Elektrische Gitarren
    struct ElectricGuitar {
        float volume;
        float pan;
        float distortion;
        float reverb;
        float delay;
        bool enabled;
    };

    struct BassGuitar {
        float volume;
        float pan;
        float distortion;
        float reverb;
        float delay;
        bool enabled;
    };

    struct ElectricViolin {
        float volume;
        float pan;
        float distortion;
        float reverb;
        float delay;
        bool enabled;
    };

    // Elektrische Keyboards
    struct ElectricPiano {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Organ {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Synthesizer {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    // Elektronische Instrumente
    struct DrumMachine {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Sampler {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    struct Sequencer {
        float volume;
        float pan;
        float reverb;
        float delay;
        bool enabled;
    };

    ElectricInstruments();
    ~ElectricInstruments();

    // Parameter-Setter für alle Instrumente
    void setElectricGuitarParams(const ElectricGuitar& params);
    void setBassGuitarParams(const BassGuitar& params);
    void setElectricViolinParams(const ElectricViolin& params);
    void setElectricPianoParams(const ElectricPiano& params);
    void setOrganParams(const Organ& params);
    void setSynthesizerParams(const Synthesizer& params);
    void setDrumMachineParams(const DrumMachine& params);
    void setSamplerParams(const Sampler& params);
    void setSequencerParams(const Sequencer& params);

    // Audio-Verarbeitung für alle Instrumente
    void processElectricGuitar(std::vector<float>& buffer);
    void processBassGuitar(std::vector<float>& buffer);
    void processElectricViolin(std::vector<float>& buffer);
    void processElectricPiano(std::vector<float>& buffer);
    void processOrgan(std::vector<float>& buffer);
    void processSynthesizer(std::vector<float>& buffer);
    void processDrumMachine(std::vector<float>& buffer);
    void processSampler(std::vector<float>& buffer);
    void processSequencer(std::vector<float>& buffer);

    // Callback-Setter für alle Instrumente
    void setElectricGuitarCallback(std::function<void(const std::vector<float>&)> callback);
    void setBassGuitarCallback(std::function<void(const std::vector<float>&)> callback);
    void setElectricViolinCallback(std::function<void(const std::vector<float>&)> callback);
    void setElectricPianoCallback(std::function<void(const std::vector<float>&)> callback);
    void setOrganCallback(std::function<void(const std::vector<float>&)> callback);
    void setSynthesizerCallback(std::function<void(const std::vector<float>&)> callback);
    void setDrumMachineCallback(std::function<void(const std::vector<float>&)> callback);
    void setSamplerCallback(std::function<void(const std::vector<float>&)> callback);
    void setSequencerCallback(std::function<void(const std::vector<float>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; 