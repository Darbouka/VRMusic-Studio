#include "BeatGrinderEffect.hpp"
#include <algorithm>
#include <cmath>

namespace VRMusicStudio {

BeatGrinderEffect::BeatGrinderEffect() :
    gridSize(0.25f),
    swing(0.0f),
    shuffle(0.0f),
    stutter(0.0f),
    reverse(0.0f),
    mix(0.5f),
    quality(1.0f),
    automatedGridSize(false),
    automatedSwing(false),
    automatedShuffle(false),
    automatedStutter(false),
    automatedReverse(false),
    automatedMix(false),
    automatedQuality(false),
    bufferSize(0),
    currentGridSize(0.25f),
    currentSwing(0.0f),
    currentShuffle(0.0f),
    currentStutter(0.0f),
    currentReverse(0.0f),
    currentMix(0.5f),
    currentQuality(1.0f)
{
}

BeatGrinderEffect::~BeatGrinderEffect() {
    shutdown();
}

bool BeatGrinderEffect::initialize() {
    bufferSize = static_cast<size_t>(44100 * 2.0f); // 2 Sekunden Buffer
    initializeGrids();
    return true;
}

void BeatGrinderEffect::shutdown() {
    grids.clear();
}

std::vector<PluginParameter> BeatGrinderEffect::getParameters() const {
    return {
        {"gridSize", 0.25f, 0.0625f, 1.0f},
        {"swing", 0.0f, 0.0f, 1.0f},
        {"shuffle", 0.0f, 0.0f, 1.0f},
        {"stutter", 0.0f, 0.0f, 1.0f},
        {"reverse", 0.0f, 0.0f, 1.0f},
        {"mix", 0.5f, 0.0f, 1.0f},
        {"quality", 1.0f, 0.0f, 1.0f}
    };
}

void BeatGrinderEffect::setParameter(const std::string& name, float value) {
    if (name == "gridSize") gridSize = value;
    else if (name == "swing") swing = value;
    else if (name == "shuffle") shuffle = value;
    else if (name == "stutter") stutter = value;
    else if (name == "reverse") reverse = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
}

float BeatGrinderEffect::getParameter(const std::string& name) const {
    if (name == "gridSize") return gridSize;
    else if (name == "swing") return swing;
    else if (name == "shuffle") return shuffle;
    else if (name == "stutter") return stutter;
    else if (name == "reverse") return reverse;
    else if (name == "mix") return mix;
    else if (name == "quality") return quality;
    return 0.0f;
}

void BeatGrinderEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "gridSize") automatedGridSize = automated;
    else if (name == "swing") automatedSwing = automated;
    else if (name == "shuffle") automatedShuffle = automated;
    else if (name == "stutter") automatedStutter = automated;
    else if (name == "reverse") automatedReverse = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool BeatGrinderEffect::isParameterAutomated(const std::string& name) const {
    if (name == "gridSize") return automatedGridSize;
    else if (name == "swing") return automatedSwing;
    else if (name == "shuffle") return automatedShuffle;
    else if (name == "stutter") return automatedStutter;
    else if (name == "reverse") return automatedReverse;
    else if (name == "mix") return automatedMix;
    else if (name == "quality") return automatedQuality;
    return false;
}

void BeatGrinderEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    // Update Parameter
    currentGridSize = gridSize;
    currentSwing = swing;
    currentShuffle = shuffle;
    currentStutter = stutter;
    currentReverse = reverse;
    currentMix = mix;
    currentQuality = quality;

    // Process Grids
    processGrids(buffer, framesPerBuffer);

    // Apply Effects
    applySwing(buffer, framesPerBuffer);
    applyShuffle(buffer, framesPerBuffer);
    applyStutter(buffer, framesPerBuffer);
    applyReverse(buffer, framesPerBuffer);
    applyMix(buffer, framesPerBuffer);
    applyQuality(buffer, framesPerBuffer);
}

void BeatGrinderEffect::loadPreset(const std::string& presetName) {
    // TODO: Implement preset loading
}

void BeatGrinderEffect::savePreset(const std::string& presetName) {
    // TODO: Implement preset saving
}

std::vector<std::string> BeatGrinderEffect::getAvailablePresets() const {
    return {}; // TODO: Implement preset list
}

void BeatGrinderEffect::initializeGrids() {
    grids.clear();
    for (int i = 0; i < 4; ++i) {
        Grid grid;
        grid.buffer.resize(bufferSize);
        grid.writePos = 0;
        grid.readPos = 0;
        grid.time = 0.0f;
        grid.level = 1.0f;
        grid.active = false;
        grid.reverse = false;
        grids.push_back(grid);
    }
}

void BeatGrinderEffect::updateGrids() {
    for (auto& grid : grids) {
        if (grid.active) {
            grid.time += currentGridSize;
            if (grid.time >= 1.0f) {
                grid.time = 0.0f;
                grid.reverse = !grid.reverse;
            }
        }
    }
}

void BeatGrinderEffect::processGrids(float* buffer, unsigned long framesPerBuffer) {
    updateGrids();
    
    for (size_t i = 0; i < framesPerBuffer; ++i) {
        float sample = buffer[i];
        
        for (auto& grid : grids) {
            if (grid.active) {
                // Write to buffer
                grid.buffer[grid.writePos] = sample;
                grid.writePos = (grid.writePos + 1) % bufferSize;
                
                // Read from buffer
                if (grid.reverse) {
                    grid.readPos = (grid.readPos + bufferSize - 1) % bufferSize;
                } else {
                    grid.readPos = (grid.readPos + 1) % bufferSize;
                }
                
                sample = grid.buffer[grid.readPos] * grid.level;
            }
        }
        
        buffer[i] = sample;
    }
}

void BeatGrinderEffect::applySwing(float* buffer, unsigned long framesPerBuffer) {
    if (currentSwing <= 0.0f) return;
    
    for (size_t i = 0; i < framesPerBuffer; i += 2) {
        if (i + 1 < framesPerBuffer) {
            float swingAmount = currentSwing * 0.5f;
            buffer[i] *= (1.0f - swingAmount);
            buffer[i + 1] *= (1.0f + swingAmount);
        }
    }
}

void BeatGrinderEffect::applyShuffle(float* buffer, unsigned long framesPerBuffer) {
    if (currentShuffle <= 0.0f) return;
    
    for (size_t i = 0; i < framesPerBuffer; i += 3) {
        if (i + 2 < framesPerBuffer) {
            float shuffleAmount = currentShuffle * 0.5f;
            buffer[i] *= (1.0f - shuffleAmount);
            buffer[i + 1] *= (1.0f + shuffleAmount);
            buffer[i + 2] *= (1.0f - shuffleAmount);
        }
    }
}

void BeatGrinderEffect::applyStutter(float* buffer, unsigned long framesPerBuffer) {
    if (currentStutter <= 0.0f) return;
    
    size_t stutterLength = static_cast<size_t>(currentStutter * framesPerBuffer * 0.25f);
    if (stutterLength == 0) return;
    
    for (size_t i = 0; i < framesPerBuffer; i += stutterLength * 2) {
        if (i + stutterLength < framesPerBuffer) {
            for (size_t j = 0; j < stutterLength; ++j) {
                buffer[i + j] = buffer[i];
            }
        }
    }
}

void BeatGrinderEffect::applyReverse(float* buffer, unsigned long framesPerBuffer) {
    if (currentReverse <= 0.0f) return;
    
    size_t reverseLength = static_cast<size_t>(currentReverse * framesPerBuffer * 0.25f);
    if (reverseLength == 0) return;
    
    for (size_t i = 0; i < framesPerBuffer; i += reverseLength * 2) {
        if (i + reverseLength < framesPerBuffer) {
            std::reverse(buffer + i, buffer + i + reverseLength);
        }
    }
}

void BeatGrinderEffect::applyMix(float* buffer, unsigned long framesPerBuffer) {
    if (currentMix == 0.5f) return;
    
    for (size_t i = 0; i < framesPerBuffer; ++i) {
        buffer[i] *= currentMix * 2.0f;
    }
}

void BeatGrinderEffect::applyQuality(float* buffer, unsigned long framesPerBuffer) {
    if (currentQuality >= 1.0f) return;
    
    // Simple low-pass filter based on quality
    float alpha = currentQuality;
    float prev = buffer[0];
    
    for (size_t i = 1; i < framesPerBuffer; ++i) {
        float current = buffer[i];
        buffer[i] = prev + alpha * (current - prev);
        prev = buffer[i];
    }
}

} // namespace VRMusicStudio 