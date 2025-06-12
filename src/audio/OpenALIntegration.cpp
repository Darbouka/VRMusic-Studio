#include "OpenALIntegration.hpp"
#include <stdexcept>
#include <fstream>
#include <sndfile.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
<<<<<<< HEAD
=======
#include <AL/efx.h>
>>>>>>> 0dff1c4 (init 2)

namespace VR_DAW {

OpenALIntegration& OpenALIntegration::getInstance() {
    static OpenALIntegration instance;
    return instance;
}

OpenALIntegration::OpenALIntegration()
<<<<<<< HEAD
    : initialized(false)
    , device(nullptr)
=======
    : device(nullptr)
>>>>>>> 0dff1c4 (init 2)
    , context(nullptr)
    , reverbEffect(0)
    , echoEffect(0)
    , distortionEffect(0)
    , roomSize(10.0f)
    , roomReflection(0.5f)
    , roomAbsorption(0.5f)
<<<<<<< HEAD
    , logger("OpenALIntegration") {
    
=======
    , logger("OpenALIntegration")
    , initialized(false)
{
>>>>>>> 0dff1c4 (init 2)
    listener.position = glm::vec3(0.0f);
    listener.orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    listener.volume = 1.0f;
}

OpenALIntegration::~OpenALIntegration() {
    shutdown();
}

bool OpenALIntegration::initialize() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (initialized) {
        return true;
    }

    try {
        if (!initializeOpenAL()) {
            logger.error("OpenAL konnte nicht initialisiert werden");
            return false;
        }

        // Standard-Reverb-Eigenschaften setzen
        reverbProperties = {
            {"density", 1.0f},
            {"diffusion", 1.0f},
            {"gain", 0.32f},
            {"gain_hf", 0.89f},
            {"decay_time", 1.49f},
            {"decay_hf_ratio", 0.83f},
            {"reflections_gain", 0.05f},
            {"reflections_delay", 0.007f},
            {"late_reverb_gain", 1.26f},
            {"late_reverb_delay", 0.011f},
            {"air_absorption_gain_hf", 0.994f},
            {"room_rolloff_factor", 0.0f}
        };

        // Standard-Okklusion-Eigenschaften setzen
        occlusionProperties = {
            {"low_pass_gain", 0.25f},
            {"high_pass_gain", 1.0f},
            {"occlusion_factor", 0.5f}
        };

        initialized = true;
        logger.info("OpenAL-Integration erfolgreich initialisiert");
        return true;
    }
    catch (const std::exception& e) {
        logger.error("Fehler bei der OpenAL-Integration-Initialisierung: {}", e.what());
        return false;
    }
}

void OpenALIntegration::shutdown() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        // Audio-Quellen beenden
        for (const auto& [name, source] : sources) {
            alDeleteSources(1, &source->source);
            alDeleteBuffers(1, &source->buffer);
        }
        sources.clear();

        // Effekte beenden
        if (reverbEffect) alDeleteEffects(1, &reverbEffect);
        if (echoEffect) alDeleteEffects(1, &echoEffect);
        if (distortionEffect) alDeleteEffects(1, &distortionEffect);

        cleanupOpenAL();
        initialized = false;
        logger.info("OpenAL-Integration erfolgreich beendet");
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Beenden der OpenAL-Integration: {}", e.what());
    }
}

std::shared_ptr<AudioSource> OpenALIntegration::createSource(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return nullptr;
    }

    try {
        auto source = std::make_shared<AudioSource>();
        alGenSources(1, &source->source);
        
        if (!checkALError("Source-Generierung")) {
            return nullptr;
        }

        source->position = glm::vec3(0.0f);
        source->orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        source->volume = 1.0f;
        source->pitch = 1.0f;
        source->isLooping = false;
        source->is3D = true;

        sources[name] = source;
        logger.info("Audio-Quelle erstellt: {}", name);
        return source;
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Erstellen der Audio-Quelle: {}", e.what());
        return nullptr;
    }
}

bool OpenALIntegration::loadAudioFile(const std::string& filePath, ALuint& buffer) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return false;
    }

    try {
        SF_INFO fileInfo;
        SNDFILE* file = sf_open(filePath.c_str(), SFM_READ, &fileInfo);
        if (!file) {
            logger.error("Audio-Datei konnte nicht geöffnet werden: {}", filePath);
            return false;
        }

        std::vector<float> audioData(fileInfo.frames * fileInfo.channels);
        sf_readf_float(file, audioData.data(), fileInfo.frames);
        sf_close(file);

        // In Mono konvertieren, falls nötig
        std::vector<float> monoData;
        if (fileInfo.channels > 1) {
            convertToMono(audioData, monoData);
        } else {
            monoData = audioData;
        }

        alGenBuffers(1, &buffer);
        alBufferData(buffer, AL_FORMAT_MONO_FLOAT32, monoData.data(), 
                    monoData.size() * sizeof(float), fileInfo.samplerate);

        if (!checkALError("Buffer-Daten-Laden")) {
            alDeleteBuffers(1, &buffer);
            return false;
        }

        logger.info("Audio-Datei geladen: {}", filePath);
        return true;
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Laden der Audio-Datei: {}", e.what());
        return false;
    }
}

void OpenALIntegration::setSourcePosition(const std::string& sourceName, 
                                        const glm::vec3& position) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        auto it = sources.find(sourceName);
        if (it != sources.end()) {
            it->second->position = position;
            alSource3f(it->second->source, AL_POSITION, 
                      position.x, position.y, position.z);
            checkALError("Setzen der Quellenposition");
        }
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Setzen der Quellenposition: {}", e.what());
    }
}

void OpenALIntegration::setListenerPosition(const glm::vec3& position) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        listener.position = position;
        alListener3f(AL_POSITION, position.x, position.y, position.z);
        checkALError("Setzen der Listener-Position");
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Setzen der Listener-Position: {}", e.what());
    }
}

bool OpenALIntegration::playSource(const std::string& sourceName) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return false;
    }

    try {
        auto it = sources.find(sourceName);
        if (it != sources.end()) {
            alSourcePlay(it->second->source);
            if (!checkALError("Starten der Audio-Quelle")) {
                return false;
            }
            logger.info("Audio-Quelle gestartet: {}", sourceName);
            return true;
        }
        return false;
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Starten der Audio-Quelle: {}", e.what());
        return false;
    }
}

bool OpenALIntegration::applyEffect(const std::string& sourceName, 
                                  const std::string& effectName,
                                  const std::map<std::string, float>& parameters) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return false;
    }

    try {
        auto it = sources.find(sourceName);
        if (it == sources.end()) {
            return false;
        }

        auto& source = it->second;
        source->effects[effectName] = parameters.at("intensity");

        // Effekt-spezifische Parameter anwenden
        if (effectName == "reverb") {
            alSourcei(source->source, AL_EFFECT, reverbEffect);
            // Reverb-Parameter setzen
        }
        else if (effectName == "echo") {
            alSourcei(source->source, AL_EFFECT, echoEffect);
            // Echo-Parameter setzen
        }
        else if (effectName == "distortion") {
            alSourcei(source->source, AL_EFFECT, distortionEffect);
            // Distortion-Parameter setzen
        }

        checkALError("Anwenden des Effekts");
        logger.info("Effekt angewendet: {} auf {}", effectName, sourceName);
        return true;
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Anwenden des Effekts: {}", e.what());
        return false;
    }
}

void OpenALIntegration::setRoomProperties(float size, float reflection, float absorption) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        roomSize = size;
        roomReflection = reflection;
        roomAbsorption = absorption;

        // Raum-Audio-Parameter aktualisieren
        for (const auto& [name, source] : sources) {
            apply3DAudioEffects(name);
        }
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Setzen der Raum-Eigenschaften: {}", e.what());
    }
}

bool OpenALIntegration::initializeOpenAL() {
    device = alcOpenDevice(nullptr);
    if (!device) {
        logger.error("OpenAL-Gerät konnte nicht geöffnet werden");
        return false;
    }

    context = alcCreateContext(device, nullptr);
    if (!context) {
        logger.error("OpenAL-Kontext konnte nicht erstellt werden");
        alcCloseDevice(device);
        return false;
    }

    if (!alcMakeContextCurrent(context)) {
        logger.error("OpenAL-Kontext konnte nicht aktiviert werden");
        alcDestroyContext(context);
        alcCloseDevice(device);
        return false;
    }

    // Effekte initialisieren
    alGenEffects(1, &reverbEffect);
    alGenEffects(1, &echoEffect);
    alGenEffects(1, &distortionEffect);

    return true;
}

void OpenALIntegration::cleanupOpenAL() {
    if (context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
    }
    if (device) {
        alcCloseDevice(device);
    }
}

bool OpenALIntegration::checkALError(const std::string& operation) {
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        logger.error("OpenAL-Fehler bei {}: {}", operation, alGetString(error));
        return false;
    }
    return true;
}

void OpenALIntegration::apply3DAudioEffects(const std::string& sourceName) {
    auto it = sources.find(sourceName);
    if (it == sources.end()) {
        return;
    }

    auto& source = it->second;
    if (!source->is3D) {
        return;
    }

    // Entfernung berechnen
    float distance = glm::length(source->position - listener.position);
    
    // Lautstärke basierend auf Entfernung anpassen
    float volume = source->volume * (1.0f / (1.0f + distance * roomAbsorption));
    alSourcef(source->source, AL_GAIN, volume);

    // Raum-Effekte anwenden
    if (distance < roomSize) {
        float reverbIntensity = (1.0f - distance / roomSize) * roomReflection;
        source->effects["reverb"] = reverbIntensity;
    }
}

bool OpenALIntegration::convertToMono(const std::vector<float>& input, 
                                    std::vector<float>& output) {
    if (input.empty()) {
        return false;
    }

    size_t numChannels = 2;  // Annahme: Stereo-Input
    size_t numFrames = input.size() / numChannels;
    output.resize(numFrames);

    for (size_t i = 0; i < numFrames; ++i) {
        float sum = 0.0f;
        for (size_t ch = 0; ch < numChannels; ++ch) {
            sum += input[i * numChannels + ch];
        }
        output[i] = sum / numChannels;
    }

    return true;
}

} // namespace VR_DAW 