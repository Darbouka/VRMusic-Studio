#include "IceverbEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

IceverbEffect::IceverbEffect()
    : size(0.8f)
    , decay(0.7f)
    , diffusion(0.5f)
    , modulation(0.3f)
    , shimmer(0.2f)
    , mix(0.5f)
    , quality(1.0f)
    , automatedSize(false)
    , automatedDecay(false)
    , automatedDiffusion(false)
    , automatedModulation(false)
    , automatedShimmer(false)
    , automatedMix(false)
    , automatedQuality(false)
    , bufferSize(44100 * 4) // 4 Sekunden bei 44.1kHz
{
    std::random_device rd;
    rng.seed(rd());
    initializeDelayLines();
}

IceverbEffect::~IceverbEffect() {
    shutdown();
}

bool IceverbEffect::initialize() {
    initializeDelayLines();
    return true;
}

void IceverbEffect::shutdown() {
    delayLines.clear();
}

void IceverbEffect::initializeDelayLines() {
    delayLines.clear();
    
    // Erstelle 8 Delay-Lines für komplexen Hall
    for (int i = 0; i < 8; ++i) {
        DelayLine line;
        line.buffer.resize(bufferSize, 0.0f);
        line.writePos = 0;
        line.readPos = 0;
        line.time = 1.0f;
        line.feedback = 0.7f;
        line.modulation = 0.0f;
        line.shimmer = 0.0f;
        delayLines.push_back(line);
    }
}

void IceverbEffect::updateDelayLines() {
    for (auto& line : delayLines) {
        // Aktualisiere Delay-Positionen
        line.writePos = (line.writePos + 1) % bufferSize;
        line.readPos = (line.readPos + 1) % bufferSize;
        
        // Wende Size an
        if (size != currentSize) {
            line.time = size;
            currentSize = size;
        }
        
        // Wende Decay an
        if (decay != currentDecay) {
            line.feedback = decay;
            currentDecay = decay;
        }
        
        // Wende Modulation an
        if (modulation != currentModulation) {
            line.modulation = modulation;
            currentModulation = modulation;
        }
        
        // Wende Shimmer an
        if (shimmer != currentShimmer) {
            line.shimmer = shimmer;
            currentShimmer = shimmer;
        }
    }
}

void IceverbEffect::processDelayLine(DelayLine& line, float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        // Berechne Delay-Position mit Modulation
        float modAmount = line.modulation * std::sin(2.0f * M_PI * i / framesPerBuffer);
        size_t delayPos = static_cast<size_t>((line.time + modAmount) * bufferSize);
        
        // Lese Sample aus Delay-Line
        float sample = line.buffer[delayPos];
        
        // Wende Feedback an
        sample *= line.feedback;
        
        // Wende Shimmer an
        if (line.shimmer > 0.0f) {
            float shimmerAmount = line.shimmer * std::sin(4.0f * M_PI * i / framesPerBuffer);
            sample *= (1.0f + shimmerAmount);
        }
        
        // Schreibe in Output Buffer
        buffer[i] = sample;
        
        // Schreibe in Delay-Line
        line.buffer[line.writePos] = buffer[i];
    }
}

void IceverbEffect::applyModulation(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float modAmount = modulation * std::sin(2.0f * M_PI * i / framesPerBuffer);
        buffer[i] *= (1.0f + modAmount);
    }
}

void IceverbEffect::applyShimmer(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float shimmerAmount = shimmer * std::sin(4.0f * M_PI * i / framesPerBuffer);
        buffer[i] *= (1.0f + shimmerAmount);
    }
}

void IceverbEffect::applyDiffusion(float* buffer, unsigned long framesPerBuffer) {
    if (diffusion == 0.0f) return;
    
    std::uniform_real_distribution<float> dist(-diffusion, diffusion);
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float diffAmount = dist(rng);
        buffer[i] *= (1.0f + diffAmount);
    }
}

void IceverbEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateDelayLines();
    
    // Verarbeite jede Delay-Line
    for (auto& line : delayLines) {
        processDelayLine(line, buffer, framesPerBuffer);
    }
    
    // Wende Modulation an
    if (modulation > 0.0f) {
        applyModulation(buffer, framesPerBuffer);
    }
    
    // Wende Shimmer an
    if (shimmer > 0.0f) {
        applyShimmer(buffer, framesPerBuffer);
    }
    
    // Wende Diffusion an
    if (diffusion > 0.0f) {
        applyDiffusion(buffer, framesPerBuffer);
    }
    
    // Wende Mix an
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = buffer[i] * mix;
    }
}

std::vector<PluginParameter> IceverbEffect::getParameters() const {
    return {
        {"size", size, 0.1f, 1.0f, ""},
        {"decay", decay, 0.0f, 1.0f, ""},
        {"diffusion", diffusion, 0.0f, 1.0f, ""},
        {"modulation", modulation, 0.0f, 1.0f, ""},
        {"shimmer", shimmer, 0.0f, 1.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void IceverbEffect::setParameter(const std::string& name, float value) {
    if (name == "size") size = value;
    else if (name == "decay") decay = value;
    else if (name == "diffusion") diffusion = value;
    else if (name == "modulation") modulation = value;
    else if (name == "shimmer") shimmer = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
}

float IceverbEffect::getParameter(const std::string& name) const {
    if (name == "size") return size;
    if (name == "decay") return decay;
    if (name == "diffusion") return diffusion;
    if (name == "modulation") return modulation;
    if (name == "shimmer") return shimmer;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
    return 0.0f;
}

void IceverbEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "size") automatedSize = automated;
    else if (name == "decay") automatedDecay = automated;
    else if (name == "diffusion") automatedDiffusion = automated;
    else if (name == "modulation") automatedModulation = automated;
    else if (name == "shimmer") automatedShimmer = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool IceverbEffect::isParameterAutomated(const std::string& name) const {
    if (name == "size") return automatedSize;
    if (name == "decay") return automatedDecay;
    if (name == "diffusion") return automatedDiffusion;
    if (name == "modulation") return automatedModulation;
    if (name == "shimmer") return automatedShimmer;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    return false;
}

void IceverbEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        size = 0.8f;
        decay = 0.7f;
        diffusion = 0.5f;
        modulation = 0.3f;
        shimmer = 0.2f;
        mix = 0.5f;
        quality = 1.0f;
    }
    else if (presetName == "Heavy") {
        size = 0.9f;
        decay = 0.8f;
        diffusion = 0.7f;
        modulation = 0.5f;
        shimmer = 0.4f;
        mix = 0.7f;
        quality = 1.0f;
    }
    else if (presetName == "Subtle") {
        size = 0.6f;
        decay = 0.5f;
        diffusion = 0.3f;
        modulation = 0.2f;
        shimmer = 0.1f;
        mix = 0.3f;
        quality = 1.0f;
    }
    
    updateDelayLines();
}

void IceverbEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> IceverbEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 