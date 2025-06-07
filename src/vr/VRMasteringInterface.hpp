#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "VRKIIntegration.hpp"

namespace VR_DAW {

class VRMasteringInterface {
public:
    VRMasteringInterface();
    ~VRMasteringInterface();

    void initialize();
    void update();
    void shutdown();

    // VR-Interaktion
    void processHandInput(const glm::vec3& position, const glm::quat& rotation, bool isGrabbing);
    void processControllerInput(const glm::vec3& position, const glm::quat& rotation, float triggerValue);
    void updateHapticFeedback();
    void updateVisualization();

    // Parameter-Steuerung
    void setParameter(const std::string& name, float value);
    float getParameter(const std::string& name) const;
    void automateParameter(const std::string& name, bool automated);
    bool isParameterAutomated(const std::string& name) const;

    // KI-Integration
    void connectKIIntegration(std::shared_ptr<VRKIIntegration> kiIntegration);
    void updateKIFeedback();
    void applyKISuggestions();

    // Visualisierung
    void updateMeterVisualization();
    void updateSpectrumVisualization();
    void updateWaveformVisualization();
    void updateParameterVisualization();

private:
    // VR-Komponenten
    std::shared_ptr<VRKIIntegration> kiIntegration;
    std::vector<glm::vec3> handPositions;
    std::vector<glm::quat> handRotations;
    std::vector<bool> isGrabbing;
    std::vector<float> triggerValues;

    // Parameter
    struct Parameter {
        std::string name;
        float value;
        float minValue;
        float maxValue;
        bool automated;
        std::vector<float> automationCurve;
    };
    std::vector<Parameter> parameters;

    // Visualisierung
    struct Visualization {
        std::vector<float> meterData;
        std::vector<float> spectrumData;
        std::vector<float> waveformData;
        std::vector<glm::vec3> parameterPositions;
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
    void initializeParameters();
    void updateParameterPositions();
    void calculateHapticFeedback();
    void updateVisualizationData();
    void validateState();
    void handleErrors();
};

} // namespace VR_DAW 