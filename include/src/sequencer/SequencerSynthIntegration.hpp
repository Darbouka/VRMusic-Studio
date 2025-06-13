#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "SynthEngine.hpp"
#include "SequencerKIIntegration.hpp"

namespace VR_DAW {

class SequencerSynthIntegration {
public:
    SequencerSynthIntegration();
    ~SequencerSynthIntegration();

    // Initialisierung und Shutdown
    void initialize();
    void update();
    void shutdown();

    // Synth-Verarbeitung
    void processSynth(const std::string& synthName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processPatternSynth(const std::string& patternName, const std::string& synthName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processAutomationSynth(const std::string& patternName, const std::string& synthName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);

    // Pattern-Management
    void createPattern(const std::string& name);
    void editPattern(const std::string& name);
    void deletePattern(const std::string& name);
    void savePattern(const std::string& name);
    void loadPattern(const std::string& name);

    // Sequenz-Steuerung
    void playSequence();
    void pauseSequence();
    void stopSequence();
    void setTempo(float tempo);
    void setSwing(float swing);

    // KI-Integration
    void connectKI();
    void updateKIFeedback();
    void applyKISuggestions();

    // Visualisierung
    void updatePatternVisualization();
    void updateSequenceVisualization();
    void updateAutomationVisualization();
    void updateAnalysisVisualization();

private:
    // Komponenten
    std::unique_ptr<SynthEngine> synthEngine;
    std::unique_ptr<SequencerKIIntegration> kiIntegration;

    // Zustand
    struct State {
        bool isPlaying = false;
        bool isPaused = false;
        float currentTempo = 120.0f;
        float currentSwing = 0.0f;
        std::vector<std::string> activePatterns;
        std::map<std::string, bool> patternAutomation;
        std::map<std::string, std::vector<std::string>> patternSynths;
    } currentState;

    // Parameter
    struct Parameters {
        float volume = 1.0f;
        float pitch = 0.0f;
        float filter = 0.0f;
        float resonance = 0.0f;
        float envelope = 0.0f;
        float lfo = 0.0f;
    } parameters;

    // Analyse
    struct Analysis {
        float level = 0.0f;
        float spectrum = 0.0f;
        float phase = 0.0f;
        float correlation = 0.0f;
        float coherence = 0.0f;
    } analysis;

    // Hilfsfunktionen
    void initializeComponents();
    void updateState();
    void processSynthToSequencer();
    void processSequencerToSynth();
    void updateParameters();
    void updateAnalysis();
    void generateVisualization();
    void applyAutomationCurves();
    void optimizeSequence();
    void validateState();
    void handleErrors();

    // Buffer-Validierung und Berechnungen
    bool validateSynthBuffer(const std::vector<float>& buffer);
    float calculateSynthLevel(const std::vector<float>& buffer);
    float calculateSynthSpectrum(const std::vector<float>& buffer);
    float calculateSynthPhase(const std::vector<float>& buffer);
};

} // namespace VR_DAW 