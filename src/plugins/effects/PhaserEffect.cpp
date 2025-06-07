#include "PhaserEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

PhaserEffect::PhaserEffect()
    : rate(1.0f)
    , depth(0.5f)
    , feedback(0.3f)
    , mix(0.5f)
    , stereoWidth(0.5f)
    , syncRate(0.25f)
    , stages(4.0f)
    , phase(0.0f)
    , automatedRate(false)
    , automatedDepth(false)
    , automatedFeedback(false)
    , automatedMix(false)
    , automatedStereoWidth(false)
    , automatedSyncRate(false)
    , automatedStages(false)
    , automatedPhase(false)
    , lfoPhase(0.0f)
{
    // Allpass-Filter initialisieren
    filters.resize(static_cast<size_t>(stages));
    for (auto& filter : filters) {
        filter.x1 = filter.x2 = 0.0f;
        filter.y1 = filter.y2 = 0.0f;
        filter.a1 = filter.a2 = 0.0f;
    }
}

PhaserEffect::~PhaserEffect() {
}

bool PhaserEffect::initialize() {
    return true;
}

void PhaserEffect::shutdown() {
}

std::vector<PluginParameter> PhaserEffect::getParameters() const {
    return {
        {"rate", "Rate", PluginParameter::Type::Float, 0.1f, 10.0f, rate},
        {"depth", "Depth", PluginParameter::Type::Float, 0.0f, 1.0f, depth},
        {"feedback", "Feedback", PluginParameter::Type::Float, 0.0f, 0.9f, feedback},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"stereoWidth", "Stereo Width", PluginParameter::Type::Float, 0.0f, 1.0f, stereoWidth},
        {"syncRate", "Sync Rate", PluginParameter::Type::Float, 0.0625f, 1.0f, syncRate},
        {"stages", "Stages", PluginParameter::Type::Float, 2.0f, 12.0f, stages},
        {"phase", "Phase", PluginParameter::Type::Float, 0.0f, 1.0f, phase}
    };
}

void PhaserEffect::setParameter(const std::string& name, float value) {
    if (name == "rate") rate = value;
    else if (name == "depth") depth = value;
    else if (name == "feedback") feedback = value;
    else if (name == "mix") mix = value;
    else if (name == "stereoWidth") stereoWidth = value;
    else if (name == "syncRate") syncRate = value;
    else if (name == "stages") {
        stages = value;
        filters.resize(static_cast<size_t>(stages));
    }
    else if (name == "phase") phase = value;
}

float PhaserEffect::getParameter(const std::string& name) const {
    if (name == "rate") return rate;
    if (name == "depth") return depth;
    if (name == "feedback") return feedback;
    if (name == "mix") return mix;
    if (name == "stereoWidth") return stereoWidth;
    if (name == "syncRate") return syncRate;
    if (name == "stages") return stages;
    if (name == "phase") return phase;
    return 0.0f;
}

void PhaserEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "rate") automatedRate = automated;
    else if (name == "depth") automatedDepth = automated;
    else if (name == "feedback") automatedFeedback = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "stereoWidth") automatedStereoWidth = automated;
    else if (name == "syncRate") automatedSyncRate = automated;
    else if (name == "stages") automatedStages = automated;
    else if (name == "phase") automatedPhase = automated;
}

bool PhaserEffect::isParameterAutomated(const std::string& name) const {
    if (name == "rate") return automatedRate;
    if (name == "depth") return automatedDepth;
    if (name == "feedback") return automatedFeedback;
    if (name == "mix") return automatedMix;
    if (name == "stereoWidth") return automatedStereoWidth;
    if (name == "syncRate") return automatedSyncRate;
    if (name == "stages") return automatedStages;
    if (name == "phase") return automatedPhase;
    return false;
}

void PhaserEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // LFO-Wert berechnen
        float lfoValue = processLFO();
        
        // Filter-Frequenz berechnen
        float frequency = 440.0f * std::pow(2.0f, lfoValue * depth * 4.0f);  // 440Hz - 7040Hz
        updateFilterCoefficients(frequency);
        
        // Allpass-Filter anwenden
        float filteredLeft = left;
        float filteredRight = right;
        
        for (auto& filter : filters) {
            filteredLeft = filter.a1 * filteredLeft + filter.a2 * filter.x1 + filter.y1;
            filter.x1 = filteredLeft;
            filter.y1 = filteredLeft;
            
            filteredRight = filter.a1 * filteredRight + filter.a2 * filter.x2 + filter.y2;
            filter.x2 = filteredRight;
            filter.y2 = filteredRight;
        }
        
        // Feedback
        filteredLeft = left + filteredLeft * feedback;
        filteredRight = right + filteredRight * feedback;
        
        // Stereo-Width
        float mid = (filteredLeft + filteredRight) * 0.5f;
        float side = (filteredLeft - filteredRight) * 0.5f;
        filteredLeft = mid + side * stereoWidth;
        filteredRight = mid - side * stereoWidth;
        
        // Mix
        buffer[i] = left * (1.0f - mix) + filteredLeft * mix;
        buffer[i + 1] = right * (1.0f - mix) + filteredRight * mix;
    }
}

float PhaserEffect::processLFO() {
    // LFO-Phase aktualisieren
    float lfoRate = rate;
    if (syncRate > 0.0f) {
        lfoRate = calculateSyncTime();
    }
    
    lfoPhase += lfoRate / 44100.0f;
    if (lfoPhase >= 1.0f) {
        lfoPhase -= 1.0f;
    }
    
    // Sinus-Welle mit Phase
    return 0.5f * (1.0f + std::sin(2.0f * M_PI * (lfoPhase + phase)));
}

float PhaserEffect::calculateSyncTime() {
    // Sync-Zeit basierend auf BPM berechnen (angenommen 120 BPM)
    return (60.0f / 120.0f) * syncRate;
}

void PhaserEffect::updateFilterCoefficients(float frequency) {
    float w0 = 2.0f * M_PI * frequency / 44100.0f;
    float alpha = std::sin(w0) / (2.0f * 0.707f);  // Q = 0.707
    
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * std::cos(w0);
    float a2 = 1.0f - alpha;
    
    // Koeffizienten für alle Filter aktualisieren
    for (auto& filter : filters) {
        filter.a1 = a1 / a0;
        filter.a2 = a2 / a0;
    }
}

void PhaserEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        rate = 1.0f;
        depth = 0.5f;
        feedback = 0.3f;
        mix = 0.5f;
        stereoWidth = 0.5f;
        syncRate = 0.25f;
        stages = 4.0f;
        phase = 0.0f;
    }
    else if (presetName == "Jet") {
        rate = 2.0f;
        depth = 0.7f;
        feedback = 0.5f;
        mix = 0.7f;
        stereoWidth = 0.7f;
        syncRate = 0.5f;
        stages = 6.0f;
        phase = 0.25f;
    }
    else if (presetName == "Subtle") {
        rate = 0.5f;
        depth = 0.3f;
        feedback = 0.2f;
        mix = 0.3f;
        stereoWidth = 0.3f;
        syncRate = 0.125f;
        stages = 2.0f;
        phase = 0.0f;
    }
    
    // Filter neu initialisieren
    filters.resize(static_cast<size_t>(stages));
    for (auto& filter : filters) {
        filter.x1 = filter.x2 = 0.0f;
        filter.y1 = filter.y2 = 0.0f;
        filter.a1 = filter.a2 = 0.0f;
    }
}

void PhaserEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> PhaserEffect::getAvailablePresets() const {
    return {"Standard", "Jet", "Subtle"};
}

} // namespace VR_DAW 