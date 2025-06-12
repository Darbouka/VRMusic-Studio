#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

namespace VRMusicStudio {

class VoiceAI {
public:
    // Schauspieler-Profil
    struct ActorProfile {
        std::string name;
        std::string originalVoice;
        std::string germanVoice;
        std::string avatarModel;
        std::map<std::string, float> voiceParameters;
        std::map<std::string, float> avatarParameters;
    };

    // Voice-Transformation
    struct VoiceTransform {
        float pitch;
        float formant;
        float timbre;
        float emotion;
        float accent;
        bool enabled;
    };

    // Avatar-Transformation
    struct AvatarTransform {
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;
        float expression;
        float animation;
        bool enabled;
    };

    // Emotion-Parameter
    struct EmotionParameters {
        float happiness;
        float sadness;
        float anger;
        float fear;
        float surprise;
        float neutral;
    };

    VoiceAI();
    ~VoiceAI();

    // Schauspieler-Management
    void addActorProfile(const ActorProfile& profile);
    void removeActorProfile(const std::string& name);
    void setActiveActor(const std::string& name);

    // Voice-Transformation
    void setVoiceTransform(const VoiceTransform& transform);
    void processVoice(float* buffer, int numSamples);

    // Avatar-Transformation
    void setAvatarTransform(const AvatarTransform& transform);
    void updateAvatar();

    // Emotion-Kontrolle
    void setEmotionParameters(const EmotionParameters& params);
    void processEmotion(float* buffer, int numSamples);

    // Callback-Setter
    void setVoiceCallback(std::function<void(const std::vector<float>&)> callback);
    void setAvatarCallback(std::function<void(const glm::mat4&)> callback);
    void setEmotionCallback(std::function<void(const EmotionParameters&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; 