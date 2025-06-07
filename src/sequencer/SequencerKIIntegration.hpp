#pragma once

#include "KIEngine.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace VR_DAW {

class SequencerKIIntegration {
public:
    SequencerKIIntegration();
    ~SequencerKIIntegration();

    // Initialisierung und Shutdown
    void initialize();
    void update();
    void shutdown();

    // KI-Verarbeitung
    void processKI(const std::string& kiName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processPatternKI(const std::string& patternName, const std::string& kiName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processAutomationKI(const std::string& patternName, const std::string& kiName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);

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
    void connect();
    void updateFeedback();
    void applySuggestions();

    // Visualisierung
    void updatePatternVisualization();
    void updateSequenceVisualization();
    void updateAutomationVisualization();
    void updateAnalysisVisualization();

private:
    // Komponenten
    std::unique_ptr<KIEngine> kiEngine;

    // Zustandsvariablen
    struct State {
        bool isPlaying;
        bool isPaused;
        float currentTempo;
        float currentSwing;
        std::vector<std::string> activePatterns;
        std::map<std::string, bool> patternAutomation;
        std::map<std::string, std::vector<std::string>> patternKI;
    } currentState;

    // Parameter
    struct Parameter {
        float value;
        float min;
        float max;
        bool automated;
        std::vector<float> automationCurve;
    };
    std::map<std::string, Parameter> parameters;

    // Analyse
    struct Analysis {
        std::vector<float> rhythm;
        std::vector<float> melody;
        std::vector<float> harmony;
        float complexity;
        float coherence;
        float originality;
    } currentAnalysis;

    // Hilfsfunktionen
    void initializeComponents();
    void updateState();
    void processKIToSequencer();
    void processSequencerToKI();
    void updateParameters();
    void updateAnalysis();
    void generateVisualization();
    void applyAutomationCurves();
    void optimizeSequence();
    void validateState();
    void handleErrors();

    // KI-Hilfsfunktionen
    bool validateKIBuffer(const std::vector<float>& buffer);
    float calculateKILevel(const std::vector<float>& buffer);
    float calculateKISpectrum(const std::vector<float>& buffer);
    float calculateKIPhase(const std::vector<float>& buffer);
};

} // namespace VR_DAW 