#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "VREngine.hpp"
#include "SequencerKIIntegration.hpp"

namespace VR_DAW {

class SequencerVRIntegration {
public:
    SequencerVRIntegration();
    ~SequencerVRIntegration();

    // Initialisierung und Shutdown
    void initialize();
    void update();
    void shutdown();

    // VR-Verarbeitung
    void processVR(const std::string& vrName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processPatternVR(const std::string& patternName, const std::string& vrName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processAutomationVR(const std::string& patternName, const std::string& vrName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);

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
    std::unique_ptr<VREngine> vrEngine;
    std::unique_ptr<SequencerKIIntegration> kiIntegration;

    // Zustand
    struct State {
        bool isPlaying = false;
        bool isPaused = false;
        float currentTempo = 120.0f;
        float currentSwing = 0.0f;
        std::vector<std::string> activePatterns;
        std::map<std::string, bool> patternAutomation;
        std::map<std::string, std::vector<std::string>> patternVRs;
    } currentState;

    // Parameter
    struct Parameters {
        float volume = 1.0f;
        float position = 0.0f;
        float rotation = 0.0f;
        float scale = 1.0f;
        float opacity = 1.0f;
        float blend = 0.0f;
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
    void processVRToSequencer();
    void processSequencerToVR();
    void updateParameters();
    void updateAnalysis();
    void generateVisualization();
    void applyAutomationCurves();
    void optimizeSequence();
    void validateState();
    void handleErrors();

    // Buffer-Validierung und Berechnungen
    bool validateVRBuffer(const std::vector<float>& buffer);
    float calculateVRLevel(const std::vector<float>& buffer);
    float calculateVRSpectrum(const std::vector<float>& buffer);
    float calculateVRPhase(const std::vector<float>& buffer);
};

} // namespace VR_DAW 