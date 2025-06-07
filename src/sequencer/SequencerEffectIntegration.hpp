#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "EffectEngine.hpp"
#include "SequencerKIIntegration.hpp"

namespace VR_DAW {

class SequencerEffectIntegration {
public:
    SequencerEffectIntegration();
    ~SequencerEffectIntegration();

    // Initialisierung und Shutdown
    void initialize();
    void update();
    void shutdown();

    // Effect-Verarbeitung
    void processEffect(const std::string& effectName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processPatternEffect(const std::string& patternName, const std::string& effectName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processAutomationEffect(const std::string& patternName, const std::string& effectName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);

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
    std::unique_ptr<EffectEngine> effectEngine;
    std::unique_ptr<SequencerKIIntegration> kiIntegration;

    // Zustand
    struct State {
        bool isPlaying = false;
        bool isPaused = false;
        float currentTempo = 120.0f;
        float currentSwing = 0.0f;
        std::vector<std::string> activePatterns;
        std::map<std::string, bool> patternAutomation;
        std::map<std::string, std::vector<std::string>> patternEffects;
    } currentState;

    // Parameter
    struct Parameters {
        float volume = 1.0f;
        float reverb = 0.0f;
        float delay = 0.0f;
        float chorus = 0.0f;
        float flanger = 0.0f;
        float phaser = 0.0f;
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
    void processEffectToSequencer();
    void processSequencerToEffect();
    void updateParameters();
    void updateAnalysis();
    void generateVisualization();
    void applyAutomationCurves();
    void optimizeSequence();
    void validateState();
    void handleErrors();

    // Buffer-Validierung und Berechnungen
    bool validateEffectBuffer(const std::vector<float>& buffer);
    float calculateEffectLevel(const std::vector<float>& buffer);
    float calculateEffectSpectrum(const std::vector<float>& buffer);
    float calculateEffectPhase(const std::vector<float>& buffer);
};

} // namespace VR_DAW 