#pragma once

#include "Instrument.hpp"
#include <vector>
#include <string>
#include <map>

namespace VRMusicStudio {

class DrumMachine : public Instrument {
public:
    DrumMachine();
    ~DrumMachine() override;

    // Instrument Identifikation
    std::string getName() const override { return "Drum Machine"; }
    std::string getVendor() const override { return "VRMusicStudio"; }
    std::string getCategory() const override { return "Percussion"; }
    std::string getVersion() const override { return "1.0.0"; }

    // Instrument Lifecycle
    bool initialize() override;
    void shutdown() override;

    // Pattern/Sequencer
    void setPattern(const std::vector<std::vector<int>>& pattern);
    const std::vector<std::vector<int>>& getPattern() const;
    void setStep(unsigned int step, unsigned int drum, int value);
    int getStep(unsigned int step, unsigned int drum) const;
    void clearPattern();
    void playPattern();
    void stopPattern();
    bool isPlaying() const;

    // Sample Management
    void loadSample(unsigned int drum, const std::string& filePath);
    void unloadSample(unsigned int drum);
    bool isSampleLoaded(unsigned int drum) const;
    void setSampleVolume(unsigned int drum, float volume);
    float getSampleVolume(unsigned int drum) const;

    // Audio Processing
    void processAudio(float* buffer, unsigned long framesPerBuffer) override;

private:
    std::vector<std::vector<int>> pattern; // [step][drum]
    std::map<unsigned int, std::vector<float>> samples;
    std::map<unsigned int, float> sampleVolumes;
    unsigned int numDrums;
    unsigned int numSteps;
    unsigned int currentStep;
    bool playing;
    float tempo;
    float sampleRate;
    unsigned long stepFrames;
    unsigned long frameCounter;

    void advanceStep();
    void triggerDrum(unsigned int drum);
};

} // namespace VRMusicStudio 