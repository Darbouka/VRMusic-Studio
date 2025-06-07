#include "QuantumCompressorEffect.hpp"
#include <cmath>
#include <algorithm>
#include <fftw3.h>

namespace VR_DAW {

QuantumCompressorEffect::QuantumCompressorEffect()
    : threshold(0.5f)
    , ratio(0.3f)
    , attack(0.4f)
    , release(0.5f)
    , entanglement(0.6f)
    , mix(0.5f)
    , quality(1.0f)
    , multiband(0.5f)
    , adaptive(0.5f)
    , spectral(0.5f)
    , transient(0.5f)
    , sidechain(0.5f)
    , parallel(0.5f)
    , midSide(0.5f)
    , lookahead(0.5f)
    , automatedThreshold(false)
    , automatedRatio(false)
    , automatedAttack(false)
    , automatedRelease(false)
    , automatedEntanglement(false)
    , automatedMix(false)
    , automatedQuality(false)
    , automatedMultiband(false)
    , automatedAdaptive(false)
    , automatedSpectral(false)
    , automatedTransient(false)
    , automatedSidechain(false)
    , automatedParallel(false)
    , automatedMidSide(false)
    , automatedLookahead(false)
    , bufferSize(44100 * 2) // 2 Sekunden bei 44.1kHz
{
    std::random_device rd;
    rng.seed(rd());
    initializeStates();
}

QuantumCompressorEffect::~QuantumCompressorEffect() {
    shutdown();
}

bool QuantumCompressorEffect::initialize() {
    initializeStates();
    return true;
}

void QuantumCompressorEffect::shutdown() {
    states.clear();
}

void QuantumCompressorEffect::initializeStates() {
    states.clear();
    
    // Erstelle 4 Compressor-Zustände für komplexe Klangtransformation
    for (int i = 0; i < 4; ++i) {
        CompressorState state;
        
        // Erstelle 8 Quantenzustände pro Compressor
        for (int j = 0; j < 8; ++j) {
            QuantumState qstate;
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            
            qstate.amplitude = 1.0f;
            qstate.phase = dist(rng);
            qstate.entanglement = entanglement;
            qstate.superposition = 0.0f;
            qstate.wavefunction.resize(64, std::complex<float>(0.0f, 0.0f));
            qstate.probabilities.resize(64, 0.0f);
            
            state.states.push_back(qstate);
        }
        
        // Initialisiere Bands
        state.bands.resize(4, 0.0f);
        state.spectral.resize(1024, 0.0f);
        state.transient.resize(1024, 0.0f);
        state.sidechain.resize(1024, 0.0f);
        state.parallel.resize(1024, 0.0f);
        state.midSide.resize(1024, 0.0f);
        state.lookahead.resize(1024, 0.0f);
        
        state.gain = 1.0f;
        state.envelope = 0.0f;
        
        states.push_back(state);
    }
}

void QuantumCompressorEffect::updateStates() {
    // Aktualisiere Zustände basierend auf Parametern
    for (auto& state : states) {
        for (auto& qstate : state.states) {
            qstate.entanglement = entanglement;
            
            // Aktualisiere Phase
            qstate.phase += 0.0001f;
            if (qstate.phase > 1.0f) qstate.phase -= 1.0f;
        }
        
        // Aktualisiere Bands
        updateBands(state);
        
        // Aktualisiere Spektralanalyse
        updateSpectral(state);
        
        // Aktualisiere Transientenerkennung
        updateTransient(state);
        
        // Aktualisiere Sidechain
        updateSidechain(state);
        
        // Aktualisiere Parallel-Processing
        updateParallel(state);
        
        // Aktualisiere Mid/Side-Processing
        updateMidSide(state);
        
        // Aktualisiere Lookahead
        updateLookahead(state);
    }
}

void QuantumCompressorEffect::processState(CompressorState& state, float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float input = buffer[i];
        float output = input;
        
        // Verarbeite jeden Quantenzustand
        for (auto& qstate : state.states) {
            // Berechne Wellenfunktion
            calculateWavefunction(qstate, input);
            
            // Kollabiere Wellenfunktion
            collapseWavefunction(qstate);
            
            // Berechne Amplitude
            float amplitude = std::abs(input);
            
            // Wende Threshold an
            if (amplitude > threshold) {
                // Berechne Kompression
                float compression = (amplitude - threshold) * ratio;
                amplitude = threshold + compression;
            }
            
            // Wende Quanteneffekte an
            float quantum = qstate.entanglement * std::sin(2.0f * M_PI * qstate.phase);
            amplitude *= (1.0f + quantum);
            
            // Aktualisiere Zustand
            qstate.amplitude = amplitude;
            qstate.superposition = input * amplitude;
            
            // Modifiziere Output
            output += qstate.superposition;
        }
        
        // Normalisiere Output
        output /= (state.states.size() + 1);
        
        // Wende Multiband an
        if (multiband > 0.0f) {
            output = applyMultiband(&output, 1);
        }
        
        // Wende Adaptive an
        if (adaptive > 0.0f) {
            output = applyAdaptive(&output, 1);
        }
        
        // Wende Spectral an
        if (spectral > 0.0f) {
            output = applySpectral(&output, 1);
        }
        
        // Wende Transient an
        if (transient > 0.0f) {
            output = applyTransient(&output, 1);
        }
        
        // Wende Sidechain an
        if (sidechain > 0.0f) {
            output = applySidechain(&output, 1);
        }
        
        // Wende Parallel an
        if (parallel > 0.0f) {
            output = applyParallel(&output, 1);
        }
        
        // Wende Mid/Side an
        if (midSide > 0.0f) {
            output = applyMidSide(&output, 1);
        }
        
        // Wende Lookahead an
        if (lookahead > 0.0f) {
            output = applyLookahead(&output, 1);
        }
        
        // Aktualisiere Envelope
        float attackTime = attack * 0.1f;
        float releaseTime = release * 0.1f;
        
        if (std::abs(output) > state.envelope) {
            state.envelope += (std::abs(output) - state.envelope) * attackTime;
        } else {
            state.envelope += (std::abs(output) - state.envelope) * releaseTime;
        }
        
        // Wende Gain an
        state.gain = 1.0f / (state.envelope + 0.0001f);
        output *= state.gain;
        
        buffer[i] = output;
    }
}

void QuantumCompressorEffect::calculateWavefunction(QuantumState& state, float input) {
    // Berechne Wellenfunktion für Eingangssignal
    for (size_t i = 0; i < state.wavefunction.size(); ++i) {
        float phase = 2.0f * M_PI * i / state.wavefunction.size();
        state.wavefunction[i] = std::complex<float>(
            input * std::cos(phase),
            input * std::sin(phase)
        );
    }
}

void QuantumCompressorEffect::collapseWavefunction(QuantumState& state) {
    // Kollabiere Wellenfunktion zu klassischem Zustand
    for (size_t i = 0; i < state.wavefunction.size(); ++i) {
        state.probabilities[i] = std::norm(state.wavefunction[i]);
    }
}

void QuantumCompressorEffect::updateBands(CompressorState& state) {
    // Aktualisiere Frequenzbänder
    for (size_t i = 0; i < state.bands.size(); ++i) {
        float freq = 20.0f * std::pow(10.0f, i * 3.0f / (state.bands.size() - 1));
        state.bands[i] = freq;
    }
}

void QuantumCompressorEffect::updateSpectral(CompressorState& state) {
    // FFT für Spektralanalyse
    fftwf_complex* in = fftwf_alloc_complex(state.spectral.size());
    fftwf_complex* out = fftwf_alloc_complex(state.spectral.size());
    fftwf_plan p = fftwf_plan_dft_1d(state.spectral.size(), in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    
    // Kopiere Daten
    for (size_t i = 0; i < state.spectral.size(); ++i) {
        in[i][0] = state.spectral[i];
        in[i][1] = 0.0f;
    }
    
    // Führe FFT durch
    fftwf_execute(p);
    
    // Kopiere Ergebnisse
    for (size_t i = 0; i < state.spectral.size(); ++i) {
        state.spectral[i] = std::sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
    }
    
    // Aufräumen
    fftwf_destroy_plan(p);
    fftwf_free(in);
    fftwf_free(out);
}

void QuantumCompressorEffect::updateTransient(CompressorState& state) {
    // Transientenerkennung
    for (size_t i = 1; i < state.transient.size(); ++i) {
        float diff = state.transient[i] - state.transient[i-1];
        state.transient[i] = std::abs(diff);
    }
}

void QuantumCompressorEffect::updateSidechain(CompressorState& state) {
    // Sidechain-Verarbeitung
    for (size_t i = 0; i < state.sidechain.size(); ++i) {
        state.sidechain[i] = state.sidechain[i] * sidechain;
    }
}

void QuantumCompressorEffect::updateParallel(CompressorState& state) {
    // Parallel-Processing
    for (size_t i = 0; i < state.parallel.size(); ++i) {
        state.parallel[i] = state.parallel[i] * parallel;
    }
}

void QuantumCompressorEffect::updateMidSide(CompressorState& state) {
    // Mid/Side-Processing
    for (size_t i = 0; i < state.midSide.size(); i += 2) {
        float mid = (state.midSide[i] + state.midSide[i+1]) * 0.5f;
        float side = (state.midSide[i] - state.midSide[i+1]) * 0.5f;
        
        state.midSide[i] = mid;
        state.midSide[i+1] = side;
    }
}

void QuantumCompressorEffect::updateLookahead(CompressorState& state) {
    // Lookahead-Verarbeitung
    for (size_t i = 0; i < state.lookahead.size(); ++i) {
        state.lookahead[i] = state.lookahead[i] * lookahead;
    }
}

float QuantumCompressorEffect::applyMultiband(float* buffer, unsigned long framesPerBuffer) {
    float output = 0.0f;
    for (size_t i = 0; i < framesPerBuffer; ++i) {
        output += buffer[i] * multiband;
    }
    return output / framesPerBuffer;
}

float QuantumCompressorEffect::applyAdaptive(float* buffer, unsigned long framesPerBuffer) {
    float output = 0.0f;
    for (size_t i = 0; i < framesPerBuffer; ++i) {
        output += buffer[i] * adaptive;
    }
    return output / framesPerBuffer;
}

float QuantumCompressorEffect::applySpectral(float* buffer, unsigned long framesPerBuffer) {
    float output = 0.0f;
    for (size_t i = 0; i < framesPerBuffer; ++i) {
        output += buffer[i] * spectral;
    }
    return output / framesPerBuffer;
}

float QuantumCompressorEffect::applyTransient(float* buffer, unsigned long framesPerBuffer) {
    float output = 0.0f;
    for (size_t i = 0; i < framesPerBuffer; ++i) {
        output += buffer[i] * transient;
    }
    return output / framesPerBuffer;
}

float QuantumCompressorEffect::applySidechain(float* buffer, unsigned long framesPerBuffer) {
    float output = 0.0f;
    for (size_t i = 0; i < framesPerBuffer; ++i) {
        output += buffer[i] * sidechain;
    }
    return output / framesPerBuffer;
}

float QuantumCompressorEffect::applyParallel(float* buffer, unsigned long framesPerBuffer) {
    float output = 0.0f;
    for (size_t i = 0; i < framesPerBuffer; ++i) {
        output += buffer[i] * parallel;
    }
    return output / framesPerBuffer;
}

float QuantumCompressorEffect::applyMidSide(float* buffer, unsigned long framesPerBuffer) {
    float output = 0.0f;
    for (size_t i = 0; i < framesPerBuffer; ++i) {
        output += buffer[i] * midSide;
    }
    return output / framesPerBuffer;
}

float QuantumCompressorEffect::applyLookahead(float* buffer, unsigned long framesPerBuffer) {
    float output = 0.0f;
    for (size_t i = 0; i < framesPerBuffer; ++i) {
        output += buffer[i] * lookahead;
    }
    return output / framesPerBuffer;
}

void QuantumCompressorEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateStates();
    
    // Verarbeite jeden Zustand
    for (auto& state : states) {
        processState(state, buffer, framesPerBuffer);
    }
    
    // Wende Effekte an
    applyThreshold(buffer, framesPerBuffer);
    applyRatio(buffer, framesPerBuffer);
    applyEnvelope(buffer, framesPerBuffer);
    
    // Wende Mix an
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = buffer[i] * mix;
    }
}

std::vector<PluginParameter> QuantumCompressorEffect::getParameters() const {
    return {
        {"threshold", threshold, 0.0f, 1.0f, ""},
        {"ratio", ratio, 0.0f, 1.0f, ""},
        {"attack", attack, 0.0f, 1.0f, ""},
        {"release", release, 0.0f, 1.0f, ""},
        {"entanglement", entanglement, 0.0f, 1.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""},
        {"multiband", multiband, 0.0f, 1.0f, ""},
        {"adaptive", adaptive, 0.0f, 1.0f, ""},
        {"spectral", spectral, 0.0f, 1.0f, ""},
        {"transient", transient, 0.0f, 1.0f, ""},
        {"sidechain", sidechain, 0.0f, 1.0f, ""},
        {"parallel", parallel, 0.0f, 1.0f, ""},
        {"midSide", midSide, 0.0f, 1.0f, ""},
        {"lookahead", lookahead, 0.0f, 1.0f, ""}
    };
}

void QuantumCompressorEffect::setParameter(const std::string& name, float value) {
    if (name == "threshold") threshold = value;
    else if (name == "ratio") ratio = value;
    else if (name == "attack") attack = value;
    else if (name == "release") release = value;
    else if (name == "entanglement") entanglement = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
    else if (name == "multiband") multiband = value;
    else if (name == "adaptive") adaptive = value;
    else if (name == "spectral") spectral = value;
    else if (name == "transient") transient = value;
    else if (name == "sidechain") sidechain = value;
    else if (name == "parallel") parallel = value;
    else if (name == "midSide") midSide = value;
    else if (name == "lookahead") lookahead = value;
}

float QuantumCompressorEffect::getParameter(const std::string& name) const {
    if (name == "threshold") return threshold;
    if (name == "ratio") return ratio;
    if (name == "attack") return attack;
    if (name == "release") return release;
    if (name == "entanglement") return entanglement;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
    if (name == "multiband") return multiband;
    if (name == "adaptive") return adaptive;
    if (name == "spectral") return spectral;
    if (name == "transient") return transient;
    if (name == "sidechain") return sidechain;
    if (name == "parallel") return parallel;
    if (name == "midSide") return midSide;
    if (name == "lookahead") return lookahead;
    return 0.0f;
}

void QuantumCompressorEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "threshold") automatedThreshold = automated;
    else if (name == "ratio") automatedRatio = automated;
    else if (name == "attack") automatedAttack = automated;
    else if (name == "release") automatedRelease = automated;
    else if (name == "entanglement") automatedEntanglement = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
    else if (name == "multiband") automatedMultiband = automated;
    else if (name == "adaptive") automatedAdaptive = automated;
    else if (name == "spectral") automatedSpectral = automated;
    else if (name == "transient") automatedTransient = automated;
    else if (name == "sidechain") automatedSidechain = automated;
    else if (name == "parallel") automatedParallel = automated;
    else if (name == "midSide") automatedMidSide = automated;
    else if (name == "lookahead") automatedLookahead = automated;
}

bool QuantumCompressorEffect::isParameterAutomated(const std::string& name) const {
    if (name == "threshold") return automatedThreshold;
    if (name == "ratio") return automatedRatio;
    if (name == "attack") return automatedAttack;
    if (name == "release") return automatedRelease;
    if (name == "entanglement") return automatedEntanglement;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    if (name == "multiband") return automatedMultiband;
    if (name == "adaptive") return automatedAdaptive;
    if (name == "spectral") return automatedSpectral;
    if (name == "transient") return automatedTransient;
    if (name == "sidechain") return automatedSidechain;
    if (name == "parallel") return automatedParallel;
    if (name == "midSide") return automatedMidSide;
    if (name == "lookahead") return automatedLookahead;
    return false;
}

void QuantumCompressorEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        threshold = 0.5f;
        ratio = 0.3f;
        attack = 0.4f;
        release = 0.5f;
        entanglement = 0.6f;
        mix = 0.5f;
        quality = 1.0f;
        multiband = 0.5f;
        adaptive = 0.5f;
        spectral = 0.5f;
        transient = 0.5f;
        sidechain = 0.5f;
        parallel = 0.5f;
        midSide = 0.5f;
        lookahead = 0.5f;
    }
    else if (presetName == "Heavy") {
        threshold = 0.7f;
        ratio = 0.5f;
        attack = 0.6f;
        release = 0.7f;
        entanglement = 0.8f;
        mix = 0.7f;
        quality = 1.0f;
        multiband = 0.7f;
        adaptive = 0.7f;
        spectral = 0.7f;
        transient = 0.7f;
        sidechain = 0.7f;
        parallel = 0.7f;
        midSide = 0.7f;
        lookahead = 0.7f;
    }
    else if (presetName == "Subtle") {
        threshold = 0.3f;
        ratio = 0.2f;
        attack = 0.3f;
        release = 0.4f;
        entanglement = 0.5f;
        mix = 0.3f;
        quality = 1.0f;
        multiband = 0.3f;
        adaptive = 0.3f;
        spectral = 0.3f;
        transient = 0.3f;
        sidechain = 0.3f;
        parallel = 0.3f;
        midSide = 0.3f;
        lookahead = 0.3f;
    }
    
    updateStates();
}

void QuantumCompressorEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> QuantumCompressorEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 