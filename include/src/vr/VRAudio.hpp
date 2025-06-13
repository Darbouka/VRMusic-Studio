#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <portaudio.h>

namespace VRMusicStudio {

class VRAudio {
public:
    struct AudioSource {
        std::string id;
        std::string filePath;
        glm::vec3 position;
        float volume;
        float pitch;
        bool loop;
        bool playing;
        PaStream* stream;
    };
    
    struct AudioListener {
        glm::vec3 position;
        glm::vec3 forward;
        glm::vec3 up;
        float volume;
    };

    VRAudio();
    ~VRAudio();

    bool initialize();
    void shutdown();
    void update();

    // Audio-Quellen
    std::string createSource(const std::string& filePath,
                           const glm::vec3& position = glm::vec3(0.0f),
                           float volume = 1.0f,
                           float pitch = 1.0f,
                           bool loop = false);
    void destroySource(const std::string& sourceId);
    void playSource(const std::string& sourceId);
    void pauseSource(const std::string& sourceId);
    void stopSource(const std::string& sourceId);
    void setSourcePosition(const std::string& sourceId, const glm::vec3& position);
    void setSourceVolume(const std::string& sourceId, float volume);
    void setSourcePitch(const std::string& sourceId, float pitch);
    void setSourceLoop(const std::string& sourceId, bool loop);
    bool isSourcePlaying(const std::string& sourceId) const;
    float getSourceVolume(const std::string& sourceId) const;
    float getSourcePitch(const std::string& sourceId) const;
    bool isSourceLooping(const std::string& sourceId) const;

    // Audio-Listener
    void setListenerPosition(const glm::vec3& position);
    void setListenerOrientation(const glm::vec3& forward, const glm::vec3& up);
    void setListenerVolume(float volume);
    glm::vec3 getListenerPosition() const;
    float getListenerVolume() const;

    // 3D-Audio
    void set3DAudioEnabled(bool enabled);
    bool is3DAudioEnabled() const;
    void set3DAudioDistance(float distance);
    float get3DAudioDistance() const;
    void set3DAudioRolloff(float rolloff);
    float get3DAudioRolloff() const;

    // Effekte
    void addEffect(const std::string& sourceId, const std::string& effectType);
    void removeEffect(const std::string& sourceId, const std::string& effectType);
    void setEffectParameter(const std::string& sourceId,
                           const std::string& effectType,
                           const std::string& parameter,
                           float value);
    float getEffectParameter(const std::string& sourceId,
                            const std::string& effectType,
                            const std::string& parameter) const;

    // Streaming
    void startStreaming(const std::string& sourceId);
    void stopStreaming(const std::string& sourceId);
    bool isStreaming(const std::string& sourceId) const;
    void setStreamingCallback(const std::string& sourceId,
                             std::function<void(const std::vector<float>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void initializePortAudio();
    void processAudio();
    void update3DAudio();
    void applyEffects(AudioSource& source);
    void handleStreaming(AudioSource& source);
    float calculate3DVolume(const AudioSource& source) const;
    glm::vec3 calculate3DPosition(const AudioSource& source) const;
};

} // namespace VRMusicStudio 