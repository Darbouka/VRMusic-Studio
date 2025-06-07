#pragma once

#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include "PluginInterface.hpp"

namespace VR_DAW {

struct MidiEvent {
    enum class Type {
        NoteOn,
        NoteOff,
        ControlChange,
        ProgramChange,
        PitchBend,
        Aftertouch
    };

    Type type;
    uint8_t channel;
    uint8_t data1;  // Note oder Controller-Nummer
    uint8_t data2;  // Velocity oder Controller-Wert
    double timestamp;
};

class MidiTrack {
public:
    MidiTrack(const std::string& name);
    ~MidiTrack();

    // Track-Identifikation
    const std::string& getId() const { return id; }
    const std::string& getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }

    // MIDI-Daten
    bool loadMidiFile(const std::string& filePath);
    bool saveMidiFile(const std::string& filePath) const;
    void clear();

    // MIDI-Events
    void addEvent(const MidiEvent& event);
    void removeEvent(size_t index);
    const std::vector<MidiEvent>& getEvents() const { return events; }

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

    // MIDI-Verarbeitung
    void processMidi(const MidiEvent& event);
    void setVolume(float volume);
    float getVolume() const;
    void setMute(bool mute);
    bool isMuted() const;
    void setChannel(uint8_t channel);
    uint8_t getChannel() const;

private:
    std::string id;
    std::string name;
    std::vector<MidiEvent> events;
    double position;
    bool recording;
    bool playing;
    float volume;
    bool muted;
    uint8_t channel;
    std::vector<std::shared_ptr<PluginInterface>> plugins;
    mutable std::mutex mutex;

    // Hilfsfunktionen
    void generateId();
    void processPlugins(const MidiEvent& event);
};

} // namespace VR_DAW 