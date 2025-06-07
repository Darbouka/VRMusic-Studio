#include "BeatGrinderEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

BeatGrinderEffect::BeatGrinderEffect()
    : gridSize(1.0f)
    , swing(0.0f)
    , shuffle(0.0f)
    , stutter(0.0f)
    , reverse(0.0f)
    , mix(0.5f)
    , quality(1.0f)
    , automatedGridSize(false)
    , automatedSwing(false)
    , automatedShuffle(false)
    , automatedStutter(false)
    , automatedReverse(false)
    , automatedMix(false)
    , automatedQuality(false)
    , bufferSize(44100 * 2) // 2 Sekunden bei 44.1kHz
{
    std::random_device rd;
    rng.seed(rd());
    initializeGrids();
}

BeatGrinderEffect::~BeatGrinderEffect() {
    shutdown();
}

bool BeatGrinderEffect::initialize() {
    initializeGrids();
    return true;
}

void BeatGrinderEffect::shutdown() {
    grids.clear();
}

void BeatGrinderEffect::initializeGrids() {
    grids.clear();
    
    // Erstelle 16 Grids für komplexe Beat-Manipulation
    for (int i = 0; i < 16; ++i) {
        Grid grid;
        grid.buffer.resize(bufferSize, 0.0f);
        grid.writePos = 0;
        grid.readPos = 0;
        grid.time = 1.0f;
        grid.level = 1.0f;
        grid.active = false;
        grid.reverse = false;
        grids.push_back(grid);
    }
}

void BeatGrinderEffect::updateGrids() {
    for (auto& grid : grids) {
        if (grid.active) {
            // Aktualisiere Grid-Positionen
            grid.writePos = (grid.writePos + 1) % bufferSize;
            grid.readPos = (grid.readPos + 1) % bufferSize;
            
            // Wende Grid-Größe an
            if (gridSize != currentGridSize) {
                grid.time = gridSize;
                currentGridSize = gridSize;
            }
            
            // Wende Reverse an
            if (reverse != currentReverse) {
                grid.reverse = (reverse > 0.5f);
                currentReverse = reverse;
            }
        }
    }
}

void BeatGrinderEffect::processGrid(Grid& grid, float* buffer, unsigned long framesPerBuffer) {
    if (!grid.active) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        // Berechne Grid-Position
        float gridPos = static_cast<float>(i) / framesPerBuffer;
        
        // Wende Swing an
        if (swing > 0.0f) {
            gridPos = applySwingToPosition(gridPos);
        }
        
        // Wende Shuffle an
        if (shuffle > 0.0f) {
            gridPos = applyShuffleToPosition(gridPos);
        }
        
        // Wende Stutter an
        if (stutter > 0.0f) {
            gridPos = applyStutterToPosition(gridPos);
        }
        
        // Berechne Buffer-Position
        size_t pos = static_cast<size_t>(gridPos * bufferSize);
        if (grid.reverse) {
            pos = bufferSize - 1 - pos;
        }
        
        // Schreibe in Buffer
        buffer[i] = grid.buffer[pos] * grid.level;
    }
}

float BeatGrinderEffect::applySwingToPosition(float pos) {
    float swingAmount = swing * 0.5f;
    float swingPhase = std::sin(pos * M_PI * 2.0f);
    return pos + swingAmount * swingPhase;
}

float BeatGrinderEffect::applyShuffleToPosition(float pos) {
    float shuffleAmount = shuffle * 0.25f;
    float shufflePhase = std::sin(pos * M_PI * 4.0f);
    return pos + shuffleAmount * shufflePhase;
}

float BeatGrinderEffect::applyStutterToPosition(float pos) {
    float stutterAmount = stutter * 0.5f;
    float stutterPhase = std::sin(pos * M_PI * 8.0f);
    return pos + stutterAmount * stutterPhase;
}

void BeatGrinderEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateGrids();
    
    // Verarbeite jeden Grid
    for (auto& grid : grids) {
        processGrid(grid, buffer, framesPerBuffer);
    }
    
    // Wende Mix an
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = buffer[i] * mix;
    }
}

std::vector<PluginParameter> BeatGrinderEffect::getParameters() const {
    return {
        {"gridSize", gridSize, 0.1f, 4.0f, ""},
        {"swing", swing, 0.0f, 1.0f, ""},
        {"shuffle", shuffle, 0.0f, 1.0f, ""},
        {"stutter", stutter, 0.0f, 1.0f, ""},
        {"reverse", reverse, 0.0f, 1.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
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
    if (name == "swing") return swing;
    if (name == "shuffle") return shuffle;
    if (name == "stutter") return stutter;
    if (name == "reverse") return reverse;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
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
    if (name == "swing") return automatedSwing;
    if (name == "shuffle") return automatedShuffle;
    if (name == "stutter") return automatedStutter;
    if (name == "reverse") return automatedReverse;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    return false;
}

void BeatGrinderEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        gridSize = 1.0f;
        swing = 0.0f;
        shuffle = 0.0f;
        stutter = 0.0f;
        reverse = 0.0f;
        mix = 0.5f;
        quality = 1.0f;
    }
    else if (presetName == "Heavy") {
        gridSize = 0.5f;
        swing = 0.7f;
        shuffle = 0.5f;
        stutter = 0.3f;
        reverse = 0.2f;
        mix = 0.7f;
        quality = 1.0f;
    }
    else if (presetName == "Subtle") {
        gridSize = 2.0f;
        swing = 0.3f;
        shuffle = 0.2f;
        stutter = 0.1f;
        reverse = 0.0f;
        mix = 0.3f;
        quality = 1.0f;
    }
    
    updateGrids();
}

void BeatGrinderEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> BeatGrinderEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 