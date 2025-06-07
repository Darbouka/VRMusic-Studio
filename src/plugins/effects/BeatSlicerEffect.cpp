#include "BeatSlicerEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

BeatSlicerEffect::BeatSlicerEffect()
    : sliceSize(0.25f)
    , sliceCount(4.0f)
    , sliceOffset(0.0f)
    , sliceRandom(0.0f)
    , sliceReverse(0.0f)
    , mix(0.5f)
    , quality(1.0f)
    , automatedSliceSize(false)
    , automatedSliceCount(false)
    , automatedSliceOffset(false)
    , automatedSliceRandom(false)
    , automatedSliceReverse(false)
    , automatedMix(false)
    , automatedQuality(false)
    , bufferSize(44100 * 2) // 2 Sekunden bei 44.1kHz
{
    std::random_device rd;
    rng.seed(rd());
    initializeSlices();
}

BeatSlicerEffect::~BeatSlicerEffect() {
    shutdown();
}

bool BeatSlicerEffect::initialize() {
    initializeSlices();
    return true;
}

void BeatSlicerEffect::shutdown() {
    slices.clear();
}

void BeatSlicerEffect::initializeSlices() {
    slices.clear();
    
    // Erstelle initiale Slices
    int numSlices = static_cast<int>(sliceCount);
    for (int i = 0; i < numSlices; ++i) {
        Slice slice;
        slice.buffer.resize(bufferSize, 0.0f);
        slice.startPos = 0;
        slice.endPos = bufferSize;
        slice.currentPos = 0;
        slice.level = 1.0f;
        slice.reverse = false;
        slice.active = true;
        slices.push_back(slice);
    }
}

void BeatSlicerEffect::updateSlices() {
    // Aktualisiere Slice-Positionen wenn sich Parameter ändern
    if (sliceSize != currentSliceSize || 
        sliceCount != currentSliceCount || 
        sliceOffset != currentSliceOffset) {
        
        updateSlicePositions();
        currentSliceSize = sliceSize;
        currentSliceCount = sliceCount;
        currentSliceOffset = sliceOffset;
    }
    
    // Wende Randomisierung an
    if (sliceRandom != currentSliceRandom) {
        applySliceRandomization();
        currentSliceRandom = sliceRandom;
    }
    
    // Wende Reversierung an
    if (sliceReverse != currentSliceReverse) {
        applySliceReversal();
        currentSliceReverse = sliceReverse;
    }
}

void BeatSlicerEffect::updateSlicePositions() {
    int numSlices = static_cast<int>(sliceCount);
    size_t sliceLength = static_cast<size_t>(bufferSize * sliceSize);
    
    for (int i = 0; i < numSlices; ++i) {
        if (i < slices.size()) {
            Slice& slice = slices[i];
            slice.startPos = i * sliceLength;
            slice.endPos = slice.startPos + sliceLength;
            slice.currentPos = slice.startPos;
            
            // Wende Offset an
            if (sliceOffset != 0.0f) {
                size_t offset = static_cast<size_t>(sliceLength * sliceOffset);
                slice.startPos = (slice.startPos + offset) % bufferSize;
                slice.endPos = (slice.endPos + offset) % bufferSize;
            }
        }
    }
}

void BeatSlicerEffect::applySliceRandomization() {
    if (sliceRandom == 0.0f) return;
    
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (auto& slice : slices) {
        if (dist(rng) < sliceRandom) {
            // Zufällige Position innerhalb des Buffers
            slice.startPos = static_cast<size_t>(dist(rng) * bufferSize);
            slice.endPos = (slice.startPos + static_cast<size_t>(bufferSize * sliceSize)) % bufferSize;
            slice.currentPos = slice.startPos;
        }
    }
}

void BeatSlicerEffect::applySliceReversal() {
    for (auto& slice : slices) {
        slice.reverse = (sliceReverse > 0.5f);
    }
}

void BeatSlicerEffect::processSlice(Slice& slice, float* buffer, unsigned long framesPerBuffer) {
    if (!slice.active) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        if (slice.currentPos >= slice.endPos) {
            slice.currentPos = slice.startPos;
        }
        
        // Lese Sample aus Buffer
        float sample = slice.buffer[slice.currentPos];
        
        // Wende Level an
        sample *= slice.level;
        
        // Schreibe in Output Buffer
        buffer[i] = sample;
        
        // Aktualisiere Position
        if (slice.reverse) {
            if (slice.currentPos == slice.startPos) {
                slice.currentPos = slice.endPos - 1;
            } else {
                slice.currentPos--;
            }
        } else {
            slice.currentPos++;
        }
    }
}

void BeatSlicerEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateSlices();
    
    // Verarbeite jeden Slice
    for (auto& slice : slices) {
        processSlice(slice, buffer, framesPerBuffer);
    }
    
    // Wende Mix an
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = buffer[i] * mix;
    }
}

std::vector<PluginParameter> BeatSlicerEffect::getParameters() const {
    return {
        {"sliceSize", sliceSize, 0.1f, 1.0f, ""},
        {"sliceCount", sliceCount, 1.0f, 16.0f, ""},
        {"sliceOffset", sliceOffset, 0.0f, 1.0f, ""},
        {"sliceRandom", sliceRandom, 0.0f, 1.0f, ""},
        {"sliceReverse", sliceReverse, 0.0f, 1.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void BeatSlicerEffect::setParameter(const std::string& name, float value) {
    if (name == "sliceSize") sliceSize = value;
    else if (name == "sliceCount") sliceCount = value;
    else if (name == "sliceOffset") sliceOffset = value;
    else if (name == "sliceRandom") sliceRandom = value;
    else if (name == "sliceReverse") sliceReverse = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
}

float BeatSlicerEffect::getParameter(const std::string& name) const {
    if (name == "sliceSize") return sliceSize;
    if (name == "sliceCount") return sliceCount;
    if (name == "sliceOffset") return sliceOffset;
    if (name == "sliceRandom") return sliceRandom;
    if (name == "sliceReverse") return sliceReverse;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
    return 0.0f;
}

void BeatSlicerEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "sliceSize") automatedSliceSize = automated;
    else if (name == "sliceCount") automatedSliceCount = automated;
    else if (name == "sliceOffset") automatedSliceOffset = automated;
    else if (name == "sliceRandom") automatedSliceRandom = automated;
    else if (name == "sliceReverse") automatedSliceReverse = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool BeatSlicerEffect::isParameterAutomated(const std::string& name) const {
    if (name == "sliceSize") return automatedSliceSize;
    if (name == "sliceCount") return automatedSliceCount;
    if (name == "sliceOffset") return automatedSliceOffset;
    if (name == "sliceRandom") return automatedSliceRandom;
    if (name == "sliceReverse") return automatedSliceReverse;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    return false;
}

void BeatSlicerEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        sliceSize = 0.25f;
        sliceCount = 4.0f;
        sliceOffset = 0.0f;
        sliceRandom = 0.0f;
        sliceReverse = 0.0f;
        mix = 0.5f;
        quality = 1.0f;
    }
    else if (presetName == "Heavy") {
        sliceSize = 0.125f;
        sliceCount = 8.0f;
        sliceOffset = 0.25f;
        sliceRandom = 0.5f;
        sliceReverse = 0.3f;
        mix = 0.7f;
        quality = 1.0f;
    }
    else if (presetName == "Subtle") {
        sliceSize = 0.5f;
        sliceCount = 2.0f;
        sliceOffset = 0.1f;
        sliceRandom = 0.2f;
        sliceReverse = 0.0f;
        mix = 0.3f;
        quality = 1.0f;
    }
    
    updateSlices();
}

void BeatSlicerEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> BeatSlicerEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 