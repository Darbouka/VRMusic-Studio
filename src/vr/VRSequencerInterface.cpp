#include "VRSequencerInterface.hpp"
#include <algorithm>
#include <cmath>

namespace VR_DAW {

VRSequencerInterface::VRSequencerInterface() {
    initializePatterns();
}

VRSequencerInterface::~VRSequencerInterface() {
    shutdown();
}

void VRSequencerInterface::initialize() {
    initializePatterns();
    updatePatternPositions();
}

void VRSequencerInterface::update() {
    updatePatternPositions();
    calculateHapticFeedback();
    updateVisualizationData();
    updateKIFeedback();
    validateState();
}

void VRSequencerInterface::shutdown() {
    // Cleanup
}

void VRSequencerInterface::processHandInput(const glm::vec3& position, const glm::quat& rotation, bool isGrabbing) {
    handPositions.push_back(position);
    handRotations.push_back(rotation);
    this->isGrabbing.push_back(isGrabbing);

    if (isGrabbing) {
        // Pattern-Interaktion
        for (auto& pattern : patterns) {
            if (isHandNearPattern(position, pattern)) {
                editPattern(pattern.name);
                updateHapticFeedback();
            }
        }
    }
}

void VRSequencerInterface::processControllerInput(const glm::vec3& position, const glm::quat& rotation, float triggerValue) {
    triggerValues.push_back(triggerValue);

    // Controller-Interaktion
    for (auto& pattern : patterns) {
        if (isControllerNearPattern(position, pattern)) {
            editPattern(pattern.name);
            updateHapticFeedback();
        }
    }
}

void VRSequencerInterface::updateHapticFeedback() {
    calculateHapticFeedback();
    
    // Aktualisiere Haptisches Feedback
    for (auto& feedback : hapticFeedback) {
        // Implementiere Haptisches Feedback
        feedback.intensity = calculateFeedbackIntensity();
        feedback.frequency = calculateFeedbackFrequency();
        feedback.duration = calculateFeedbackDuration();
    }
}

void VRSequencerInterface::updateVisualization() {
    updatePatternVisualization();
    updateSequenceVisualization();
    updateAutomationVisualization();
    updateAnalysisVisualization();
}

void VRSequencerInterface::createPattern(const std::string& name) {
    Pattern pattern;
    pattern.name = name;
    pattern.isAutomated = false;
    patterns.push_back(pattern);
    
    if (kiIntegration) {
        kiIntegration->createPattern(name);
    }
}

void VRSequencerInterface::editPattern(const std::string& name) {
    auto it = std::find_if(patterns.begin(), patterns.end(),
                          [&name](const Pattern& p) { return p.name == name; });
    
    if (it != patterns.end()) {
        if (kiIntegration) {
            kiIntegration->editPattern(name);
        }
    }
}

void VRSequencerInterface::deletePattern(const std::string& name) {
    auto it = std::find_if(patterns.begin(), patterns.end(),
                          [&name](const Pattern& p) { return p.name == name; });
    
    if (it != patterns.end()) {
        patterns.erase(it);
        if (kiIntegration) {
            kiIntegration->deletePattern(name);
        }
    }
}

void VRSequencerInterface::savePattern(const std::string& name) {
    auto it = std::find_if(patterns.begin(), patterns.end(),
                          [&name](const Pattern& p) { return p.name == name; });
    
    if (it != patterns.end()) {
        if (kiIntegration) {
            kiIntegration->savePattern(name);
        }
    }
}

void VRSequencerInterface::loadPattern(const std::string& name) {
    if (kiIntegration) {
        kiIntegration->loadPattern(name);
    }
}

void VRSequencerInterface::playSequence() {
    currentSequence.isPlaying = true;
    currentSequence.isPaused = false;
}

void VRSequencerInterface::pauseSequence() {
    currentSequence.isPaused = true;
}

void VRSequencerInterface::stopSequence() {
    currentSequence.isPlaying = false;
    currentSequence.isPaused = false;
}

void VRSequencerInterface::setTempo(float tempo) {
    currentSequence.tempo = std::clamp(tempo, 40.0f, 240.0f);
}

void VRSequencerInterface::setSwing(float swing) {
    currentSequence.swing = std::clamp(swing, 0.0f, 1.0f);
}

void VRSequencerInterface::connectKIIntegration(std::shared_ptr<SequencerKIIntegration> kiIntegration) {
    this->kiIntegration = kiIntegration;
}

void VRSequencerInterface::updateKIFeedback() {
    if (kiIntegration) {
        kiIntegration->updateKIFeedback();
    }
}

void VRSequencerInterface::applyKISuggestions() {
    if (kiIntegration) {
        kiIntegration->applyKISuggestions();
    }
}

void VRSequencerInterface::updatePatternVisualization() {
    updatePatternPositions();
}

void VRSequencerInterface::updateSequenceVisualization() {
    // Aktualisiere Sequenz-Visualisierung
}

void VRSequencerInterface::updateAutomationVisualization() {
    // Aktualisiere Automatisierungs-Visualisierung
}

void VRSequencerInterface::updateAnalysisVisualization() {
    // Aktualisiere Analyse-Visualisierung
}

void VRSequencerInterface::initializePatterns() {
    // Initialisiere Patterns
    patterns.clear();
    currentSequence.patterns.clear();
    currentSequence.tempo = 120.0f;
    currentSequence.swing = 0.0f;
    currentSequence.isPlaying = false;
    currentSequence.isPaused = false;
}

void VRSequencerInterface::updatePatternPositions() {
    // Aktualisiere 3D-Positionen der Patterns
    float radius = 2.0f;
    float height = 1.5f;
    int numPatterns = patterns.size();
    
    for (int i = 0; i < numPatterns; ++i) {
        float angle = (2.0f * M_PI * i) / numPatterns;
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        patterns[i].position = glm::vec3(x, height, z);
        patterns[i].rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }
}

void VRSequencerInterface::calculateHapticFeedback() {
    // Berechne Haptisches Feedback basierend auf Pattern-Interaktion
    for (auto& feedback : hapticFeedback) {
        feedback.intensity = calculateFeedbackIntensity();
        feedback.frequency = calculateFeedbackFrequency();
        feedback.duration = calculateFeedbackDuration();
    }
}

void VRSequencerInterface::updateVisualizationData() {
    // Aktualisiere Visualisierungsdaten
    visualization.patternPositions.clear();
    visualization.patternRotations.clear();
    
    for (const auto& pattern : patterns) {
        visualization.patternPositions.push_back(pattern.position);
        visualization.patternRotations.push_back(pattern.rotation);
    }
}

void VRSequencerInterface::validateState() {
    // Validiere Zustand
    for (const auto& pattern : patterns) {
        if (!validatePattern(pattern)) {
            handleErrors();
        }
    }
}

void VRSequencerInterface::handleErrors() {
    // Implementiere Fehlerbehandlung
}

// Hilfsfunktionen
bool VRSequencerInterface::isHandNearPattern(const glm::vec3& position, const Pattern& pattern) {
    float distance = glm::distance(position, pattern.position);
    return distance < 0.2f; // 20cm Interaktionsradius
}

bool VRSequencerInterface::isControllerNearPattern(const glm::vec3& position, const Pattern& pattern) {
    return isHandNearPattern(position, pattern);
}

float VRSequencerInterface::calculateFeedbackIntensity() {
    // Implementiere Feedback-Intensitäts-Berechnung
    return 0.5f;
}

float VRSequencerInterface::calculateFeedbackFrequency() {
    // Implementiere Feedback-Frequenz-Berechnung
    return 100.0f;
}

float VRSequencerInterface::calculateFeedbackDuration() {
    // Implementiere Feedback-Dauer-Berechnung
    return 0.1f;
}

bool VRSequencerInterface::validatePattern(const Pattern& pattern) {
    // Implementiere Pattern-Validierung
    return true;
}

} // namespace VR_DAW 