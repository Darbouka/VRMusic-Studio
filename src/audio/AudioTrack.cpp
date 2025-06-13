#include "AudioTrack.hpp"
#include <sndfile.h>
#include <spdlog/spdlog.h>
#include <random>
#include <sstream>
#include <iomanip>

namespace VRMusicStudio {

AudioTrack::AudioTrack(const std::string& name)
    : name(name)
    , position(0.0)
    , recording(false)
    , playing(false)
    , volume(1.0f)
    , muted(false)
    , pan(0.0f)
{
    generateId();
}

AudioTrack::~AudioTrack() {
    stopRecording();
    stopPlayback();
}

void AudioTrack::generateId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    const char* hex = "0123456789abcdef";
    
    std::stringstream ss;
    for (int i = 0; i < 32; ++i) {
        ss << hex[dis(gen)];
    }
    id = ss.str();
}

bool AudioTrack::loadAudioFile(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    SF_INFO fileInfo;
    SNDFILE* file = sf_open(filePath.c_str(), SFM_READ, &fileInfo);
    if (!file) {
        spdlog::error("Konnte Audio-Datei nicht öffnen: {}", filePath);
        return false;
    }

    // Audio-Daten einlesen
    std::vector<float> buffer(fileInfo.frames * fileInfo.channels);
    sf_count_t framesRead = sf_readf_float(file, buffer.data(), fileInfo.frames);
    
    if (framesRead != fileInfo.frames) {
        spdlog::error("Fehler beim Lesen der Audio-Datei: {}", filePath);
        sf_close(file);
        return false;
    }

    // In Mono konvertieren, falls nötig
    if (fileInfo.channels == 2) {
        audioData.resize(fileInfo.frames);
        for (sf_count_t i = 0; i < fileInfo.frames; ++i) {
            audioData[i] = (buffer[i * 2] + buffer[i * 2 + 1]) * 0.5f;
        }
    } else {
        audioData = std::move(buffer);
    }

    sf_close(file);
    spdlog::info("Audio-Datei geladen: {}", filePath);
    return true;
}

bool AudioTrack::saveAudioFile(const std::string& filePath) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    SF_INFO fileInfo;
    fileInfo.samplerate = 44100;
    fileInfo.channels = 1;
    fileInfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

    SNDFILE* file = sf_open(filePath.c_str(), SFM_WRITE, &fileInfo);
    if (!file) {
        spdlog::error("Konnte Audio-Datei nicht erstellen: {}", filePath);
        return false;
    }

    sf_count_t framesWritten = sf_writef_float(file, audioData.data(), audioData.size());
    sf_close(file);

    if (framesWritten != static_cast<sf_count_t>(audioData.size())) {
        spdlog::error("Fehler beim Schreiben der Audio-Datei: {}", filePath);
        return false;
    }

    spdlog::info("Audio-Datei gespeichert: {}", filePath);
    return true;
}

void AudioTrack::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    audioData.clear();
    position = 0.0;
}

void AudioTrack::startRecording() {
    std::lock_guard<std::mutex> lock(mutex);
    if (!recording) {
        recording = true;
        position = 0.0;
        audioData.clear();
    }
}

void AudioTrack::stopRecording() {
    std::lock_guard<std::mutex> lock(mutex);
    recording = false;
}

void AudioTrack::startPlayback() {
    std::lock_guard<std::mutex> lock(mutex);
    if (!playing) {
        playing = true;
    }
}

void AudioTrack::stopPlayback() {
    std::lock_guard<std::mutex> lock(mutex);
    playing = false;
}

void AudioTrack::setPosition(double newPosition) {
    std::lock_guard<std::mutex> lock(mutex);
    position = std::max(0.0, std::min(newPosition, static_cast<double>(audioData.size())));
}

double AudioTrack::getPosition() const {
    std::lock_guard<std::mutex> lock(mutex);
    return position;
}

bool AudioTrack::addPlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Hier würde die Plugin-Instanz erstellt und initialisiert werden
    // auto plugin = PluginManager::getInstance().createPlugin(pluginId);
    // if (plugin) {
    //     plugins.push_back(plugin);
    //     return true;
    // }
    return false;
}

bool AudioTrack::removePlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = std::find_if(plugins.begin(), plugins.end(),
        [&](const auto& plugin) { return plugin->getId() == pluginId; });
    
    if (it != plugins.end()) {
        plugins.erase(it);
        return true;
    }
    return false;
}

void AudioTrack::updatePlugins() {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (auto& plugin : plugins) {
        plugin->update();
    }
}

std::vector<std::shared_ptr<PluginInterface>> AudioTrack::getPlugins() const {
    std::lock_guard<std::mutex> lock(mutex);
    return plugins;
}

void AudioTrack::processAudio(float* buffer, unsigned long framesPerBuffer) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!playing || audioData.empty()) {
        std::fill(buffer, buffer + framesPerBuffer, 0.0f);
        return;
    }

    // Audio-Daten kopieren
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        size_t index = static_cast<size_t>(position + i);
        if (index < audioData.size()) {
            buffer[i] = audioData[index];
        } else {
            buffer[i] = 0.0f;
        }
    }

    // Plugins verarbeiten
    processPlugins(buffer, framesPerBuffer);

    // Volume und Pan anwenden
    if (!muted) {
        for (unsigned long i = 0; i < framesPerBuffer; ++i) {
            buffer[i] *= volume;
        }
    } else {
        std::fill(buffer, buffer + framesPerBuffer, 0.0f);
    }

    // Position aktualisieren
    position += framesPerBuffer;
    if (position >= audioData.size()) {
        playing = false;
    }
}

void AudioTrack::processPlugins(float* buffer, unsigned long framesPerBuffer) {
    for (auto& plugin : plugins) {
        plugin->processAudio(buffer, framesPerBuffer);
    }
}

void AudioTrack::setVolume(float newVolume) {
    std::lock_guard<std::mutex> lock(mutex);
    volume = std::max(0.0f, std::min(1.0f, newVolume));
}

float AudioTrack::getVolume() const {
    std::lock_guard<std::mutex> lock(mutex);
    return volume;
}

void AudioTrack::setMute(bool mute) {
    std::lock_guard<std::mutex> lock(mutex);
    muted = mute;
}

bool AudioTrack::isMuted() const {
    std::lock_guard<std::mutex> lock(mutex);
    return muted;
}

void AudioTrack::setPan(float newPan) {
    std::lock_guard<std::mutex> lock(mutex);
    pan = std::max(-1.0f, std::min(1.0f, newPan));
}

float AudioTrack::getPan() const {
    std::lock_guard<std::mutex> lock(mutex);
    return pan;
}

} // namespace VRMusicStudio 