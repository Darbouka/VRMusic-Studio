#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include "../core/Logger.hpp"

namespace VR_DAW {

struct AudioSource {
    ALuint source;
    ALuint buffer;
    glm::vec3 position;
    glm::quat orientation;
    float volume;
    float pitch;
    bool isLooping;
    bool is3D;
    std::map<std::string, float> effects;
};

struct AudioListener {
    glm::vec3 position;
    glm::quat orientation;
    float volume;
};

class OpenALIntegration {
public:
    static OpenALIntegration& getInstance();

    bool initialize();
    void shutdown();

    // Audio-Quellen-Management
    std::shared_ptr<AudioSource> createSource(const std::string& name);
    bool loadAudioFile(const std::string& filePath, ALuint& buffer);
    void setSourcePosition(const std::string& sourceName, const glm::vec3& position);
    void setListenerPosition(const glm::vec3& position);
    bool playSource(const std::string& sourceName);

    // Effekt-Verarbeitung
    bool applyEffect(const std::string& sourceName, 
                    const std::string& effectName,
                    const std::map<std::string, float>& parameters);

    // Raum-Audio
    void setRoomProperties(float size, float reflection, float absorption);

private:
    OpenALIntegration();
    ~OpenALIntegration();

    // Singleton-Pattern
    OpenALIntegration(const OpenALIntegration&) = delete;
    OpenALIntegration& operator=(const OpenALIntegration&) = delete;

    bool initializeOpenAL();
    void cleanupOpenAL();
    bool checkALError(const std::string& operation);
    void apply3DAudioEffects(const std::string& sourceName);
    bool convertToMono(const std::vector<float>& input, std::vector<float>& output);

    std::unordered_map<std::string, std::shared_ptr<AudioSource>> sources;
    AudioListener listener;
    ALCdevice* device;
    ALCcontext* context;
    ALuint reverbEffect;
    ALuint echoEffect;
    ALuint distortionEffect;
    float roomSize;
    float roomReflection;
    float roomAbsorption;
    std::map<std::string, float> reverbProperties;
    std::map<std::string, float> occlusionProperties;
    mutable std::mutex mutex;
    bool initialized;
    Logger logger;
};

} // namespace VR_DAW 