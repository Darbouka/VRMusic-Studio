#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "PluginEngine.hpp"
#include "SequencerKIIntegration.hpp"

namespace VR_DAW {

class SequencerPluginIntegration {
public:
    SequencerPluginIntegration();
    ~SequencerPluginIntegration();

    // Lifecycle Management
    void initialize();
    void update();
    void shutdown();

    // Plugin Processing
    void processPlugin(const std::string& pluginName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processPatternPlugin(const std::string& patternName, const std::string& pluginName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processAutomationPlugin(const std::string& patternName, const std::string& pluginName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);

    // Pattern Management
    void createPattern(const std::string& name);
    void editPattern(const std::string& name);
    void deletePattern(const std::string& name);
    void savePattern(const std::string& name);
    void loadPattern(const std::string& name);

    // Sequence Control
    void playSequence();
    void pauseSequence();
    void stopSequence();
    void setTempo(float tempo);
    void setSwing(float swing);

    // KI Integration
    void connectKI();
    void updateKIFeedback();
    void applyKISuggestions();

    // Visualization Updates
    void updatePatternVisualization();
    void updateSequenceVisualization();
    void updateAutomationVisualization();
    void updateAnalysisVisualization();

private:
    // Components
    std::unique_ptr<PluginEngine> pluginEngine;
    std::unique_ptr<SequencerKIIntegration> kiIntegration;

    // State
    struct {
        bool isPlaying = false;
        bool isPaused = false;
        float currentTempo = 120.0f;
        float currentSwing = 0.0f;
        std::vector<std::string> activePatterns;
        std::map<std::string, bool> patternAutomation;
        std::map<std::string, std::vector<std::string>> patternPlugins;
    } currentState;

    // Parameters
    struct {
        float volume = 1.0f;
        float pan = 0.0f;
        float filter = 1.0f;
        float resonance = 0.0f;
        float envelope = 1.0f;
        float lfo = 0.0f;
        float modulation = 0.0f;
        float effects = 0.0f;
    } parameters;

    // Analysis
    struct {
        float level = 0.0f;
        float spectrum = 0.0f;
        float phase = 0.0f;
        float correlation = 0.0f;
        float dynamics = 0.0f;
        float stereo = 0.0f;
        float frequency = 0.0f;
        float transient = 0.0f;
    } analysis;

    // Helper Functions
    void initializeComponents();
    void updateState();
    void processPluginToSequencer();
    void processSequencerToPlugin();
    void updateParameters();
    void updateAnalysis();
    void generateVisualization();
    void applyAutomationCurves();
    void optimizeSequence();
    void validateState();
    void handleErrors();

    // Buffer Validation and Calculations
    bool validatePluginBuffer(const std::vector<float>& buffer);
    float calculatePluginLevel(const std::vector<float>& buffer);
    float calculatePluginSpectrum(const std::vector<float>& buffer);
    float calculatePluginPhase(const std::vector<float>& buffer);
};

} // namespace VR_DAW 