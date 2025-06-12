#pragma once

#include <JuceHeader.h>
#include <vector>
#include <map>
#include <memory>
#include <torch/torch.h>

namespace VRMusicStudio {

class MelodyToVocals : public juce::AudioProcessor {
public:
    // Plugin-Parameter
    struct Parameters {
        float vocalIntensity;
        float breathiness;
        float vibrato;
        float formantShift;
        float pitchCorrection;
        float emotion;
        float articulation;
        float resonance;
    };

    // Vokalisierungs-Modi
    enum class VocalMode {
        Natural,
        Operatic,
        Pop,
        Jazz,
        Electronic,
        Experimental
    };

    MelodyToVocals();
    ~MelodyToVocals() override;

    // JUCE AudioProcessor Interface
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "MelodyToVocals"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Plugin-spezifische Funktionen
    void setParameters(const Parameters& params);
    void setVocalMode(VocalMode mode);
    void setVocalModel(const std::string& modelPath);
    void setLyrics(const std::vector<std::string>& lyrics);
    void setEmotion(const std::string& emotion);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Hilfsfunktionen
    void initializeParameters();
    void processVocalization(juce::AudioBuffer<float>& buffer);
    void updateVocalModel();
    void generateVocalTexture(float* buffer, int numSamples);
    void applyEmotion(float* buffer, int numSamples);
    void applyArticulation(float* buffer, int numSamples);
};

} // namespace VRMusicStudio 