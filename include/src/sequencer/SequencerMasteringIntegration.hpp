#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "MasteringEngine.hpp"
#include "SequencerKIIntegration.hpp"

namespace VR_DAW {

class SequencerMasteringIntegration {
public:
    SequencerMasteringIntegration();
    ~SequencerMasteringIntegration();

    // Initialisierung und Shutdown
    void initialize();
    void update();
    void shutdown();

    // Mastering-Verarbeitung
    void processMastering(const std::string& masteringName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processPatternMastering(const std::string& patternName, const std::string& masteringName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processAutomationMastering(const std::string& patternName, const std::string& masteringName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);

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
    std::unique_ptr<MasteringEngine> masteringEngine;
    std::unique_ptr<SequencerKIIntegration> kiIntegration;

    // Zustand
    struct State {
        bool isPlaying = false;
        bool isPaused = false;
        float currentTempo = 120.0f;
        float currentSwing = 0.0f;
        std::vector<std::string> activePatterns;
        std::map<std::string, bool> patternAutomation;
        std::map<std::string, std::vector<std::string>> patternMasterings;
    } currentState;

    // Parameter
    struct Parameters {
        float volume = 1.0f;
        float compression = 0.0f;
        float limiting = 0.0f;
        float eq = 0.0f;
        float stereo = 0.0f;
        float dither = 0.0f;
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
    void processMasteringToSequencer();
    void processSequencerToMastering();
    void updateParameters();
    void updateAnalysis();
    void generateVisualization();
    void applyAutomationCurves();
    void optimizeSequence();
    void validateState();
    void handleErrors();

    // Buffer-Validierung und Berechnungen
    bool validateMasteringBuffer(const std::vector<float>& buffer);
    float calculateMasteringLevel(const std::vector<float>& buffer);
    float calculateMasteringSpectrum(const std::vector<float>& buffer);
    float calculateMasteringPhase(const std::vector<float>& buffer);
};

} // namespace VR_DAW 