#include "WahWahEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

WahWahEffect::WahWahEffect()
    : rate(2.0f)
    , depth(0.5f)
    , mix(0.5f)
    , syncRate(0.0f)
    , phase(0.0f)
    , q(2.0f)
    , minFreq(500.0f)
    , maxFreq(2000.0f)
    , automatedRate(false)
    , automatedDepth(false)
    , automatedMix(false)
    , automatedSyncRate(false)
    , automatedPhase(false)
    , automatedQ(false)
    , automatedMinFreq(false)
    , automatedMaxFreq(false)
    , lfoPhase(0.0f)
    , x1(0.0f)
    , x2(0.0f)
    , y1(0.0f)
    , y2(0.0f)
    , a0(0.0f)
    , a1(0.0f)
    , a2(0.0f)
    , b1(0.0f)
    , b2(0.0f)
{
}

WahWahEffect::~WahWahEffect() {
}

bool WahWahEffect::initialize() {
    lfoPhase = 0.0f;
    x1 = x2 = y1 = y2 = 0.0f;
    updateFilterCoefficients(minFreq);
    return true;
}

void WahWahEffect::shutdown() {
}

std::vector<PluginParameter> WahWahEffect::getParameters() const {
    return {
        {"rate", "Rate", PluginParameter::Type::Float, 0.1f, 10.0f, rate},
        {"depth", "Depth", PluginParameter::Type::Float, 0.0f, 1.0f, depth},
        {"mix", "Mix", PluginParameter::Type::Float, 0.0f, 1.0f, mix},
        {"syncRate", "Sync Rate", PluginParameter::Type::Float, 0.0f, 1.0f, syncRate},
        {"phase", "Phase", PluginParameter::Type::Float, 0.0f, 1.0f, phase},
        {"q", "Q", PluginParameter::Type::Float, 0.1f, 10.0f, q},
        {"minFreq", "Min Frequency", PluginParameter::Type::Float, 200.0f, 1000.0f, minFreq},
        {"maxFreq", "Max Frequency", PluginParameter::Type::Float, 1000.0f, 4000.0f, maxFreq}
    };
}

void WahWahEffect::setParameter(const std::string& name, float value) {
    if (name == "rate") rate = value;
    else if (name == "depth") depth = value;
    else if (name == "mix") mix = value;
    else if (name == "syncRate") syncRate = value;
    else if (name == "phase") phase = value;
    else if (name == "q") q = value;
    else if (name == "minFreq") minFreq = value;
    else if (name == "maxFreq") maxFreq = value;
}

float WahWahEffect::getParameter(const std::string& name) const {
    if (name == "rate") return rate;
    if (name == "depth") return depth;
    if (name == "mix") return mix;
    if (name == "syncRate") return syncRate;
    if (name == "phase") return phase;
    if (name == "q") return q;
    if (name == "minFreq") return minFreq;
    if (name == "maxFreq") return maxFreq;
    return 0.0f;
}

void WahWahEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "rate") automatedRate = automated;
    else if (name == "depth") automatedDepth = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "syncRate") automatedSyncRate = automated;
    else if (name == "phase") automatedPhase = automated;
    else if (name == "q") automatedQ = automated;
    else if (name == "minFreq") automatedMinFreq = automated;
    else if (name == "maxFreq") automatedMaxFreq = automated;
}

bool WahWahEffect::isParameterAutomated(const std::string& name) const {
    if (name == "rate") return automatedRate;
    if (name == "depth") return automatedDepth;
    if (name == "mix") return automatedMix;
    if (name == "syncRate") return automatedSyncRate;
    if (name == "phase") return automatedPhase;
    if (name == "q") return automatedQ;
    if (name == "minFreq") return automatedMinFreq;
    if (name == "maxFreq") return automatedMaxFreq;
    return false;
}

void WahWahEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // Stereo-Processing
        float left = buffer[i];
        float right = buffer[i + 1];
        
        // LFO berechnen
        float lfoValue = processLFO();
        
        // Frequenz berechnen
        float frequency = minFreq + (maxFreq - minFreq) * lfoValue * depth;
        updateFilterCoefficients(frequency);
        
        // Filter anwenden
        float filteredLeft = a0 * left + a1 * x1 + a2 * x2 - b1 * y1 - b2 * y2;
        float filteredRight = a0 * right + a1 * x1 + a2 * x2 - b1 * y1 - b2 * y2;
        
        // Zustand aktualisieren
        x2 = x1;
        x1 = (left + right) * 0.5f;
        y2 = y1;
        y1 = filteredLeft;
        
        // Mix
        buffer[i] = left * (1.0f - mix) + filteredLeft * mix;
        buffer[i + 1] = right * (1.0f - mix) + filteredRight * mix;
    }
}

float WahWahEffect::processLFO() {
    // LFO-Phase aktualisieren
    float currentRate = syncRate > 0.0f ? calculateSyncTime() : rate;
    lfoPhase += currentRate / 44100.0f;
    if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
    
    // LFO-Wert berechnen
    return 0.5f + 0.5f * std::sin(2.0f * M_PI * (lfoPhase + phase));
}

float WahWahEffect::calculateSyncTime() {
    // Sync-Zeit basierend auf BPM berechnen
    const float bpm = 120.0f; // Feste BPM für jetzt
    float syncTime = 60.0f / bpm;
    
    // Sync-Rate auf verschiedene Notenlängen mappen
    if (syncRate < 0.25f) return syncTime * 4.0f;      // Ganze Note
    else if (syncRate < 0.5f) return syncTime * 2.0f;  // Halbe Note
    else if (syncRate < 0.75f) return syncTime;        // Viertel Note
    else return syncTime * 0.5f;                       // Achtel Note
}

void WahWahEffect::updateFilterCoefficients(float frequency) {
    // Biquad-Filter-Koeffizienten berechnen
    float w0 = 2.0f * M_PI * frequency / 44100.0f;
    float alpha = std::sin(w0) / (2.0f * q);
    
    float cosw0 = std::cos(w0);
    float a = std::sqrt(2.0f);
    
    a0 = alpha;
    a1 = 0.0f;
    a2 = -alpha;
    b1 = -2.0f * cosw0;
    b2 = 1.0f - alpha;
    
    // Normalisierung
    float scale = 1.0f / (1.0f + alpha);
    a0 *= scale;
    a1 *= scale;
    a2 *= scale;
    b1 *= scale;
    b2 *= scale;
}

void WahWahEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        rate = 2.0f;
        depth = 0.5f;
        mix = 0.5f;
        syncRate = 0.0f;
        phase = 0.0f;
        q = 2.0f;
        minFreq = 500.0f;
        maxFreq = 2000.0f;
    }
    else if (presetName == "Heavy") {
        rate = 1.5f;
        depth = 0.7f;
        mix = 0.7f;
        syncRate = 0.0f;
        phase = 0.0f;
        q = 3.0f;
        minFreq = 300.0f;
        maxFreq = 3000.0f;
    }
    else if (presetName == "Subtle") {
        rate = 3.0f;
        depth = 0.3f;
        mix = 0.3f;
        syncRate = 0.0f;
        phase = 0.0f;
        q = 1.5f;
        minFreq = 800.0f;
        maxFreq = 1800.0f;
    }
    
    updateFilterCoefficients(minFreq);
}

void WahWahEffect::savePreset(const std::string& presetName) {
    // Hier würde die Preset-Speicherung implementiert werden
}

std::vector<std::string> WahWahEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 