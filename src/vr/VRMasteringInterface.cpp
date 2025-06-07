#include "VRMasteringInterface.hpp"
#include <algorithm>
#include <cmath>

namespace VR_DAW {

VRMasteringInterface::VRMasteringInterface() {
    initializeParameters();
}

VRMasteringInterface::~VRMasteringInterface() {
    shutdown();
}

void VRMasteringInterface::initialize() {
    initializeParameters();
    updateParameterPositions();
}

void VRMasteringInterface::update() {
    updateParameterPositions();
    calculateHapticFeedback();
    updateVisualizationData();
    updateKIFeedback();
    validateState();
}

void VRMasteringInterface::shutdown() {
    // Cleanup
}

void VRMasteringInterface::processHandInput(const glm::vec3& position, const glm::quat& rotation, bool isGrabbing) {
    handPositions.push_back(position);
    handRotations.push_back(rotation);
    this->isGrabbing.push_back(isGrabbing);

    if (isGrabbing) {
        // Parameter-Interaktion
        for (auto& param : parameters) {
            if (isHandNearParameter(position, param)) {
                float newValue = calculateParameterValue(position, param);
                setParameter(param.name, newValue);
                updateHapticFeedback();
            }
        }
    }
}

void VRMasteringInterface::processControllerInput(const glm::vec3& position, const glm::quat& rotation, float triggerValue) {
    triggerValues.push_back(triggerValue);

    // Controller-Interaktion
    for (auto& param : parameters) {
        if (isControllerNearParameter(position, param)) {
            float newValue = calculateParameterValue(position, param);
            setParameter(param.name, newValue);
            updateHapticFeedback();
        }
    }
}

void VRMasteringInterface::updateHapticFeedback() {
    calculateHapticFeedback();
    
    // Aktualisiere Haptisches Feedback
    for (auto& feedback : hapticFeedback) {
        // Implementiere Haptisches Feedback
        feedback.intensity = calculateFeedbackIntensity();
        feedback.frequency = calculateFeedbackFrequency();
        feedback.duration = calculateFeedbackDuration();
    }
}

void VRMasteringInterface::updateVisualization() {
    updateMeterVisualization();
    updateSpectrumVisualization();
    updateWaveformVisualization();
    updateParameterVisualization();
}

void VRMasteringInterface::setParameter(const std::string& name, float value) {
    auto it = std::find_if(parameters.begin(), parameters.end(),
                          [&name](const Parameter& p) { return p.name == name; });
    
    if (it != parameters.end()) {
        it->value = std::clamp(value, it->minValue, it->maxValue);
        if (kiIntegration) {
            kiIntegration->setParameter(name, it->value);
        }
    }
}

float VRMasteringInterface::getParameter(const std::string& name) const {
    auto it = std::find_if(parameters.begin(), parameters.end(),
                          [&name](const Parameter& p) { return p.name == name; });
    
    return it != parameters.end() ? it->value : 0.0f;
}

void VRMasteringInterface::automateParameter(const std::string& name, bool automated) {
    auto it = std::find_if(parameters.begin(), parameters.end(),
                          [&name](const Parameter& p) { return p.name == name; });
    
    if (it != parameters.end()) {
        it->automated = automated;
        if (kiIntegration) {
            kiIntegration->automateParameter(name, automated);
        }
    }
}

bool VRMasteringInterface::isParameterAutomated(const std::string& name) const {
    auto it = std::find_if(parameters.begin(), parameters.end(),
                          [&name](const Parameter& p) { return p.name == name; });
    
    return it != parameters.end() ? it->automated : false;
}

void VRMasteringInterface::connectKIIntegration(std::shared_ptr<VRKIIntegration> kiIntegration) {
    this->kiIntegration = kiIntegration;
}

void VRMasteringInterface::updateKIFeedback() {
    if (kiIntegration) {
        kiIntegration->updateKIFeedback();
    }
}

void VRMasteringInterface::applyKISuggestions() {
    if (kiIntegration) {
        // Implementiere KI-Vorschläge
    }
}

void VRMasteringInterface::updateMeterVisualization() {
    if (kiIntegration) {
        visualization.meterData = kiIntegration->getMeterData();
    }
}

void VRMasteringInterface::updateSpectrumVisualization() {
    if (kiIntegration) {
        visualization.spectrumData = kiIntegration->getSpectrumData();
    }
}

void VRMasteringInterface::updateWaveformVisualization() {
    if (kiIntegration) {
        visualization.waveformData = kiIntegration->getWaveformData();
    }
}

void VRMasteringInterface::updateParameterVisualization() {
    updateParameterPositions();
}

void VRMasteringInterface::initializeParameters() {
    // Initialisiere Mastering-Parameter
    parameters = {
        {"limiter_threshold", -1.0f, -20.0f, 0.0f, false, {}},
        {"limiter_release", 100.0f, 1.0f, 1000.0f, false, {}},
        {"eq_low", 0.0f, -12.0f, 12.0f, false, {}},
        {"eq_mid", 0.0f, -12.0f, 12.0f, false, {}},
        {"eq_high", 0.0f, -12.0f, 12.0f, false, {}},
        {"stereo_width", 1.0f, 0.0f, 2.0f, false, {}},
        {"loudness", 0.0f, -12.0f, 12.0f, false, {}},
        {"harmonic_exciter", 0.0f, 0.0f, 100.0f, false, {}},
        {"multiband_compression", 0.0f, 0.0f, 100.0f, false, {}}
    };
}

void VRMasteringInterface::updateParameterPositions() {
    // Aktualisiere 3D-Positionen der Parameter
    float radius = 2.0f;
    float height = 1.5f;
    int numParams = parameters.size();
    
    for (int i = 0; i < numParams; ++i) {
        float angle = (2.0f * M_PI * i) / numParams;
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        visualization.parameterPositions.push_back(glm::vec3(x, height, z));
    }
}

void VRMasteringInterface::calculateHapticFeedback() {
    // Berechne Haptisches Feedback basierend auf Parameter-Interaktion
    for (auto& feedback : hapticFeedback) {
        feedback.intensity = calculateFeedbackIntensity();
        feedback.frequency = calculateFeedbackFrequency();
        feedback.duration = calculateFeedbackDuration();
    }
}

void VRMasteringInterface::updateVisualizationData() {
    if (kiIntegration) {
        visualization.meterData = kiIntegration->getMeterData();
        visualization.spectrumData = kiIntegration->getSpectrumData();
        visualization.waveformData = kiIntegration->getWaveformData();
    }
}

void VRMasteringInterface::validateState() {
    // Validiere Zustand
    for (const auto& param : parameters) {
        if (param.value < param.minValue || param.value > param.maxValue) {
            handleErrors();
        }
    }
}

void VRMasteringInterface::handleErrors() {
    // Implementiere Fehlerbehandlung
}

// Hilfsfunktionen
bool VRMasteringInterface::isHandNearParameter(const glm::vec3& position, const Parameter& param) {
    auto it = std::find_if(parameters.begin(), parameters.end(),
                          [&param](const Parameter& p) { return p.name == param.name; });
    
    if (it != parameters.end()) {
        int index = std::distance(parameters.begin(), it);
        glm::vec3 paramPos = visualization.parameterPositions[index];
        float distance = glm::distance(position, paramPos);
        return distance < 0.2f; // 20cm Interaktionsradius
    }
    return false;
}

bool VRMasteringInterface::isControllerNearParameter(const glm::vec3& position, const Parameter& param) {
    return isHandNearParameter(position, param);
}

float VRMasteringInterface::calculateParameterValue(const glm::vec3& position, const Parameter& param) {
    auto it = std::find_if(parameters.begin(), parameters.end(),
                          [&param](const Parameter& p) { return p.name == param.name; });
    
    if (it != parameters.end()) {
        int index = std::distance(parameters.begin(), it);
        glm::vec3 paramPos = visualization.parameterPositions[index];
        float distance = glm::distance(position, paramPos);
        float normalizedValue = 1.0f - std::clamp(distance / 0.2f, 0.0f, 1.0f);
        return param.minValue + normalizedValue * (param.maxValue - param.minValue);
    }
    return param.value;
}

float VRMasteringInterface::calculateFeedbackIntensity() {
    // Implementiere Feedback-Intensitäts-Berechnung
    return 0.5f;
}

float VRMasteringInterface::calculateFeedbackFrequency() {
    // Implementiere Feedback-Frequenz-Berechnung
    return 100.0f;
}

float VRMasteringInterface::calculateFeedbackDuration() {
    // Implementiere Feedback-Dauer-Berechnung
    return 0.1f;
}

} // namespace VR_DAW 