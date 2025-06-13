#include "DrumMachine.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace VRMusicStudio {

DrumMachine::DrumMachine()
    : numDrums(8), numSteps(16), currentStep(0), playing(false), tempo(120.0f), sampleRate(44100.0f), stepFrames(0), frameCounter(0) {
    pattern.resize(numSteps, std::vector<int>(numDrums, 0));
    for (unsigned int i = 0; i < numDrums; ++i) {
        sampleVolumes[i] = 1.0f;
    }
    stepFrames = static_cast<unsigned long>((60.0f / tempo) * sampleRate / numSteps);
}

DrumMachine::~DrumMachine() {
    shutdown();
}

bool DrumMachine::initialize() {
    frameCounter = 0;
    currentStep = 0;
    playing = false;
    return true;
}

void DrumMachine::shutdown() {
    samples.clear();
    sampleVolumes.clear();
    clearPattern();
}

void DrumMachine::setPattern(const std::vector<std::vector<int>>& newPattern) {
    if (newPattern.size() != numSteps || newPattern[0].size() != numDrums)
        throw std::invalid_argument("Pattern size mismatch");
    pattern = newPattern;
}

const std::vector<std::vector<int>>& DrumMachine::getPattern() const {
    return pattern;
}

void DrumMachine::setStep(unsigned int step, unsigned int drum, int value) {
    if (step < numSteps && drum < numDrums)
        pattern[step][drum] = value;
}

int DrumMachine::getStep(unsigned int step, unsigned int drum) const {
    if (step < numSteps && drum < numDrums)
        return pattern[step][drum];
    return 0;
}

void DrumMachine::clearPattern() {
    for (auto& row : pattern)
        std::fill(row.begin(), row.end(), 0);
}

void DrumMachine::playPattern() {
    playing = true;
    frameCounter = 0;
    currentStep = 0;
}

void DrumMachine::stopPattern() {
    playing = false;
    frameCounter = 0;
    currentStep = 0;
}

bool DrumMachine::isPlaying() const {
    return playing;
}

void DrumMachine::loadSample(unsigned int drum, const std::string& filePath) {
    // Dummy: In echter Implementierung würde hier ein Audio-Loader verwendet
    samples[drum] = std::vector<float>(sampleRate / 10, 0.0f); // 0.1s Stille als Platzhalter
    sampleVolumes[drum] = 1.0f;
}

void DrumMachine::unloadSample(unsigned int drum) {
    samples.erase(drum);
    sampleVolumes.erase(drum);
}

bool DrumMachine::isSampleLoaded(unsigned int drum) const {
    return samples.find(drum) != samples.end();
}

void DrumMachine::setSampleVolume(unsigned int drum, float volume) {
    sampleVolumes[drum] = std::clamp(volume, 0.0f, 1.0f);
}

float DrumMachine::getSampleVolume(unsigned int drum) const {
    auto it = sampleVolumes.find(drum);
    return it != sampleVolumes.end() ? it->second : 1.0f;
}

void DrumMachine::processAudio(float* buffer, unsigned long framesPerBuffer) {
    if (!playing) return;
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        if (frameCounter == 0) {
            // Trigger all drums for this step
            for (unsigned int drum = 0; drum < numDrums; ++drum) {
                if (pattern[currentStep][drum] > 0 && isSampleLoaded(drum)) {
                    triggerDrum(drum);
                }
            }
        }
        // Mix all active samples (hier Dummy, in echt: Sample-Playback mit Hüllkurve etc.)
        float mix = 0.0f;
        for (unsigned int drum = 0; drum < numDrums; ++drum) {
            if (isSampleLoaded(drum)) {
                // Dummy: Einfach Sample-Start, kein echtes Voice-Management
                mix += samples[drum][0] * getSampleVolume(drum);
            }
        }
        buffer[i] += mix;
        frameCounter = (frameCounter + 1) % stepFrames;
        if (frameCounter == 0) {
            advanceStep();
        }
    }
}

void DrumMachine::advanceStep() {
    currentStep = (currentStep + 1) % numSteps;
}

void DrumMachine::triggerDrum(unsigned int drum) {
    // In echter Implementierung: Sample-Playback, Voice-Management, Velocity, Envelope, FX, etc.
    // Hier Dummy: Sample wird auf Position 0 gesetzt
    if (isSampleLoaded(drum)) {
        samples[drum][0] = 1.0f; // Trigger Impuls
    }
}

} // namespace VRMusicStudio 