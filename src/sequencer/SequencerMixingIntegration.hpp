#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "MixingEngine.hpp"
#include "SequencerKIIntegration.hpp"

namespace VR_DAW {

class SequencerMixingIntegration {
public:
    SequencerMixingIntegration();
    ~SequencerMixingIntegration();

    // Initialisierung und Shutdown
    void initialize();
    void update();
    void shutdown();

    // Mixing-Verarbeitung
    void processMixing(const std::string& mixingName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processPatternMixing(const std::string& patternName, const std::string& mixingName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processAutomationMixing(const std::string& patternName, const std::string& mixingName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);

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
    std::unique_ptr<MixingEngine> mixingEngine;
    std::unique_ptr<SequencerKIIntegration> kiIntegration;

    // Zustand
    struct State {
        bool isPlaying = false;
        bool isPaused = false;
        float currentTempo = 120.0f;
        float currentSwing = 0.0f;
        std::vector<std::string> activePatterns;
        std::map<std::string, bool> patternAutomation;
        std::map<std::string, std::vector<std::string>> patternMixings;
    } currentState;

    // Parameter
    struct Parameters {
        float volume = 1.0f;
        float pan = 0.0f;
        float eq = 0.0f;
        float compression = 0.0f;
        float reverb = 0.0f;
        float delay = 0.0f;
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
    void processMixingToSequencer();
    void processSequencerToMixing();
    void updateParameters();
    void updateAnalysis();
    void generateVisualization();
    void applyAutomationCurves();
    void optimizeSequence();
    void validateState();
    void handleErrors();

    // Buffer-Validierung und Berechnungen
    bool validateMixingBuffer(const std::vector<float>& buffer);
    float calculateMixingLevel(const std::vector<float>& buffer);
    float calculateMixingSpectrum(const std::vector<float>& buffer);
    float calculateMixingPhase(const std::vector<float>& buffer);
};

} // namespace VR_DAW 