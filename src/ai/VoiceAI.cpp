#include "VoiceAI.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>
#include <torch/torch.h>

namespace VRMusicStudio {

struct VoiceAI::Impl {
    // Schauspieler-Profile
    std::map<std::string, ActorProfile> actorProfiles;
    std::string activeActor;

    // Voice-Transformation
    VoiceTransform voiceTransform;
    std::vector<float> voiceBuffer;
    torch::jit::script::Module voiceModel;

    // Avatar-Transformation
    AvatarTransform avatarTransform;
    torch::jit::script::Module avatarModel;

    // Emotion-Parameter
    EmotionParameters emotionParams;
    std::vector<float> emotionBuffer;

    // Zustandsvariablen
    bool isProcessing;
    float currentTime;

    // Callbacks
    std::function<void(const std::vector<float>&)> voiceCallback;
    std::function<void(const glm::mat4&)> avatarCallback;
    std::function<void(const EmotionParameters&)> emotionCallback;

    Impl() : isProcessing(false), currentTime(0.0f) {
        try {
            // Lade KI-Modelle
            voiceModel = torch::jit::load("models/voice_transformer.pt");
            avatarModel = torch::jit::load("models/avatar_transformer.pt");
        } catch (const c10::Error& e) {
            spdlog::error("Fehler beim Laden der KI-Modelle: {}", e.what());
        }
    }
};

VoiceAI::VoiceAI() : pImpl(std::make_unique<Impl>()) {
    // Initialisiere Standard-Parameter
    pImpl->voiceTransform = {1.0f, 1.0f, 1.0f, 0.5f, 0.0f, true};
    pImpl->avatarTransform = {
        glm::vec3(0.0f),
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f),
        0.5f,
        0.0f,
        true
    };
    pImpl->emotionParams = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
}

VoiceAI::~VoiceAI() = default;

void VoiceAI::addActorProfile(const ActorProfile& profile) {
    pImpl->actorProfiles[profile.name] = profile;
}

void VoiceAI::removeActorProfile(const std::string& name) {
    pImpl->actorProfiles.erase(name);
}

void VoiceAI::setActiveActor(const std::string& name) {
    if (pImpl->actorProfiles.find(name) != pImpl->actorProfiles.end()) {
        pImpl->activeActor = name;
        
        // Aktualisiere Avatar
        const auto& profile = pImpl->actorProfiles[name];
        pImpl->avatarTransform.scale = glm::vec3(1.0f);
        pImpl->avatarTransform.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        
        // Aktualisiere Voice-Parameter
        pImpl->voiceTransform.pitch = profile.voiceParameters["pitch"];
        pImpl->voiceTransform.formant = profile.voiceParameters["formant"];
        pImpl->voiceTransform.timbre = profile.voiceParameters["timbre"];
    }
}

void VoiceAI::setVoiceTransform(const VoiceTransform& transform) {
    pImpl->voiceTransform = transform;
}

void VoiceAI::processVoice(float* buffer, int numSamples) {
    if (!pImpl->voiceTransform.enabled || pImpl->activeActor.empty()) return;

    try {
        // Erstelle Input-Tensor
        std::vector<torch::jit::IValue> inputs;
        auto inputTensor = torch::from_blob(buffer, {1, numSamples}, torch::kFloat32);
        inputs.push_back(inputTensor);

        // Füge Transformations-Parameter hinzu
        auto params = torch::tensor({
            pImpl->voiceTransform.pitch,
            pImpl->voiceTransform.formant,
            pImpl->voiceTransform.timbre,
            pImpl->voiceTransform.emotion,
            pImpl->voiceTransform.accent
        });
        inputs.push_back(params);

        // Führe Transformation durch
        auto output = pImpl->voiceModel.forward(inputs).toTensor();
        
        // Kopiere Ergebnis zurück
        std::memcpy(buffer, output.data_ptr<float>(), numSamples * sizeof(float));

        if (pImpl->voiceCallback) {
            pImpl->voiceCallback(std::vector<float>(buffer, buffer + numSamples));
        }
    } catch (const c10::Error& e) {
        spdlog::error("Fehler bei der Voice-Transformation: {}", e.what());
    }
}

void VoiceAI::setAvatarTransform(const AvatarTransform& transform) {
    pImpl->avatarTransform = transform;
}

void VoiceAI::updateAvatar() {
    if (!pImpl->avatarTransform.enabled || pImpl->activeActor.empty()) return;

    try {
        // Erstelle Input-Tensor für Avatar
        std::vector<torch::jit::IValue> inputs;
        auto position = torch::tensor({
            pImpl->avatarTransform.position.x,
            pImpl->avatarTransform.position.y,
            pImpl->avatarTransform.position.z
        });
        auto rotation = torch::tensor({
            pImpl->avatarTransform.rotation.x,
            pImpl->avatarTransform.rotation.y,
            pImpl->avatarTransform.rotation.z,
            pImpl->avatarTransform.rotation.w
        });
        auto scale = torch::tensor({
            pImpl->avatarTransform.scale.x,
            pImpl->avatarTransform.scale.y,
            pImpl->avatarTransform.scale.z
        });
        auto expression = torch::tensor({pImpl->avatarTransform.expression});
        auto animation = torch::tensor({pImpl->avatarTransform.animation});

        inputs.push_back(position);
        inputs.push_back(rotation);
        inputs.push_back(scale);
        inputs.push_back(expression);
        inputs.push_back(animation);

        // Führe Avatar-Transformation durch
        auto output = pImpl->avatarModel.forward(inputs).toTensor();
        
        // Konvertiere zu glm::mat4 für Rendering
        glm::mat4 transform;
        std::memcpy(&transform, output.data_ptr<float>(), sizeof(glm::mat4));

        if (pImpl->avatarCallback) {
            pImpl->avatarCallback(transform);
        }
    } catch (const c10::Error& e) {
        spdlog::error("Fehler bei der Avatar-Transformation: {}", e.what());
    }
}

void VoiceAI::setEmotionParameters(const EmotionParameters& params) {
    pImpl->emotionParams = params;
}

void VoiceAI::processEmotion(float* buffer, int numSamples) {
    if (pImpl->activeActor.empty()) return;

    try {
        // Erstelle Input-Tensor für Emotion
        std::vector<torch::jit::IValue> inputs;
        auto emotionTensor = torch::tensor({
            pImpl->emotionParams.happiness,
            pImpl->emotionParams.sadness,
            pImpl->emotionParams.anger,
            pImpl->emotionParams.fear,
            pImpl->emotionParams.surprise,
            pImpl->emotionParams.neutral
        });
        inputs.push_back(emotionTensor);

        // Führe Emotion-Transformation durch
        auto output = pImpl->voiceModel.forward(inputs).toTensor();
        
        // Kopiere Ergebnis zurück
        std::memcpy(buffer, output.data_ptr<float>(), numSamples * sizeof(float));

        if (pImpl->emotionCallback) {
            pImpl->emotionCallback(pImpl->emotionParams);
        }
    } catch (const c10::Error& e) {
        spdlog::error("Fehler bei der Emotion-Transformation: {}", e.what());
    }
}

void VoiceAI::setVoiceCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->voiceCallback = callback;
}

void VoiceAI::setAvatarCallback(std::function<void(const glm::mat4&)> callback) {
    pImpl->avatarCallback = callback;
}

void VoiceAI::setEmotionCallback(std::function<void(const EmotionParameters&)> callback) {
    pImpl->emotionCallback = callback;
}

} // namespace VRMusicStudio 