#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "SequencerKIIntegration.hpp"

namespace VR_DAW {

class VRSequencerInterface {
public:
    VRSequencerInterface();
    ~VRSequencerInterface();

    void initialize();
    void update();
    void shutdown();

    // VR-Interaktion
    void processHandInput(const glm::vec3& position, const glm::quat& rotation, bool isGrabbing);
    void processControllerInput(const glm::vec3& position, const glm::quat& rotation, float triggerValue);
    void updateHapticFeedback();
    void updateVisualization();

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
    void connectKIIntegration(std::shared_ptr<SequencerKIIntegration> kiIntegration);
    void updateKIFeedback();
    void applyKISuggestions();

    // Visualisierung
    void updatePatternVisualization();
    void updateSequenceVisualization();
    void updateAutomationVisualization();
    void updateAnalysisVisualization();

private:
    // VR-Komponenten
    std::shared_ptr<SequencerKIIntegration> kiIntegration;
    std::vector<glm::vec3> handPositions;
    std::vector<glm::quat> handRotations;
    std::vector<bool> isGrabbing;
    std::vector<float> triggerValues;

    // Pattern
    struct Pattern {
        std::string name;
        std::vector<float> data;
        std::vector<float> automation;
        bool isAutomated;
        glm::vec3 position;
        glm::quat rotation;
    };
    std::vector<Pattern> patterns;

    // Sequenz
    struct Sequence {
        std::vector<Pattern> patterns;
        float tempo;
        float swing;
        bool isPlaying;
        bool isPaused;
    };
    Sequence currentSequence;

    // Visualisierung
    struct Visualization {
        std::vector<glm::vec3> patternPositions;
        std::vector<glm::quat> patternRotations;
        std::vector<float> automationData;
        std::vector<float> analysisData;
    };
    Visualization visualization;

    // Haptisches Feedback
    struct HapticFeedback {
        float intensity;
        float frequency;
        float duration;
    };
    std::vector<HapticFeedback> hapticFeedback;

    // Hilfsfunktionen
    void initializePatterns();
    void updatePatternPositions();
    void calculateHapticFeedback();
    void updateVisualizationData();
    void validateState();
    void handleErrors();
};

} // namespace VR_DAW 