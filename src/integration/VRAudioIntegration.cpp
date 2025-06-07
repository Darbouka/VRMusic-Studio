#include "VRAudioIntegration.hpp"
#include <stdexcept>

namespace VR_DAW {

VRAudioIntegration& VRAudioIntegration::getInstance() {
    static VRAudioIntegration instance;
    return instance;
}

VRAudioIntegration::VRAudioIntegration()
    : initialized(false)
    , logger("VRAudioIntegration")
    , vrManager(VRManager::getInstance())
    , audioEngine(AudioEngine::getInstance())
    , roomSize(10.0f, 10.0f, 10.0f)
    , roomReflection(0.5f)
    , roomAbsorption(0.5f) {
}

VRAudioIntegration::~VRAudioIntegration() {
    shutdown();
}

bool VRAudioIntegration::initialize() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (initialized) {
        return true;
    }

    try {
        if (!vrManager.isInitialized()) {
            logger.error("VR-Manager nicht initialisiert");
            return false;
        }

        if (!audioEngine.isInitialized()) {
            logger.error("Audio-Engine nicht initialisiert");
            return false;
        }

        initialized = true;
        logger.info("VR-Audio-Integration erfolgreich initialisiert");
        return true;
    }
    catch (const std::exception& e) {
        logger.error("Fehler bei der VR-Audio-Integration-Initialisierung: {}", e.what());
        return false;
    }
}

void VRAudioIntegration::shutdown() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        // Controller-Audio beenden
        for (auto& [controllerIndex, audio] : controllerAudio) {
            stopControllerFeedback(controllerIndex);
        }
        controllerAudio.clear();

        // Audio-Quellen zurücksetzen
        audioSources.clear();

        initialized = false;
        logger.info("VR-Audio-Integration erfolgreich beendet");
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Beenden der VR-Audio-Integration: {}", e.what());
    }
}

bool VRAudioIntegration::isInitialized() const {
    std::lock_guard<std::mutex> lock(mutex);
    return initialized;
}

void VRAudioIntegration::update() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        updateListenerPosition();
        updateAudioSources();
        processControllerAudio();
        updateRoomAudio();
        updateVisualization();
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim VR-Audio-Update: {}", e.what());
    }
}

void VRAudioIntegration::setAudioSourcePosition(const std::string& sourceId, const glm::vec3& position) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        auto& source = audioSources[sourceId];
        source.position = position;
        audioEngine.setSourcePosition(sourceId, position);
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Setzen der Audio-Quellen-Position: {}", e.what());
    }
}

void VRAudioIntegration::setAudioSourceOrientation(const std::string& sourceId, const glm::quat& orientation) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        auto& source = audioSources[sourceId];
        source.orientation = orientation;
        audioEngine.setSourceOrientation(sourceId, orientation);
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Setzen der Audio-Quellen-Orientierung: {}", e.what());
    }
}

void VRAudioIntegration::setAudioSourceVolume(const std::string& sourceId, float volume) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        auto& source = audioSources[sourceId];
        source.volume = volume;
        // Volume an Audio-Engine weiterleiten
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Setzen der Audio-Quellen-Lautstärke: {}", e.what());
    }
}

void VRAudioIntegration::setAudioSourceMute(const std::string& sourceId, bool mute) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        auto& source = audioSources[sourceId];
        source.isMuted = mute;
        // Mute-Status an Audio-Engine weiterleiten
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Setzen des Audio-Quellen-Mute-Status: {}", e.what());
    }
}

void VRAudioIntegration::playControllerFeedback(vr::TrackedDeviceIndex_t controllerIndex, float intensity) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        auto& audio = controllerAudio[controllerIndex];
        audio.isPlaying = true;
        audio.intensity = intensity;
        audio.frequency = 440.0f * intensity; // Basis-Frequenz anpassen
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Starten des Controller-Feedbacks: {}", e.what());
    }
}

void VRAudioIntegration::stopControllerFeedback(vr::TrackedDeviceIndex_t controllerIndex) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        auto it = controllerAudio.find(controllerIndex);
        if (it != controllerAudio.end()) {
            it->second.isPlaying = false;
        }
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Stoppen des Controller-Feedbacks: {}", e.what());
    }
}

void VRAudioIntegration::setRoomSize(const glm::vec3& size) {
    std::lock_guard<std::mutex> lock(mutex);
    roomSize = size;
}

void VRAudioIntegration::setRoomReflection(float reflection) {
    std::lock_guard<std::mutex> lock(mutex);
    roomReflection = reflection;
}

void VRAudioIntegration::setRoomAbsorption(float absorption) {
    std::lock_guard<std::mutex> lock(mutex);
    roomAbsorption = absorption;
}

void VRAudioIntegration::updateAudioVisualization() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        updateVisualization();
    }
    catch (const std::exception& e) {
        logger.error("Fehler bei der Audio-Visualisierung: {}", e.what());
    }
}

std::vector<float> VRAudioIntegration::getAudioSpectrum(const std::string& sourceId) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = audioSources.find(sourceId);
    if (it != audioSources.end()) {
        return it->second.spectrum;
    }
    return std::vector<float>();
}

std::vector<float> VRAudioIntegration::getAudioWaveform(const std::string& sourceId) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = audioSources.find(sourceId);
    if (it != audioSources.end()) {
        return it->second.waveform;
    }
    return std::vector<float>();
}

void VRAudioIntegration::updateListenerPosition() {
    auto hmd = vrManager.getHMD();
    if (hmd.isTracking) {
        audioEngine.setListenerPosition(hmd.position);
        audioEngine.setListenerOrientation(hmd.rotation);
    }
}

void VRAudioIntegration::updateAudioSources() {
    for (auto& [sourceId, source] : audioSources) {
        if (!source.isMuted) {
            // Audio-Quellen-Position und -Orientierung aktualisieren
            // z.B. basierend auf VR-Controller-Positionen oder anderen VR-Elementen
        }
    }
}

void VRAudioIntegration::processControllerAudio() {
    for (auto& [controllerIndex, audio] : controllerAudio) {
        if (audio.isPlaying) {
            // Controller-Feedback-Audio generieren
            // z.B. Sinus-Welle mit angepasster Frequenz und Intensität
        }
    }
}

void VRAudioIntegration::updateRoomAudio() {
    // Raum-Audio-Parameter an Audio-Engine weiterleiten
    // z.B. Reverb, Echo, etc.
}

void VRAudioIntegration::updateVisualization() {
    for (auto& [sourceId, source] : audioSources) {
        // FFT für Spektrum berechnen
        // Waveform aus Audio-Buffer extrahieren
    }
}

} // namespace VR_DAW 