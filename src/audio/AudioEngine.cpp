#include "AudioEngine.hpp"
#include "core/Logger.hpp"
#include <stdexcept>
#include <fstream>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cmath>
#include <spdlog/spdlog.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <portaudio.h>

namespace VRMusicStudio {
namespace Audio {

AudioEngine& AudioEngine::getInstance() {
    static AudioEngine instance;
    return instance;
}

AudioEngine::AudioEngine()
    : isInitialized(false)
    , isPlaying(false)
    , currentPosition(0.0)
    , stream(nullptr)
    , sampleRate(44100.0)
    , bufferSize(1024)
    , inputDevice(-1)
    , outputDevice(-1)
    , paStream(nullptr)
    , jackClient(nullptr)
    , jackInputPort(nullptr)
    , jackOutputPort(nullptr) {
    initialize();
}

AudioEngine::~AudioEngine() {
    shutdown();
}

bool AudioEngine::initialize() {
    if (isInitialized) return true;

    PaError err = Pa_Initialize();
    if (err != paNoError) {
        throw std::runtime_error("Failed to initialize PortAudio: " + std::string(Pa_GetErrorText(err)));
    }

    // Audio-Stream konfigurieren
    PaStreamParameters inputParameters;
    PaStreamParameters outputParameters;
    
    inputParameters.device = (inputDevice >= 0) ? inputDevice : Pa_GetDefaultInputDevice();
    inputParameters.channelCount = 2;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    
    outputParameters.device = (outputDevice >= 0) ? outputDevice : Pa_GetDefaultOutputDevice();
    outputParameters.channelCount = 2;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;

    // Stream öffnen
    err = Pa_OpenStream(&stream,
                       &inputParameters,
                       &outputParameters,
                       sampleRate,
                       bufferSize,
                       paClipOff | paDitherOff,
                       nullptr,
                       nullptr);

    if (err != paNoError) {
        throw std::runtime_error("Failed to open audio stream: " + std::string(Pa_GetErrorText(err)));
    }

    isInitialized = true;
    return true;
}

void AudioEngine::shutdown() {
    if (!isInitialized) return;

    if (isPlaying) {
        stop();
    }

    PaError err = Pa_CloseStream(stream);
    if (err != paNoError) {
        throw std::runtime_error("Failed to close audio stream: " + std::string(Pa_GetErrorText(err)));
    }

    err = Pa_Terminate();
    if (err != paNoError) {
        throw std::runtime_error("Failed to terminate PortAudio: " + std::string(Pa_GetErrorText(err)));
    }

    isInitialized = false;
}

void AudioEngine::start() {
    if (!isInitialized || isPlaying) return;

    PaError err = Pa_StartStream(stream);
    if (err != paNoError) {
        throw std::runtime_error("Failed to start audio stream: " + std::string(Pa_GetErrorText(err)));
    }

    isPlaying = true;
}

void AudioEngine::stop() {
    if (!isInitialized || !isPlaying) return;

    PaError err = Pa_StopStream(stream);
    if (err != paNoError) {
        throw std::runtime_error("Failed to stop audio stream: " + std::string(Pa_GetErrorText(err)));
    }

    isPlaying = false;
}

void AudioEngine::processAudio(float* inputBuffer, float* outputBuffer, unsigned long framesPerBuffer) {
    if (!isInitialized) return;

    // Audio-Verarbeitung für alle Tracks
    for (auto& track : audioTracks) {
        if (!track.isMuted && (track.isSolo || !anyTrackSolo())) {
            // Track-Audio verarbeiten
            processTrack(track, inputBuffer, outputBuffer, framesPerBuffer);
        }
    }

    // MIDI-Tracks verarbeiten
    for (auto& track : midiTracks) {
        if (!track.isMuted && (track.isSolo || !anyTrackSolo())) {
            // MIDI-Daten verarbeiten
            processMidiTrack(track, framesPerBuffer);
        }
    }

    // VR-Positionierung und 3D-Audio
    processVRAudio(outputBuffer, framesPerBuffer);

    // Automation
    processAutomation(framesPerBuffer);

    // Effekte
    processEffects(outputBuffer, framesPerBuffer);

    // Mixing
    processMixing(outputBuffer, framesPerBuffer);
}

void AudioEngine::processTrack(const AudioTrack& track, float* inputBuffer, float* outputBuffer, unsigned long framesPerBuffer) {
    // Track-Audio mit Plugins verarbeiten
    for (auto& plugin : track.plugins) {
        if (!plugin->isBypassed()) {
            plugin->processAudio(outputBuffer, framesPerBuffer);
        }
    }
}

void AudioEngine::processMidiTrack(const MidiTrack& track, unsigned long framesPerBuffer) {
    // MIDI-Daten verarbeiten und an Plugins weiterleiten
    for (auto& plugin : track.plugins) {
        if (!plugin->isBypassed()) {
            plugin->processMidi(track.data);
        }
    }
}

void AudioEngine::processVRAudio(float* outputBuffer, unsigned long framesPerBuffer) {
    // 3D-Audio basierend auf VR-Positionierung
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        // HRTF oder Ambisonics für 3D-Audio
        float left = outputBuffer[i];
        float right = outputBuffer[i + 1];
        
        // VR-Positionierung anwenden
        glm::vec3 listenerPosition = getListenerPosition();
        glm::vec3 sourcePosition = getSourcePosition();
        float distance = glm::distance(listenerPosition, sourcePosition);
        float attenuation = 1.0f / (1.0f + distance);
        
        outputBuffer[i] = left * attenuation;
        outputBuffer[i + 1] = right * attenuation;
    }
}

void AudioEngine::processAutomation(unsigned long framesPerBuffer) {
    // Automation-Punkte verarbeiten
    for (auto& [parameter, points] : automationPoints) {
        float currentTime = currentPosition;
        float value = interpolateAutomationValue(points, currentTime);
        setParameter(parameter, value);
    }
}

void AudioEngine::processEffects(float* outputBuffer, unsigned long framesPerBuffer) {
    // Effekte auf den Mix anwenden
    for (auto& effect : effects) {
        if (!effect->isBypassed()) {
            effect->processAudio(outputBuffer, framesPerBuffer);
        }
    }
}

void AudioEngine::processMixing(float* outputBuffer, unsigned long framesPerBuffer) {
    // Finales Mixing mit Volume, Pan, etc.
    for (unsigned long i = 0; i < framesPerBuffer; i += 2) {
        float left = outputBuffer[i];
        float right = outputBuffer[i + 1];
        
        // Volume
        left *= masterVolume;
        right *= masterVolume;
        
        // Pan
        float pan = masterPan;
        float leftGain = std::cos(pan * M_PI / 2.0f);
        float rightGain = std::sin(pan * M_PI / 2.0f);
        
        outputBuffer[i] = left * leftGain;
        outputBuffer[i + 1] = right * rightGain;
    }
}

bool AudioEngine::anyTrackSolo() const {
    return std::any_of(audioTracks.begin(), audioTracks.end(),
                      [](const AudioTrack& track) { return track.isSolo; });
}

float AudioEngine::interpolateAutomationValue(const std::vector<AutomationPoint>& points, float time) {
    if (points.empty()) return 0.0f;
    if (points.size() == 1) return points[0].value;
    
    // Finde die umgebenden Punkte
    auto it = std::lower_bound(points.begin(), points.end(), time,
                              [](const AutomationPoint& point, float t) {
                                  return point.time < t;
                              });
    
    if (it == points.begin()) return points.front().value;
    if (it == points.end()) return points.back().value;
    
    // Lineare Interpolation
    const AutomationPoint& p1 = *(it - 1);
    const AutomationPoint& p2 = *it;
    float t = (time - p1.time) / (p2.time - p1.time);
    return p1.value + t * (p2.value - p1.value);
}

glm::vec3 AudioEngine::getListenerPosition() const {
    // VR-Listener-Position aus der VR-Engine holen
    return glm::vec3(0.0f); // Dummy
}

glm::vec3 AudioEngine::getSourcePosition() const {
    // Audio-Source-Position aus der VR-Engine holen
    return glm::vec3(0.0f); // Dummy
}

void AudioEngine::update() {
    std::lock_guard<std::mutex> lock(mutex);

    if (!isInitialized || !isPlaying) {
        return;
    }

    try {
        // Audio-Update-Logik hier implementieren
    } catch (const std::exception& e) {
        spdlog::error("Fehler im Audio-Update: {}", e.what());
    }
}

bool AudioEngine::isStreamActive() const {
    std::lock_guard<std::mutex> lock(mutex);
    return isPlaying;
}

void AudioEngine::setSampleRate(double newSampleRate) {
    std::lock_guard<std::mutex> lock(mutex);
    sampleRate = newSampleRate;
}

double AudioEngine::getSampleRate() const {
    std::lock_guard<std::mutex> lock(mutex);
    return sampleRate;
}

void AudioEngine::setBufferSize(unsigned int newBufferSize) {
    std::lock_guard<std::mutex> lock(mutex);
    bufferSize = newBufferSize;
}

unsigned int AudioEngine::getBufferSize() const {
    std::lock_guard<std::mutex> lock(mutex);
    return bufferSize;
}

void AudioEngine::setInputDevice(int deviceId) {
    std::lock_guard<std::mutex> lock(mutex);
    inputDevice = deviceId;
}

void AudioEngine::setOutputDevice(int deviceId) {
    std::lock_guard<std::mutex> lock(mutex);
    outputDevice = deviceId;
}

int AudioEngine::getInputDevice() const {
    std::lock_guard<std::mutex> lock(mutex);
    return inputDevice;
}

int AudioEngine::getOutputDevice() const {
    std::lock_guard<std::mutex> lock(mutex);
    return outputDevice;
}

std::vector<std::string> AudioEngine::getAvailableInputDevices() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> devices;

    int numDevices = Pa_GetDeviceCount();
    for (int i = 0; i < numDevices; i++) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->maxInputChannels > 0) {
            devices.push_back(deviceInfo->name);
        }
    }

    return devices;
}

std::vector<std::string> AudioEngine::getAvailableOutputDevices() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> devices;

    int numDevices = Pa_GetDeviceCount();
    for (int i = 0; i < numDevices; i++) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->maxOutputChannels > 0) {
            devices.push_back(deviceInfo->name);
        }
    }

    return devices;
}

int AudioEngine::paCallback(const void* inputBuffer, void* outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void* userData) {
    (void)timeInfo;
    (void)statusFlags;

    auto* engine = static_cast<AudioEngine*>(userData);
    if (!engine) {
        return paAbort;
    }

    // Audio-Verarbeitung hier implementieren
    const float* in = static_cast<const float*>(inputBuffer);
    float* out = static_cast<float*>(outputBuffer);

    for (unsigned long i = 0; i < framesPerBuffer; i++) {
        out[i] = in[i];  // Einfaches Pass-Through
    }

    return paContinue;
}

int AudioEngine::jackCallback(jack_nframes_t nframes, void* arg) {
    auto* engine = static_cast<AudioEngine*>(arg);
    if (!engine) {
        return 1;
    }

    // JACK-Audio-Verarbeitung hier implementieren
    return 0;
}

void AudioEngine::initializePortAudio() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        throw std::runtime_error("Fehler bei der PortAudio-Initialisierung: " + std::string(Pa_GetErrorText(err)));
    }

    PaStreamParameters inputParameters;
    PaStreamParameters outputParameters;

    inputParameters.device = (inputDevice >= 0) ? inputDevice : Pa_GetDefaultInputDevice();
    inputParameters.channelCount = 2;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    outputParameters.device = (outputDevice >= 0) ? outputDevice : Pa_GetDefaultOutputDevice();
    outputParameters.channelCount = 2;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(&paStream,
                       &inputParameters,
                       &outputParameters,
                       sampleRate,
                       bufferSize,
                       paClipOff,
                       paCallback,
                       this);

    if (err != paNoError) {
        throw std::runtime_error("Fehler beim Öffnen des PortAudio-Streams: " + std::string(Pa_GetErrorText(err)));
    }
}

void AudioEngine::initializeJACK() {
    jackClient = jack_client_open("VRMusicStudio", JackNoStartServer, nullptr);
    if (!jackClient) {
        throw std::runtime_error("Fehler beim Öffnen des JACK-Clients");
    }

    jack_set_process_callback(jackClient, jackCallback, this);

    jackInputPort = jack_port_register(jackClient, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    jackOutputPort = jack_port_register(jackClient, "output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    if (!jackInputPort || !jackOutputPort) {
        throw std::runtime_error("Fehler bei der JACK-Port-Registrierung");
    }

    if (jack_activate(jackClient) != 0) {
        throw std::runtime_error("Fehler bei der JACK-Client-Aktivierung");
    }
}

void AudioEngine::shutdownPortAudio() {
    if (paStream) {
        Pa_CloseStream(paStream);
        paStream = nullptr;
    }
    Pa_Terminate();
}

void AudioEngine::shutdownJACK() {
    if (jackClient) {
        jack_client_close(jackClient);
        jackClient = nullptr;
        jackInputPort = nullptr;
        jackOutputPort = nullptr;
    }
}

} // namespace Audio
} // namespace VRMusicStudio
