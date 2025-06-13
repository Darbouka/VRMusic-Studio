#pragma once

#include "VRMixingInterface.hpp"
#include "AIMixingEngine.hpp"
#include "AIMasteringEngine.hpp"
#include <memory>
#include <vector>
#include <string>

namespace VR_DAW {

class VRKIIntegration {
public:
    VRKIIntegration();
    ~VRKIIntegration();

    // Hauptfunktionen
    void initialize();
    void update();
    void shutdown();

    // VR-KI-Interaktion
    void processVRInput(const glm::vec3& position, const glm::quat& rotation, bool trigger);
    void updateKIFeedback();
    void applyKISuggestions();
    void overrideKISettings();

    // KI-Modell-Management
    void loadModel(const std::string& modelPath);
    void saveModel(const std::string& modelPath);
    void updateModelParameters();
    void resetModelState();

    // Echtzeit-Analyse
    void analyzeCurrentState();
    void generateSuggestions();
    void applyAutomation();
    void updateVisualization();

    // Parameter-Management
    void setParameter(const std::string& name, float value);
    float getParameter(const std::string& name) const;
    void automateParameter(const std::string& name, bool automated);
    bool isParameterAutomated(const std::string& name) const;

private:
    // Komponenten
    std::unique_ptr<VRMixingInterface> vrInterface;
    std::unique_ptr<AIMixingEngine> mixingEngine;
    std::unique_ptr<AIMasteringEngine> masteringEngine;

    // Zustandsvariablen
    struct State {
        std::vector<float> currentParameters;
        std::vector<float> targetParameters;
        std::vector<float> automationCurves;
        bool isAnalyzing;
        bool isGenerating;
        bool isAutomating;
    };

    // Parameter
    struct Parameter {
        std::string name;
        float value;
        float minValue;
        float maxValue;
        bool automated;
        std::vector<float> automationData;
    };

    // Analyse
    struct Analysis {
        std::vector<float> frequencyResponse;
        std::vector<float> dynamicRange;
        std::vector<float> stereoImage;
        float loudness;
        float clarity;
        float balance;
    };

    // Zustand
    State currentState;
    std::vector<Parameter> parameters;
    Analysis currentAnalysis;

    // Hilfsmethoden
    void initializeComponents();
    void updateState();
    void processVRToKI();
    void processKIToVR();
    void updateParameters();
    void updateAnalysis();
    void generateVisualization();
    void applyAutomationCurves();
    void optimizeParameters();
    void validateState();
    void handleErrors();
};

} // namespace VR_DAW 