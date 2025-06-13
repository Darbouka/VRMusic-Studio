#pragma once

#include <memory>
#include <vector>
#include <string>
#include <mutex>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../core/Logger.hpp"
#include "../core/EventSystem.hpp"
#include "../audio/AudioEngine.hpp"
#include "../vr/VRManager.hpp"

namespace VR_DAW {

class VRAudioIntegration {
public:
    static VRAudioIntegration& getInstance();

    bool initialize();
    void shutdown();
    bool isInitialized() const;

    // VR-Audio-Integration
    void update();
    void setAudioSourcePosition(const std::string& sourceId, const glm::vec3& position);
    void setAudioSourceOrientation(const std::string& sourceId, const glm::quat& orientation);
    void setAudioSourceVolume(const std::string& sourceId, float volume);
    void setAudioSourceMute(const std::string& sourceId, bool mute);
    
    // Controller-Audio-Feedback
    void playControllerFeedback(vr::TrackedDeviceIndex_t controllerIndex, float intensity);
    void stopControllerFeedback(vr::TrackedDeviceIndex_t controllerIndex);
    
    // Raum-Audio
    void setRoomSize(const glm::vec3& size);
    void setRoomReflection(float reflection);
    void setRoomAbsorption(float absorption);
    
    // Audio-Visualisierung
    void updateAudioVisualization();
    std::vector<float> getAudioSpectrum(const std::string& sourceId) const;
    std::vector<float> getAudioWaveform(const std::string& sourceId) const;

private:
    VRAudioIntegration();
    ~VRAudioIntegration();
    VRAudioIntegration(const VRAudioIntegration&) = delete;
    VRAudioIntegration& operator=(const VRAudioIntegration&) = delete;

    void updateListenerPosition();
    void updateAudioSources();
    void processControllerAudio();
    void updateRoomAudio();
    void updateVisualization();

    bool initialized;
    std::mutex mutex;
    Logger logger;

    // VR-Referenzen
    VRManager& vrManager;
    AudioEngine& audioEngine;

    // Audio-Quellen
    struct AudioSource {
        std::string id;
        glm::vec3 position;
        glm::quat orientation;
        float volume;
        bool isMuted;
        std::vector<float> spectrum;
        std::vector<float> waveform;
    };
    std::unordered_map<std::string, AudioSource> audioSources;

    // Raum-Audio
    glm::vec3 roomSize;
    float roomReflection;
    float roomAbsorption;

    // Controller-Audio
    struct ControllerAudio {
        bool isPlaying;
        float intensity;
        float frequency;
    };
    std::unordered_map<vr::TrackedDeviceIndex_t, ControllerAudio> controllerAudio;
};

} // namespace VR_DAW 