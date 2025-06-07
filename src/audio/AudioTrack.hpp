#pragma once

#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include "PluginInterface.hpp"

namespace VR_DAW {

class AudioTrack {
public:
    AudioTrack(const std::string& name);
    ~AudioTrack();

    // Track-Identifikation
    const std::string& getId() const { return id; }
    const std::string& getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }

    // Audio-Daten
    bool loadAudioFile(const std::string& filePath);
    bool saveAudioFile(const std::string& filePath) const;
    void clear();

    // Aufnahme und Wiedergabe
    void startRecording();
    void stopRecording();
    void startPlayback();
    void stopPlayback();
    void setPosition(double position);
    double getPosition() const;
    bool isRecording() const { return recording; }
    bool isPlaying() const { return playing; }

    // Plugin-Integration
    bool addPlugin(const std::string& pluginId);
    bool removePlugin(const std::string& pluginId);
    void updatePlugins();
    std::vector<std::shared_ptr<PluginInterface>> getPlugins() const;

    // Audio-Verarbeitung
    void processAudio(float* buffer, unsigned long framesPerBuffer);
    void setVolume(float volume);
    float getVolume() const;
    void setMute(bool mute);
    bool isMuted() const;
    void setPan(float pan);
    float getPan() const;

private:
    std::string id;
    std::string name;
    std::vector<float> audioData;
    double position;
    bool recording;
    bool playing;
    float volume;
    bool muted;
    float pan;
    std::vector<std::shared_ptr<PluginInterface>> plugins;
    mutable std::mutex mutex;

    // Hilfsfunktionen
    void generateId();
    void processPlugins(float* buffer, unsigned long framesPerBuffer);
};

} // namespace VR_DAW 