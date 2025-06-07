#include "VRKIIntegration.hpp"
#include <algorithm>
#include <cmath>

namespace VR_DAW {

VRKIIntegration::VRKIIntegration() {
    initializeComponents();
}

VRKIIntegration::~VRKIIntegration() {
    shutdown();
}

void VRKIIntegration::initialize() {
    initializeComponents();
    vrInterface->initialize();
    mixingEngine->initialize();
    masteringEngine->initialize();
}

void VRKIIntegration::update() {
    updateState();
    processVRToKI();
    processKIToVR();
    updateParameters();
    updateAnalysis();
    generateVisualization();
    applyAutomationCurves();
    optimizeParameters();
    validateState();
}

void VRKIIntegration::shutdown() {
    vrInterface->shutdown();
}

void VRKIIntegration::processVRInput(const glm::vec3& position, const glm::quat& rotation, bool trigger) {
    vrInterface->handleControllerInput(position, rotation, trigger);
    updateKIFeedback();
}

void VRKIIntegration::updateKIFeedback() {
    analyzeCurrentState();
    generateSuggestions();
    applyAutomation();
    updateVisualization();
}

void VRKIIntegration::applyKISuggestions() {
    for (const auto& param : parameters) {
        if (param.automated) {
            setParameter(param.name, param.value);
        }
    }
}

void VRKIIntegration::overrideKISettings() {
    // Implementiere KI-Überschreibungen
}

void VRKIIntegration::loadModel(const std::string& modelPath) {
    try {
        mixingEngine->loadModel(modelPath + "_mixing.pt");
        masteringEngine->loadModel(modelPath + "_mastering.pt");
    }
    catch (const std::exception& e) {
        handleErrors();
    }
}

void VRKIIntegration::saveModel(const std::string& modelPath) {
    try {
        mixingEngine->saveModel(modelPath + "_mixing.pt");
        masteringEngine->saveModel(modelPath + "_mastering.pt");
    }
    catch (const std::exception& e) {
        handleErrors();
    }
}

void VRKIIntegration::updateModelParameters() {
    for (const auto& param : parameters) {
        mixingEngine->setParameter(param.name, param.value);
        masteringEngine->setParameter(param.name, param.value);
    }
}

void VRKIIntegration::resetModelState() {
    currentState = State();
    parameters.clear();
    currentAnalysis = Analysis();
}

void VRKIIntegration::analyzeCurrentState() {
    currentState.isAnalyzing = true;
    
    // Führe Analyse durch
    mixingEngine->analyzeTrack();
    masteringEngine->analyzeTrack();
    
    // Aktualisiere Analyse-Ergebnisse
    currentAnalysis.frequencyResponse = masteringEngine->getFrequencyResponse();
    currentAnalysis.dynamicRange = masteringEngine->getCurrentDynamicRange();
    currentAnalysis.loudness = masteringEngine->getCurrentLoudness();
    
    currentState.isAnalyzing = false;
}

void VRKIIntegration::generateSuggestions() {
    currentState.isGenerating = true;
    
    // Generiere KI-Vorschläge
    for (auto& param : parameters) {
        if (param.automated) {
            // Berechne optimale Parameter-Werte
            float suggestedValue = calculateOptimalValue(param);
            param.value = suggestedValue;
        }
    }
    
    currentState.isGenerating = false;
}

void VRKIIntegration::applyAutomation() {
    currentState.isAutomating = true;
    
    // Wende Automatisierung an
    for (const auto& param : parameters) {
        if (param.automated) {
            applyAutomationCurve(param);
        }
    }
    
    currentState.isAutomating = false;
}

void VRKIIntegration::updateVisualization() {
    // Aktualisiere VR-Visualisierung
    vrInterface->update();
}

void VRKIIntegration::setParameter(const std::string& name, float value) {
    auto it = std::find_if(parameters.begin(), parameters.end(),
                          [&name](const Parameter& p) { return p.name == name; });
    
    if (it != parameters.end()) {
        it->value = std::max(it->minValue, std::min(it->maxValue, value));
        updateModelParameters();
    }
}

float VRKIIntegration::getParameter(const std::string& name) const {
    auto it = std::find_if(parameters.begin(), parameters.end(),
                          [&name](const Parameter& p) { return p.name == name; });
    
    return (it != parameters.end()) ? it->value : 0.0f;
}

void VRKIIntegration::automateParameter(const std::string& name, bool automated) {
    auto it = std::find_if(parameters.begin(), parameters.end(),
                          [&name](const Parameter& p) { return p.name == name; });
    
    if (it != parameters.end()) {
        it->automated = automated;
    }
}

bool VRKIIntegration::isParameterAutomated(const std::string& name) const {
    auto it = std::find_if(parameters.begin(), parameters.end(),
                          [&name](const Parameter& p) { return p.name == name; });
    
    return (it != parameters.end()) ? it->automated : false;
}

void VRKIIntegration::initializeComponents() {
    vrInterface = std::make_unique<VRMixingInterface>();
    mixingEngine = std::make_unique<AIMixingEngine>();
    masteringEngine = std::make_unique<AIMasteringEngine>();
    
    // Initialisiere Parameter
    parameters = {
        {"volume", 0.0f, -60.0f, 6.0f, false, {}},
        {"pan", 0.0f, -1.0f, 1.0f, false, {}},
        {"eq_low", 0.0f, -12.0f, 12.0f, false, {}},
        {"eq_mid", 0.0f, -12.0f, 12.0f, false, {}},
        {"eq_high", 0.0f, -12.0f, 12.0f, false, {}},
        {"comp_threshold", -20.0f, -60.0f, 0.0f, false, {}},
        {"comp_ratio", 1.0f, 1.0f, 20.0f, false, {}},
        {"reverb_amount", 0.0f, 0.0f, 1.0f, false, {}},
        {"delay_amount", 0.0f, 0.0f, 1.0f, false, {}}
    };
}

void VRKIIntegration::updateState() {
    // Aktualisiere Zustandsvariablen
    currentState.currentParameters.clear();
    for (const auto& param : parameters) {
        currentState.currentParameters.push_back(param.value);
    }
}

void VRKIIntegration::processVRToKI() {
    // Verarbeite VR-Eingaben für KI
    for (const auto& param : parameters) {
        if (!param.automated) {
            mixingEngine->setParameter(param.name, param.value);
            masteringEngine->setParameter(param.name, param.value);
        }
    }
}

void VRKIIntegration::processKIToVR() {
    // Verarbeite KI-Ausgaben für VR
    for (auto& param : parameters) {
        if (param.automated) {
            param.value = mixingEngine->getParameter(param.name);
        }
    }
}

void VRKIIntegration::updateParameters() {
    // Aktualisiere Parameter basierend auf KI-Analyse
    for (auto& param : parameters) {
        if (param.automated) {
            float suggestedValue = calculateOptimalValue(param);
            param.value = suggestedValue;
        }
    }
}

void VRKIIntegration::updateAnalysis() {
    // Aktualisiere Analyse-Ergebnisse
    currentAnalysis.frequencyResponse = masteringEngine->getFrequencyResponse();
    currentAnalysis.dynamicRange = masteringEngine->getCurrentDynamicRange();
    currentAnalysis.loudness = masteringEngine->getCurrentLoudness();
}

void VRKIIntegration::generateVisualization() {
    // Generiere Visualisierungsdaten
    vrInterface->update();
}

void VRKIIntegration::applyAutomationCurves() {
    // Wende Automatisierungskurven an
    for (auto& param : parameters) {
        if (param.automated && !param.automationData.empty()) {
            applyAutomationCurve(param);
        }
    }
}

void VRKIIntegration::optimizeParameters() {
    // Optimiere Parameter basierend auf KI-Analyse
    for (auto& param : parameters) {
        if (param.automated) {
            float optimalValue = calculateOptimalValue(param);
            param.value = optimalValue;
        }
    }
}

void VRKIIntegration::validateState() {
    // Validiere aktuellen Zustand
    for (const auto& param : parameters) {
        if (param.value < param.minValue || param.value > param.maxValue) {
            handleErrors();
        }
    }
}

void VRKIIntegration::handleErrors() {
    // Implementiere Fehlerbehandlung
}

float VRKIIntegration::calculateOptimalValue(const Parameter& param) {
    // Berechne optimalen Parameter-Wert
    return param.value; // Placeholder
}

void VRKIIntegration::applyAutomationCurve(const Parameter& param) {
    // Wende Automatisierungskurve an
}

} // namespace VR_DAW 