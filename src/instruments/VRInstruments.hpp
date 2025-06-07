#pragma once

#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "../audio/AudioEngine.hpp"
#include "../vr/VRInterface.hpp"

namespace VR_DAW {

// Revolutionäre Instrumenten-Typen
enum class InstrumentType {
    Piano,
    Guitar,
    Drums,
    Violin,
    Saxophone,
    Synthesizer,
    Custom
};

// Revolutionäre Spielposition
enum class PlayingPosition {
    Standing,
    Sitting,
    Custom
};

// Revolutionäre Instrumenten-Interaktion
struct InstrumentInteraction {
    glm::vec3 position;
    glm::vec3 rotation;
    float velocity;
    float pressure;
    bool isTouching;
    std::vector<glm::vec3> touchPoints;
};

// Revolutionäre Instrumenten-Visualisierung
struct InstrumentVisualization {
    std::string modelPath;
    std::string texturePath;
    glm::vec3 scale;
    glm::vec3 position;
    glm::vec3 rotation;
    bool isVisible;
    float animationSpeed;
    std::vector<std::string> animationStates;
};

// Revolutionäre Instrumenten-Audio
struct InstrumentAudio {
    std::string samplePath;
    float volume;
    float pitch;
    float reverb;
    float delay;
    bool isMuted;
    std::vector<float> effects;
};

// Revolutionäre Basis-Instrumenten-Klasse
class VRInstrument {
public:
    VRInstrument(InstrumentType type, const std::string& name);
    virtual ~VRInstrument() = default;

    // Revolutionäre Initialisierung
    virtual void initialize();
    virtual void shutdown();

    // Revolutionäre Spiel-Funktionen
    virtual void playNote(int note, float velocity);
    virtual void stopNote(int note);
    virtual void setPitch(float pitch);
    virtual void setVolume(float volume);

    // Revolutionäre VR-Interaktion
    virtual void handleVRInteraction(const InstrumentInteraction& interaction);
    virtual void updateVisualization();
    virtual void updateAudio();

    // Revolutionäre Positionierung
    virtual void setPosition(const glm::vec3& position);
    virtual void setRotation(const glm::vec3& rotation);
    virtual void setScale(const glm::vec3& scale);

    // Revolutionäre Spielposition
    virtual void setPlayingPosition(PlayingPosition position);
    virtual void adjustForPlayerHeight(float height);

    // Revolutionäre Echtzeit-Reaktion
    virtual void reactToMovement(const glm::vec3& movement);
    virtual void reactToAudio(float audioLevel);
    virtual void reactToTouch(const glm::vec3& touchPoint);

protected:
    InstrumentType type;
    std::string name;
    InstrumentVisualization visualization;
    InstrumentAudio audio;
    PlayingPosition playingPosition;
    bool isActive;
};

// Revolutionäre Piano-Implementierung
class VRPiano : public VRInstrument {
public:
    VRPiano();
    void initialize() override;
    void handleVRInteraction(const InstrumentInteraction& interaction) override;
    void updateVisualization() override;
    void updateAudio() override;
};

// Revolutionäre Gitarren-Implementierung
class VRGuitar : public VRInstrument {
public:
    VRGuitar();
    void initialize() override;
    void handleVRInteraction(const InstrumentInteraction& interaction) override;
    void updateVisualization() override;
    void updateAudio() override;
};

// Revolutionäre Schlagzeug-Implementierung
class VRDrums : public VRInstrument {
public:
    VRDrums();
    void initialize() override;
    void handleVRInteraction(const InstrumentInteraction& interaction) override;
    void updateVisualization() override;
    void updateAudio() override;
};

// Revolutionäre Violinen-Implementierung
class VRViolin : public VRInstrument {
public:
    VRViolin();
    void initialize() override;
    void handleVRInteraction(const InstrumentInteraction& interaction) override;
    void updateVisualization() override;
    void updateAudio() override;
};

// Revolutionäre Saxophon-Implementierung
class VRSaxophone : public VRInstrument {
public:
    VRSaxophone();
    void initialize() override;
    void handleVRInteraction(const InstrumentInteraction& interaction) override;
    void updateVisualization() override;
    void updateAudio() override;
};

// Revolutionäre Synthesizer-Implementierung
class VRSynthesizer : public VRInstrument {
public:
    VRSynthesizer();
    void initialize() override;
    void handleVRInteraction(const InstrumentInteraction& interaction) override;
    void updateVisualization() override;
    void updateAudio() override;
};

// Revolutionäre Instrumenten-Manager
class VRInstrumentManager {
public:
    VRInstrumentManager();
    ~VRInstrumentManager();

    // Revolutionäre Instrumenten-Verwaltung
    void initialize();
    void shutdown();
    void update();

    // Revolutionäre Instrumenten-Erstellung
    std::shared_ptr<VRInstrument> createInstrument(InstrumentType type, const std::string& name);
    void removeInstrument(const std::string& name);
    std::shared_ptr<VRInstrument> getInstrument(const std::string& name);

    // Revolutionäre Raum-Verwaltung
    void setRoomSize(const glm::vec3& size);
    void setRoomAcoustics(float reverb, float reflection);
    void setRoomLighting(const glm::vec3& color, float intensity);

    // Revolutionäre Spieler-Verwaltung
    void setPlayerPosition(const glm::vec3& position);
    void setPlayerHeight(float height);
    void setPlayerView(const glm::vec3& direction);

private:
    std::vector<std::shared_ptr<VRInstrument>> instruments;
    glm::vec3 roomSize;
    float roomReverb;
    float roomReflection;
    glm::vec3 roomLighting;
    float roomLightIntensity;
    glm::vec3 playerPosition;
    float playerHeight;
    glm::vec3 playerView;
};

} // namespace VR_DAW 