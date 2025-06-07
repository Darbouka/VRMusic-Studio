#pragma once

#include "../PluginInterface.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace VR_DAW {

class Synthesizer : public InstrumentPlugin {
public:
    Synthesizer();
    ~Synthesizer();

    // Plugin-Identifikation
    const std::string& getId() const override { return id; }
    const std::string& getName() const override { return name; }
    PluginType getType() const override { return PluginType::Instrument; }

    // Plugin-Lebenszyklus
    bool initialize() override;
    void shutdown() override;
    void update() override;

    // Parameter-Management
    std::vector<PluginParameter> getParameters() const override;
    void setParameter(const std::string& name, float value) override;
    float getParameter(const std::string& name) const override;
    void setParameterAutomated(const std::string& name, bool automated) override;
    bool isParameterAutomated(const std::string& name) const override;

    // Audio-Verarbeitung
    void processAudio(float* buffer, unsigned long framesPerBuffer) override;
    void processMidi(const std::vector<uint8_t>& midiData) override;

    // Automation
    void setAutomationPoint(const std::string& parameter, double time, float value) override;
    void removeAutomationPoint(const std::string& parameter, double time) override;
    void clearAutomation(const std::string& parameter) override;
    std::vector<std::pair<double, float>> getAutomationPoints(const std::string& parameter) const override;

    // Preset-Management
    void savePreset(const std::string& name) override;
    void loadPreset(const std::string& name) override;
    std::vector<std::string> getPresetList() const override;

    // UI-Integration
    void setUIPosition(const glm::vec3& position) override;
    void setUIOrientation(const glm::quat& orientation) override;
    void setUIScale(const glm::vec3& scale) override;
    void renderUI() override;

    // Instrument-spezifische Funktionen
    void noteOn(int note, int velocity) override;
    void noteOff(int note) override;
    void setPitchBend(float value) override;
    void setModulation(float value) override;
    void setAftertouch(float value) override;

    // Synthesizer-spezifische Funktionen
    void setOscillatorWaveform(int oscillator, const std::string& waveform);
    void setOscillatorFrequency(int oscillator, float frequency);
    void setOscillatorDetune(int oscillator, float detune);
    void setOscillatorMix(int oscillator, float mix);
    void setFilterType(const std::string& type);
    void setFilterCutoff(float cutoff);
    void setFilterResonance(float resonance);
    void setFilterEnvelopeAmount(float amount);
    void setFilterEnvelopeAttack(float attack);
    void setFilterEnvelopeDecay(float decay);
    void setFilterEnvelopeSustain(float sustain);
    void setFilterEnvelopeRelease(float release);
    void setAmpEnvelopeAttack(float attack);
    void setAmpEnvelopeDecay(float decay);
    void setAmpEnvelopeSustain(float sustain);
    void setAmpEnvelopeRelease(float release);
    void setLfoRate(float rate);
    void setLfoAmount(float amount);
    void setLfoWaveform(const std::string& waveform);
    void setLfoDestination(const std::string& destination);

private:
    std::string id;
    std::string name;

    // Synthesizer-Komponenten
    struct Oscillator {
        std::string waveform;
        float frequency;
        float detune;
        float mix;
        float phase;
    };
    std::vector<Oscillator> oscillators;

    struct Filter {
        std::string type;
        float cutoff;
        float resonance;
        float envelopeAmount;
        float envelopeAttack;
        float envelopeDecay;
        float envelopeSustain;
        float envelopeRelease;
    };
    Filter filter;

    struct Envelope {
        float attack;
        float decay;
        float sustain;
        float release;
    };
    Envelope ampEnvelope;
    Envelope filterEnvelope;

    struct LFO {
        float rate;
        float amount;
        std::string waveform;
        std::string destination;
        float phase;
    };
    LFO lfo;

    // Aktive Noten
    struct Note {
        int note;
        int velocity;
        float amplitude;
        float filterEnvelope;
        float ampEnvelope;
        bool active;
    };
    std::map<int, Note> activeNotes;

    // Hilfsfunktionen
    void generateId();
    void initializeOscillators();
    void initializeFilter();
    void initializeEnvelopes();
    void initializeLFO();
    float processOscillator(const Oscillator& osc, float time);
    float processFilter(float input, float cutoff, float resonance);
    float processEnvelope(const Envelope& env, float time, bool noteOn);
    float processLFO(float time);
};

} // namespace VR_DAW 