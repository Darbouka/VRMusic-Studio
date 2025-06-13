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
    : m_isInitialized(false)
    , m_sampleRate(44100)
    , m_blockSize(512)
    , m_numChannels(2)
    , m_audioStream(nullptr)
    , m_isPlaying(false)
    , m_currentPosition(0.0)
    , m_stream(nullptr)
    , m_inputDevice(-1)
    , m_outputDevice(-1)
    , m_paStream(nullptr)
    , m_jackClient(nullptr)
    , m_jackInputPort(nullptr)
    , m_jackOutputPort(nullptr) {
    initialize();
}

AudioEngine::~AudioEngine() {
    shutdown();
}

bool AudioEngine::initialize() {
    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Initialisiere Audio-Engine...");

        // Audio-Stream initialisieren
        m_audioStream = std::make_unique<AudioStream>();
        if (!m_audioStream->initialize()) {
            logger.error("Fehler bei der Audio-Stream-Initialisierung");
            return false;
        }

        // Audio-Parameter setzen
        m_audioStream->setSampleRate(m_sampleRate);
        m_audioStream->setBlockSize(m_blockSize);
        m_audioStream->setChannelCount(m_numChannels);

        // Audio-Stream starten
        if (!m_audioStream->start()) {
            logger.error("Fehler beim Starten des Audio-Streams");
            return false;
        }

        m_isInitialized = true;
        logger.info("Audio-Engine erfolgreich initialisiert");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Audio-Engine-Initialisierung: {}", e.what());
        return false;
    }
}

void AudioEngine::shutdown() {
    if (!m_isInitialized) {
        return;
    }

    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Beende Audio-Engine...");

        if (m_audioStream) {
            m_audioStream->stop();
            m_audioStream->shutdown();
            m_audioStream.reset();
        }

        m_isInitialized = false;
        logger.info("Audio-Engine erfolgreich beendet");
    } catch (const std::exception& e) {
        spdlog::error("Fehler beim Beenden der Audio-Engine: {}", e.what());
    }
}

void AudioEngine::start() {
    if (!m_isInitialized || m_isPlaying) return;

    PaError err = Pa_StartStream(m_stream);
    if (err != paNoError) {
        throw std::runtime_error("Failed to start audio stream: " + std::string(Pa_GetErrorText(err)));
    }

    m_isPlaying = true;
}

void AudioEngine::stop() {
    if (!m_isInitialized || !m_isPlaying) return;

    PaError err = Pa_StopStream(m_stream);
    if (err != paNoError) {
        throw std::runtime_error("Failed to stop audio stream: " + std::string(Pa_GetErrorText(err)));
    }

    m_isPlaying = false;
}

void AudioEngine::processAudio(float* inputBuffer, float* outputBuffer, unsigned long framesPerBuffer) {
    if (!m_isInitialized) return;

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
        float currentTime = m_currentPosition;
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
    if (!m_isInitialized || !m_audioStream) {
        return;
    }

    // Audio-Buffer verarbeiten
    AudioBuffer buffer(m_numChannels, m_blockSize);
    m_audioStream->process(buffer);

    // Audio-Effekte anwenden
    for (auto& effect : m_effects) {
        effect->process(buffer);
    }

    // Audio an Output weiterleiten
    m_audioStream->write(buffer);
}

bool AudioEngine::isStreamActive() const {
    std::lock_guard<std::mutex> lock(mutex);
    return m_isPlaying;
}

void AudioEngine::setSampleRate(int sampleRate) {
    m_sampleRate = sampleRate;
    if (m_audioStream) {
        m_audioStream->setSampleRate(sampleRate);
    }
}

int AudioEngine::getSampleRate() const {
    return m_sampleRate;
}

void AudioEngine::setBlockSize(int blockSize) {
    m_blockSize = blockSize;
    if (m_audioStream) {
        m_audioStream->setBlockSize(blockSize);
    }
}

int AudioEngine::getBlockSize() const {
    return m_blockSize;
}

void AudioEngine::setChannelCount(int numChannels) {
    m_numChannels = numChannels;
    if (m_audioStream) {
        m_audioStream->setChannelCount(numChannels);
    }
}

int AudioEngine::getChannelCount() const {
    return m_numChannels;
}

void AudioEngine::setInputDevice(int deviceId) {
    std::lock_guard<std::mutex> lock(mutex);
    m_inputDevice = deviceId;
}

void AudioEngine::setOutputDevice(int deviceId) {
    std::lock_guard<std::mutex> lock(mutex);
    m_outputDevice = deviceId;
}

int AudioEngine::getInputDevice() const {
    std::lock_guard<std::mutex> lock(mutex);
    return m_inputDevice;
}

int AudioEngine::getOutputDevice() const {
    std::lock_guard<std::mutex> lock(mutex);
    return m_outputDevice;
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

    inputParameters.device = (m_inputDevice >= 0) ? m_inputDevice : Pa_GetDefaultInputDevice();
    inputParameters.channelCount = 2;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    outputParameters.device = (m_outputDevice >= 0) ? m_outputDevice : Pa_GetDefaultOutputDevice();
    outputParameters.channelCount = 2;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(&m_paStream,
                       &inputParameters,
                       &outputParameters,
                       m_sampleRate,
                       m_blockSize,
                       paClipOff,
                       paCallback,
                       this);

    if (err != paNoError) {
        throw std::runtime_error("Fehler beim Öffnen des PortAudio-Streams: " + std::string(Pa_GetErrorText(err)));
    }
}

void AudioEngine::initializeJACK() {
    m_jackClient = jack_client_open("VRMusicStudio", JackNoStartServer, nullptr);
    if (!m_jackClient) {
        throw std::runtime_error("Fehler beim Öffnen des JACK-Clients");
    }

    jack_set_process_callback(m_jackClient, jackCallback, this);

    m_jackInputPort = jack_port_register(m_jackClient, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    m_jackOutputPort = jack_port_register(m_jackClient, "output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    if (!m_jackInputPort || !m_jackOutputPort) {
        throw std::runtime_error("Fehler bei der JACK-Port-Registrierung");
    }

    if (jack_activate(m_jackClient) != 0) {
        throw std::runtime_error("Fehler bei der JACK-Client-Aktivierung");
    }
}

void AudioEngine::shutdownPortAudio() {
    if (m_paStream) {
        Pa_CloseStream(m_paStream);
        m_paStream = nullptr;
    }
    Pa_Terminate();
}

void AudioEngine::shutdownJACK() {
    if (m_jackClient) {
        jack_client_close(m_jackClient);
        m_jackClient = nullptr;
        m_jackInputPort = nullptr;
        m_jackOutputPort = nullptr;
    }
}

void AudioEngine::addEffect(std::shared_ptr<AudioEffect> effect) {
    if (effect) {
        effects.push_back(effect);
    }
}

void AudioEngine::removeEffect(std::shared_ptr<AudioEffect> effect) {
    auto it = std::find(effects.begin(), effects.end(), effect);
    if (it != effects.end()) {
        effects.erase(it);
    }
}

} // namespace Audio
} // namespace VRMusicStudio
