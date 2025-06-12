#include "VRAudio.hpp"
#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#include <algorithm>
#include <cmath>
#include <map>

namespace VRMusicStudio {

struct VRAudio::Impl {
    // PortAudio
    PaStream* mainStream = nullptr;
    bool paInitialized;
    
    // OpenAL
    ALCdevice* device;
    ALCcontext* context;
    bool alInitialized;
    
    // Audio-Quellen
    std::map<std::string, AudioSource> sources;
    std::map<std::string, ALuint> sourceBuffers;
    std::map<std::string, ALuint> sourceIds;
    
    // Listener
    AudioListener listener;
    bool is3DAudioEnabled = true;
    float audioDistance = 1.0f;
    float audioRolloff = 1.0f;
    
    // PortAudio-Konfiguration
    const int sampleRate = 44100;
    const int framesPerBuffer = 256;
    const int numChannels = 2;
    
    // Zusätzliche Strukturen für Effekte und Streaming
    struct Effect {
        std::string type;
        float parameter = 1.0f; // z.B. Verstärkungsfaktor
    };

    struct StreamingInfo {
        bool active = false;
        std::function<void(const std::vector<float>&)> callback;
    };

    std::map<std::string, std::vector<Effect>> effects;
    std::map<std::string, StreamingInfo> streaming;
    
    Impl() : paInitialized(false),
             device(nullptr),
             context(nullptr),
             alInitialized(false) {}
};

VRAudio::VRAudio() : pImpl(std::make_unique<Impl>()) {}

VRAudio::~VRAudio() {
    shutdown();
}

bool VRAudio::initialize() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        spdlog::error("Failed to initialize PortAudio: {}", Pa_GetErrorText(err));
        return false;
    }
    
    err = Pa_OpenDefaultStream(&pImpl->mainStream,
                             0,          // Keine Eingabekanäle
                             pImpl->numChannels,  // Stereo-Ausgabe
                             paFloat32,  // 32-bit Float Samples
                             pImpl->sampleRate,
                             pImpl->framesPerBuffer,
                             nullptr,    // Kein Callback
                             nullptr);   // Keine Callback-Daten
    
    if (err != paNoError) {
        spdlog::error("Failed to open audio stream: {}", Pa_GetErrorText(err));
        return false;
    }
    
    err = Pa_StartStream(pImpl->mainStream);
    if (err != paNoError) {
        spdlog::error("Failed to start audio stream: {}", Pa_GetErrorText(err));
        return false;
    }
    
    // Initialisiere Listener
    pImpl->listener.position = glm::vec3(0.0f);
    pImpl->listener.forward = glm::vec3(0.0f, 0.0f, -1.0f);
    pImpl->listener.up = glm::vec3(0.0f, 1.0f, 0.0f);
    pImpl->listener.volume = 1.0f;
    
    return true;
}

void VRAudio::shutdown() {
    if (pImpl->mainStream) {
        Pa_StopStream(pImpl->mainStream);
        Pa_CloseStream(pImpl->mainStream);
        pImpl->mainStream = nullptr;
    }
    
    for (auto& source : pImpl->sources) {
        if (source.second.stream) {
            Pa_StopStream(source.second.stream);
            Pa_CloseStream(source.second.stream);
        }
    }
    pImpl->sources.clear();
    
    Pa_Terminate();
    
    // OpenAL beenden
    if (pImpl->alInitialized) {
        // Quellen und Buffer freigeben
        for (const auto& [id, buffer] : pImpl->sourceBuffers) {
            alDeleteBuffers(1, &buffer);
        }
        for (const auto& [id, source] : pImpl->sourceIds) {
            alDeleteSources(1, &source);
        }
        
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(pImpl->context);
        alcCloseDevice(pImpl->device);
        pImpl->alInitialized = false;
    }
}

void VRAudio::update() {
    processAudio();
    update3DAudio();
}

std::string VRAudio::createSource(const std::string& filePath,
                                const glm::vec3& position,
                                float volume,
                                float pitch,
                                bool loop) {
    AudioSource source;
    source.id = filePath; // TODO: Generiere eindeutige ID
    source.filePath = filePath;
    source.position = position;
    source.volume = volume;
    source.pitch = pitch;
    source.loop = loop;
    source.playing = false;
    source.stream = nullptr;
    
    pImpl->sources[source.id] = source;
    return source.id;
}

void VRAudio::destroySource(const std::string& sourceId) {
    auto it = pImpl->sources.find(sourceId);
    if (it != pImpl->sources.end()) {
        if (it->second.stream) {
            Pa_StopStream(it->second.stream);
            Pa_CloseStream(it->second.stream);
        }
        pImpl->sources.erase(it);
    }
}

void VRAudio::playSource(const std::string& sourceId) {
    auto it = pImpl->sources.find(sourceId);
    if (it != pImpl->sources.end()) {
        it->second.playing = true;
        // TODO: Implementiere Audio-Wiedergabe
    }
}

void VRAudio::pauseSource(const std::string& sourceId) {
    auto it = pImpl->sources.find(sourceId);
    if (it != pImpl->sources.end()) {
        it->second.playing = false;
        // TODO: Implementiere Audio-Pause
    }
}

void VRAudio::stopSource(const std::string& sourceId) {
    auto it = pImpl->sources.find(sourceId);
    if (it != pImpl->sources.end()) {
        it->second.playing = false;
        // TODO: Implementiere Audio-Stop
    }
}

void VRAudio::setSourcePosition(const std::string& sourceId, const glm::vec3& position) {
    auto it = pImpl->sources.find(sourceId);
    if (it != pImpl->sources.end()) {
        it->second.position = position;
    }
}

void VRAudio::setSourceVolume(const std::string& sourceId, float volume) {
    auto it = pImpl->sources.find(sourceId);
    if (it != pImpl->sources.end()) {
        it->second.volume = std::clamp(volume, 0.0f, 1.0f);
    }
}

void VRAudio::setSourcePitch(const std::string& sourceId, float pitch) {
    auto it = pImpl->sources.find(sourceId);
    if (it != pImpl->sources.end()) {
        it->second.pitch = std::max(0.0f, pitch);
    }
}

void VRAudio::setSourceLoop(const std::string& sourceId, bool loop) {
    auto it = pImpl->sources.find(sourceId);
    if (it != pImpl->sources.end()) {
        it->second.loop = loop;
    }
}

bool VRAudio::isSourcePlaying(const std::string& sourceId) const {
    auto it = pImpl->sources.find(sourceId);
    return it != pImpl->sources.end() && it->second.playing;
}

float VRAudio::getSourceVolume(const std::string& sourceId) const {
    auto it = pImpl->sources.find(sourceId);
    return it != pImpl->sources.end() ? it->second.volume : 0.0f;
}

float VRAudio::getSourcePitch(const std::string& sourceId) const {
    auto it = pImpl->sources.find(sourceId);
    return it != pImpl->sources.end() ? it->second.pitch : 1.0f;
}

bool VRAudio::isSourceLooping(const std::string& sourceId) const {
    auto it = pImpl->sources.find(sourceId);
    return it != pImpl->sources.end() && it->second.loop;
}

void VRAudio::setListenerPosition(const glm::vec3& position) {
    pImpl->listener.position = position;
}

void VRAudio::setListenerOrientation(const glm::vec3& forward, const glm::vec3& up) {
    pImpl->listener.forward = glm::normalize(forward);
    pImpl->listener.up = glm::normalize(up);
}

void VRAudio::setListenerVolume(float volume) {
    pImpl->listener.volume = std::clamp(volume, 0.0f, 1.0f);
}

glm::vec3 VRAudio::getListenerPosition() const {
    return pImpl->listener.position;
}

float VRAudio::getListenerVolume() const {
    return pImpl->listener.volume;
}

void VRAudio::set3DAudioEnabled(bool enabled) {
    pImpl->is3DAudioEnabled = enabled;
}

bool VRAudio::is3DAudioEnabled() const {
    return pImpl->is3DAudioEnabled;
}

void VRAudio::set3DAudioDistance(float distance) {
    pImpl->audioDistance = std::max(0.0f, distance);
}

float VRAudio::get3DAudioDistance() const {
    return pImpl->audioDistance;
}

void VRAudio::set3DAudioRolloff(float rolloff) {
    pImpl->audioRolloff = std::max(0.0f, rolloff);
}

float VRAudio::get3DAudioRolloff() const {
    return pImpl->audioRolloff;
}

void VRAudio::addEffect(const std::string& sourceId, const std::string& effectType) {
    // TODO: Implementiere Effekt-Hinzufügung
}

void VRAudio::removeEffect(const std::string& sourceId, const std::string& effectType) {
    // TODO: Implementiere Effekt-Entfernung
}

void VRAudio::setEffectParameter(const std::string& sourceId,
                                const std::string& effectType,
                                const std::string& parameter,
                                float value) {
    // TODO: Implementiere Effekt-Parameter-Änderung
}

float VRAudio::getEffectParameter(const std::string& sourceId,
                                 const std::string& effectType,
                                 const std::string& parameter) const {
    // TODO: Implementiere Effekt-Parameter-Abfrage
    return 0.0f;
}

void VRAudio::startStreaming(const std::string& sourceId) {
    // TODO: Implementiere Streaming-Start
}

void VRAudio::stopStreaming(const std::string& sourceId) {
    // TODO: Implementiere Streaming-Stop
}

bool VRAudio::isStreaming(const std::string& sourceId) const {
    // TODO: Implementiere Streaming-Status
    return false;
}

void VRAudio::setStreamingCallback(const std::string& sourceId,
                                 std::function<void(const std::vector<float>&)> callback) {
    // TODO: Implementiere Streaming-Callback
}

void VRAudio::processAudio() {
    // TODO: Implementiere Audio-Verarbeitung
}

void VRAudio::update3DAudio() {
    if (!pImpl->is3DAudioEnabled) return;
    
    for (auto& source : pImpl->sources) {
        float volume = calculate3DVolume(source.second);
        source.second.volume = volume;
    }
}

void VRAudio::applyEffects(AudioSource& source) {
    // TODO: Implementiere Effekt-Anwendung
}

void VRAudio::handleStreaming(AudioSource& source) {
    // TODO: Implementiere Streaming-Handling
}

float VRAudio::calculate3DVolume(const AudioSource& source) const {
    if (!pImpl->is3DAudioEnabled) return source.volume;
    
    float distance = glm::length(source.position - pImpl->listener.position);
    float volume = source.volume * pImpl->listener.volume;
    
    if (distance > pImpl->audioDistance) {
        volume *= std::pow(pImpl->audioDistance / distance, pImpl->audioRolloff);
    }
    
    return std::clamp(volume, 0.0f, 1.0f);
}

glm::vec3 VRAudio::calculate3DPosition(const AudioSource& source) const {
    if (!pImpl->is3DAudioEnabled) return source.position;
    
    // TODO: Implementiere 3D-Positionsberechnung
    return source.position;
}

} // namespace VRMusicStudio 