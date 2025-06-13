#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "../vr/VRInterface.hpp"
#include "../audio/AudioEngine.hpp"

namespace VR_DAW {

// Revolutionäre Schauspieler-Daten
struct ActorData {
    std::string name;
    std::string modelPath;
    std::string texturePath;
    std::string voiceId;
    std::string animationSet;
    float height;
    glm::vec3 scale;
};

// Revolutionäre Hologramm-Einstellungen
struct HologramSettings {
    float opacity;
    float glowIntensity;
    float particleDensity;
    float animationSpeed;
    bool isVisible;
    glm::vec3 color;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

// Revolutionäre KI-Interaktion
struct AIInteraction {
    std::string command;
    std::string response;
    float emotion;
    std::vector<std::string> gestures;
    bool isSpeaking;
    bool isAnimating;
};

// Revolutionäre Hologramm-KI-Klasse
class HolographicAI {
public:
    HolographicAI();
    ~HolographicAI();

    // Revolutionäre Initialisierung
    void initialize();
    void shutdown();
    void update();

    // Revolutionäre Schauspieler-Verwaltung
    void setActor(const ActorData& actor);
    void setVoice(const std::string& voiceId);
    void setAppearance(const std::string& modelPath, const std::string& texturePath);

    // Revolutionäre Hologramm-Einstellungen
    void setHologramSettings(const HologramSettings& settings);
    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotation);
    void setScale(const glm::vec3& scale);

    // Revolutionäre KI-Interaktion
    void processCommand(const std::string& command);
    void speak(const std::string& text);
    void animate(const std::string& animation);
    void gesture(const std::string& gesture);

    // Revolutionäre Raum-Anpassung
    void adaptToRoomSize(const glm::vec3& roomSize);
    void adjustHeight(float playerHeight);
    void updateLighting(const glm::vec3& lightColor, float intensity);

    // Revolutionäre Echtzeit-Reaktion
    void reactToPlayer(const glm::vec3& playerPosition, const glm::vec3& playerView);
    void reactToAudio(float audioLevel);
    void reactToMovement(const glm::vec3& movement);

private:
    ActorData currentActor;
    HologramSettings settings;
    AIInteraction currentInteraction;
    bool isInitialized;
    float lastUpdateTime;

    // Revolutionäre interne Funktionen
    void updateHologram();
    void updateVoice();
    void updateAnimation();
    void updateGestures();
    void processEmotion(float emotion);
    void adjustHologramSize();
    void syncVoiceWithAnimation();
};

// Revolutionäre Hologramm-Manager
class HologramManager {
public:
    HologramManager();
    ~HologramManager();

    // Revolutionäre Manager-Funktionen
    void initialize();
    void shutdown();
    void update();

    // Revolutionäre Hologramm-Verwaltung
    std::shared_ptr<HolographicAI> createHologram(const ActorData& actor);
    void removeHologram(const std::string& id);
    std::shared_ptr<HolographicAI> getHologram(const std::string& id);

    // Revolutionäre Raum-Verwaltung
    void setRoomSize(const glm::vec3& size);
    void setRoomLighting(const glm::vec3& color, float intensity);
    void setRoomAcoustics(float reverb, float reflection);

    // Revolutionäre Spieler-Verwaltung
    void setPlayerPosition(const glm::vec3& position);
    void setPlayerHeight(float height);
    void setPlayerView(const glm::vec3& direction);

private:
    std::vector<std::shared_ptr<HolographicAI>> holograms;
    glm::vec3 roomSize;
    glm::vec3 roomLighting;
    float roomLightIntensity;
    float roomReverb;
    float roomReflection;
    glm::vec3 playerPosition;
    float playerHeight;
    glm::vec3 playerView;
};

} // namespace VR_DAW 