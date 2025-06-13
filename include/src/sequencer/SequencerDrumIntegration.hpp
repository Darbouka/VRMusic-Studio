#pragma once

#include "DrumEngine.hpp"
#include "SequencerKIIntegration.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace VR_DAW {

class SequencerDrumIntegration {
public:
    SequencerDrumIntegration();
    ~SequencerDrumIntegration();

    // Initialisierung und Shutdown
    void initialize();
    void update();
    void shutdown();

    // Drum-Verarbeitung
    void processDrum(const std::string& drumName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processPatternDrum(const std::string& patternName, const std::string& drumName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processAutomationDrum(const std::string& patternName, const std::string& drumName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);

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
    std::unique_ptr<DrumEngine> drumEngine;
    std::unique_ptr<SequencerKIIntegration> kiIntegration;

    // Zustandsvariablen
    struct State {
        bool isPlaying;
        bool isPaused;
        float currentTempo;
        float currentSwing;
        std::vector<std::string> activePatterns;
        std::map<std::string, bool> patternAutomation;
        std::map<std::string, std::vector<std::string>> patternDrums;
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
    void processDrumToSequencer();
    void processSequencerToDrum();
    void updateParameters();
    void updateAnalysis();
    void generateVisualization();
    void applyAutomationCurves();
    void optimizeSequence();
    void validateState();
    void handleErrors();

    // Drum-Hilfsfunktionen
    bool validateDrumBuffer(const std::vector<float>& buffer);
    float calculateDrumLevel(const std::vector<float>& buffer);
    float calculateDrumSpectrum(const std::vector<float>& buffer);
    float calculateDrumPhase(const std::vector<float>& buffer);
};

} // namespace VR_DAW 