#include "BeatBreakerEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

BeatBreakerEffect::BeatBreakerEffect()
    : breakSize(0.25f)
    , breakCount(4.0f)
    , breakOffset(0.0f)
    , breakRandom(0.0f)
    , breakReverse(0.0f)
    , breakStutter(0.0f)
    , mix(0.5f)
    , quality(1.0f)
    , automatedBreakSize(false)
    , automatedBreakCount(false)
    , automatedBreakOffset(false)
    , automatedBreakRandom(false)
    , automatedBreakReverse(false)
    , automatedBreakStutter(false)
    , automatedMix(false)
    , automatedQuality(false)
    , bufferSize(44100 * 2) // 2 Sekunden bei 44.1kHz
{
    std::random_device rd;
    rng.seed(rd());
    initializeBreaks();
}

BeatBreakerEffect::~BeatBreakerEffect() {
    shutdown();
}

bool BeatBreakerEffect::initialize() {
    initializeBreaks();
    return true;
}

void BeatBreakerEffect::shutdown() {
    breaks.clear();
}

void BeatBreakerEffect::initializeBreaks() {
    breaks.clear();
    
    // Erstelle initiale Breaks
    int numBreaks = static_cast<int>(breakCount);
    for (int i = 0; i < numBreaks; ++i) {
        Break break_;
        break_.buffer.resize(bufferSize, 0.0f);
        break_.startPos = 0;
        break_.endPos = bufferSize;
        break_.currentPos = 0;
        break_.level = 1.0f;
        break_.reverse = false;
        break_.stutter = false;
        break_.active = true;
        breaks.push_back(break_);
    }
}

void BeatBreakerEffect::updateBreaks() {
    // Aktualisiere Break-Positionen wenn sich Parameter ändern
    if (breakSize != currentBreakSize || 
        breakCount != currentBreakCount || 
        breakOffset != currentBreakOffset) {
        
        updateBreakPositions();
        currentBreakSize = breakSize;
        currentBreakCount = breakCount;
        currentBreakOffset = breakOffset;
    }
    
    // Wende Randomisierung an
    if (breakRandom != currentBreakRandom) {
        applyBreakRandomization();
        currentBreakRandom = breakRandom;
    }
    
    // Wende Reversierung an
    if (breakReverse != currentBreakReverse) {
        applyBreakReversal();
        currentBreakReverse = breakReverse;
    }
    
    // Wende Stutter an
    if (breakStutter != currentBreakStutter) {
        applyBreakStutter();
        currentBreakStutter = breakStutter;
    }
}

void BeatBreakerEffect::updateBreakPositions() {
    int numBreaks = static_cast<int>(breakCount);
    size_t breakLength = static_cast<size_t>(bufferSize * breakSize);
    
    for (int i = 0; i < numBreaks; ++i) {
        if (i < breaks.size()) {
            Break& break_ = breaks[i];
            break_.startPos = i * breakLength;
            break_.endPos = break_.startPos + breakLength;
            break_.currentPos = break_.startPos;
            
            // Wende Offset an
            if (breakOffset != 0.0f) {
                size_t offset = static_cast<size_t>(breakLength * breakOffset);
                break_.startPos = (break_.startPos + offset) % bufferSize;
                break_.endPos = (break_.endPos + offset) % bufferSize;
            }
        }
    }
}

void BeatBreakerEffect::applyBreakRandomization() {
    if (breakRandom == 0.0f) return;
    
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (auto& break_ : breaks) {
        if (dist(rng) < breakRandom) {
            // Zufällige Position innerhalb des Buffers
            break_.startPos = static_cast<size_t>(dist(rng) * bufferSize);
            break_.endPos = (break_.startPos + static_cast<size_t>(bufferSize * breakSize)) % bufferSize;
            break_.currentPos = break_.startPos;
        }
    }
}

void BeatBreakerEffect::applyBreakReversal() {
    for (auto& break_ : breaks) {
        break_.reverse = (breakReverse > 0.5f);
    }
}

void BeatBreakerEffect::applyBreakStutter() {
    for (auto& break_ : breaks) {
        break_.stutter = (breakStutter > 0.5f);
    }
}

void BeatBreakerEffect::processBreak(Break& break_, float* buffer, unsigned long framesPerBuffer) {
    if (!break_.active) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        if (break_.currentPos >= break_.endPos) {
            break_.currentPos = break_.startPos;
        }
        
        // Lese Sample aus Buffer
        float sample = break_.buffer[break_.currentPos];
        
        // Wende Level an
        sample *= break_.level;
        
        // Wende Stutter an
        if (break_.stutter) {
            static size_t stutterCount = 0;
            if (stutterCount < 4) { // 4-facher Stutter
                sample = break_.buffer[break_.currentPos];
                stutterCount++;
            } else {
                stutterCount = 0;
            }
        }
        
        // Schreibe in Output Buffer
        buffer[i] = sample;
        
        // Aktualisiere Position
        if (break_.reverse) {
            if (break_.currentPos == break_.startPos) {
                break_.currentPos = break_.endPos - 1;
            } else {
                break_.currentPos--;
            }
        } else {
            break_.currentPos++;
        }
    }
}

void BeatBreakerEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateBreaks();
    
    // Verarbeite jeden Break
    for (auto& break_ : breaks) {
        processBreak(break_, buffer, framesPerBuffer);
    }
    
    // Wende Mix an
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = buffer[i] * mix;
    }
}

std::vector<PluginParameter> BeatBreakerEffect::getParameters() const {
    return {
        {"breakSize", breakSize, 0.1f, 1.0f, ""},
        {"breakCount", breakCount, 1.0f, 16.0f, ""},
        {"breakOffset", breakOffset, 0.0f, 1.0f, ""},
        {"breakRandom", breakRandom, 0.0f, 1.0f, ""},
        {"breakReverse", breakReverse, 0.0f, 1.0f, ""},
        {"breakStutter", breakStutter, 0.0f, 1.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void BeatBreakerEffect::setParameter(const std::string& name, float value) {
    if (name == "breakSize") breakSize = value;
    else if (name == "breakCount") breakCount = value;
    else if (name == "breakOffset") breakOffset = value;
    else if (name == "breakRandom") breakRandom = value;
    else if (name == "breakReverse") breakReverse = value;
    else if (name == "breakStutter") breakStutter = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
}

float BeatBreakerEffect::getParameter(const std::string& name) const {
    if (name == "breakSize") return breakSize;
    if (name == "breakCount") return breakCount;
    if (name == "breakOffset") return breakOffset;
    if (name == "breakRandom") return breakRandom;
    if (name == "breakReverse") return breakReverse;
    if (name == "breakStutter") return breakStutter;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
    return 0.0f;
}

void BeatBreakerEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "breakSize") automatedBreakSize = automated;
    else if (name == "breakCount") automatedBreakCount = automated;
    else if (name == "breakOffset") automatedBreakOffset = automated;
    else if (name == "breakRandom") automatedBreakRandom = automated;
    else if (name == "breakReverse") automatedBreakReverse = automated;
    else if (name == "breakStutter") automatedBreakStutter = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool BeatBreakerEffect::isParameterAutomated(const std::string& name) const {
    if (name == "breakSize") return automatedBreakSize;
    if (name == "breakCount") return automatedBreakCount;
    if (name == "breakOffset") return automatedBreakOffset;
    if (name == "breakRandom") return automatedBreakRandom;
    if (name == "breakReverse") return automatedBreakReverse;
    if (name == "breakStutter") return automatedBreakStutter;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    return false;
}

void BeatBreakerEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        breakSize = 0.25f;
        breakCount = 4.0f;
        breakOffset = 0.0f;
        breakRandom = 0.0f;
        breakReverse = 0.0f;
        breakStutter = 0.0f;
        mix = 0.5f;
        quality = 1.0f;
    }
    else if (presetName == "Heavy") {
        breakSize = 0.125f;
        breakCount = 8.0f;
        breakOffset = 0.25f;
        breakRandom = 0.5f;
        breakReverse = 0.3f;
        breakStutter = 0.4f;
        mix = 0.7f;
        quality = 1.0f;
    }
    else if (presetName == "Subtle") {
        breakSize = 0.5f;
        breakCount = 2.0f;
        breakOffset = 0.1f;
        breakRandom = 0.2f;
        breakReverse = 0.0f;
        breakStutter = 0.1f;
        mix = 0.3f;
        quality = 1.0f;
    }
    
    updateBreaks();
}

void BeatBreakerEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> BeatBreakerEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 