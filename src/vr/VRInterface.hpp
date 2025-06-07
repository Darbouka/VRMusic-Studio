#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <glm/glm.hpp>
#include "../core/Logger.hpp"
#include "../core/EventSystem.hpp"

namespace VR_DAW {

class VRInterface {
public:
    // Revolutionäre VR-Interaktionsdaten
    struct MotionData {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 velocity;
        glm::vec3 acceleration;
        float gripStrength;
        float triggerValue;
        bool isGrabbing;
        bool isTriggering;
    };

    // Revolutionäre VR-Gesten
    struct GestureData {
        enum class Type {
            Grab,
            Release,
            Point,
            Wave,
            Pinch,
            Swipe,
            Rotate,
            Scale
        };
        
        Type type;
        glm::vec3 position;
        glm::vec3 direction;
        float intensity;
        float duration;
    };

    // Revolutionäre VR-Raumdaten
    struct SpatialData {
        glm::vec3 roomSize;
        glm::vec3 playArea;
        std::vector<glm::vec3> boundaries;
        std::vector<glm::vec3> obstacles;
    };

    // Revolutionäre VR-Interaktionsfunktionen
    void initialize();
    void shutdown();
    void update();
    void render();

    // Revolutionäre Bewegungssteuerung
    void handleMotion(const MotionData& motion);
    void processGesture(const GestureData& gesture);
    void updateSpatialData(const SpatialData& spatial);

    // Revolutionäre Interaktionsfunktionen
    void grabObject(const std::string& objectId);
    void releaseObject(const std::string& objectId);
    void manipulateObject(const std::string& objectId, const MotionData& motion);
    void interactWithUI(const std::string& uiElementId, const MotionData& motion);

    // Revolutionäre Raumfunktionen
    void definePlayArea(const glm::vec3& size);
    void addBoundary(const glm::vec3& point);
    void addObstacle(const glm::vec3& position, const glm::vec3& size);
    void updateRoomScale(float scale);

    // Revolutionäre Feedback-Funktionen
    void provideHapticFeedback(const std::string& controllerId, float intensity, float duration);
    void provideAudioFeedback(const std::string& sourceId, const std::string& soundId);
    void provideVisualFeedback(const std::string& objectId, const glm::vec4& color);

    // Revolutionäre Tracking-Funktionen
    void trackController(const std::string& controllerId);
    void trackObject(const std::string& objectId);
    void trackUser(const std::string& userId);
    void updateTracking(const std::string& targetId, const MotionData& motion);

private:
    // Revolutionäre interne Funktionen
    void processMotionData(const MotionData& motion);
    void updateGestureRecognition(const MotionData& motion);
    void handleSpatialInteraction(const MotionData& motion);
    void updateObjectPhysics(const std::string& objectId, const MotionData& motion);
    void processHapticFeedback(const std::string& controllerId, float intensity);
    void updateAudioSpatialization(const std::string& sourceId, const glm::vec3& position);
    void renderVisualEffects(const std::string& objectId, const glm::vec4& color);
};

} // namespace VR_DAW 