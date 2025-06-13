#include "GranularEffect.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace VRMusicStudio {

GranularEffect::GranularEffect() :
    grainSize(100.0f),
    density(20.0f),
    pitch(1.0f),
    spread(0.5f),
    mix(0.5f),
    feedback(0.0f),
    quality(1.0f),
    automatedGrainSize(false),
    automatedDensity(false),
    automatedPitch(false),
    automatedSpread(false),
    automatedMix(false),
    automatedFeedback(false),
    automatedQuality(false),
    bufferSize(0),
    currentGrainSize(100.0f),
    currentDensity(20.0f),
    currentPitch(1.0f),
    currentSpread(0.5f),
    currentMix(0.5f),
    currentFeedback(0.0f),
    currentQuality(1.0f),
    grainTimer(0.0f),
    densityTimer(0.0f)
{
}

GranularEffect::~GranularEffect() {
    shutdown();
}

bool GranularEffect::initialize() {
    const float sampleRate = 44100.0f;
    bufferSize = static_cast<size_t>(sampleRate * 2.0f); // 2 Sekunden Buffer
    inputBuffer.resize(bufferSize, 0.0f);
    initializeGrains();
    return true;
}

void GranularEffect::shutdown() {
    grains.clear();
    inputBuffer.clear();
}

std::vector<PluginParameter> GranularEffect::getParameters() const {
    return {
        {"grainSize", 100.0f, 1.0f, 1000.0f},
        {"density", 20.0f, 1.0f, 100.0f},
        {"pitch", 1.0f, 0.25f, 4.0f},
        {"spread", 0.5f, 0.0f, 1.0f},
        {"mix", 0.5f, 0.0f, 1.0f},
        {"feedback", 0.0f, 0.0f, 0.9f},
        {"quality", 1.0f, 0.0f, 1.0f}
    };
}

void GranularEffect::setParameter(const std::string& name, float value) {
    if (name == "grainSize") grainSize = value;
    else if (name == "density") density = value;
    else if (name == "pitch") pitch = value;
    else if (name == "spread") spread = value;
    else if (name == "mix") mix = value;
    else if (name == "feedback") feedback = value;
    else if (name == "quality") quality = value;
}

float GranularEffect::getParameter(const std::string& name) const {
    if (name == "grainSize") return grainSize;
    else if (name == "density") return density;
    else if (name == "pitch") return pitch;
    else if (name == "spread") return spread;
    else if (name == "mix") return mix;
    else if (name == "feedback") return feedback;
    else if (name == "quality") return quality;
    return 0.0f;
}

void GranularEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "grainSize") automatedGrainSize = automated;
    else if (name == "density") automatedDensity = automated;
    else if (name == "pitch") automatedPitch = automated;
    else if (name == "spread") automatedSpread = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "feedback") automatedFeedback = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool GranularEffect::isParameterAutomated(const std::string& name) const {
    if (name == "grainSize") return automatedGrainSize;
    else if (name == "density") return automatedDensity;
    else if (name == "pitch") return automatedPitch;
    else if (name == "spread") return automatedSpread;
    else if (name == "mix") return automatedMix;
    else if (name == "feedback") return automatedFeedback;
    else if (name == "quality") return automatedQuality;
    return false;
}

void GranularEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateParameters();

    // Update input buffer
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        inputBuffer.push_back(buffer[i]);
        inputBuffer.pop_front();
    }

    // Process grains
    processGrains(buffer, framesPerBuffer);
}

void GranularEffect::loadPreset(const std::string& presetName) {
    // TODO: Implement preset loading
}

void GranularEffect::savePreset(const std::string& presetName) {
    // TODO: Implement preset saving
}

std::vector<std::string> GranularEffect::getAvailablePresets() const {
    return {}; // TODO: Implement preset list
}

void GranularEffect::updateParameters() {
    currentGrainSize = grainSize;
    currentDensity = density;
    currentPitch = pitch;
    currentSpread = spread;
    currentMix = mix;
    currentFeedback = feedback;
    currentQuality = quality;
}

void GranularEffect::initializeGrains() {
    grains.clear();
    const int maxGrains = 100; // Maximum number of simultaneous grains
    grains.resize(maxGrains);
    for (auto& grain : grains) {
        grain.buffer.resize(bufferSize, 0.0f);
        grain.position = 0;
        grain.pitch = 1.0f;
        grain.volume = 0.0f;
        grain.pan = 0.5f;
        grain.active = false;
    }
}

void GranularEffect::updateGrains() {
    const float sampleRate = 44100.0f;
    const float grainSizeSamples = currentGrainSize * sampleRate / 1000.0f;
    const float densitySamples = sampleRate / currentDensity;

    // Update existing grains
    for (auto& grain : grains) {
        if (grain.active) {
            grain.position += grain.pitch;
            if (grain.position >= grainSizeSamples) {
                grain.active = false;
            }
        }
    }

    // Create new grains
    densityTimer += 1.0f;
    if (densityTimer >= densitySamples) {
        densityTimer = 0.0f;
        createGrain();
    }
}

void GranularEffect::processGrains(float* buffer, unsigned long framesPerBuffer) {
    updateGrains();

    // Clear output buffer
    std::fill(buffer, buffer + framesPerBuffer, 0.0f);

    // Process each grain
    for (auto& grain : grains) {
        if (grain.active) {
            for (unsigned long i = 0; i < framesPerBuffer; ++i) {
                size_t readPos = static_cast<size_t>(grain.position);
                if (readPos < bufferSize) {
                    float envelope = calculateGrainEnvelope(grain.position, currentGrainSize);
                    float sample = grain.buffer[readPos] * envelope * grain.volume;
                    
                    // Apply panning
                    float leftGain = 1.0f - grain.pan;
                    float rightGain = grain.pan;
                    
                    buffer[i] += sample * leftGain;
                    if (i + 1 < framesPerBuffer) {
                        buffer[i + 1] += sample * rightGain;
                    }
                }
            }
        }
    }

    // Mix with original signal
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = buffer[i] * currentMix + inputBuffer[i] * (1.0f - currentMix);
    }
}

void GranularEffect::createGrain() {
    // Find inactive grain
    for (auto& grain : grains) {
        if (!grain.active) {
            // Initialize grain
            grain.position = 0;
            grain.pitch = calculateGrainPitch();
            grain.volume = 1.0f;
            grain.pan = calculateGrainPan();
            grain.active = true;

            // Copy input buffer to grain buffer
            std::copy(inputBuffer.begin(), inputBuffer.end(), grain.buffer.begin());
            break;
        }
    }
}

void GranularEffect::destroyGrain(size_t index) {
    if (index < grains.size()) {
        grains[index].active = false;
    }
}

float GranularEffect::calculateGrainEnvelope(float position, float size) {
    const float sampleRate = 44100.0f;
    const float sizeSamples = size * sampleRate / 1000.0f;
    const float attackTime = sizeSamples * 0.1f;
    const float releaseTime = sizeSamples * 0.1f;

    if (position < attackTime) {
        return position / attackTime;
    } else if (position > sizeSamples - releaseTime) {
        return (sizeSamples - position) / releaseTime;
    }
    return 1.0f;
}

float GranularEffect::calculateGrainPitch() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(-currentSpread, currentSpread);
    return currentPitch * (1.0f + dist(gen));
}

float GranularEffect::calculateGrainPan() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(gen);
}

} // namespace VRMusicStudio 