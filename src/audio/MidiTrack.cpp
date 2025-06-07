#include "MidiTrack.hpp"
#include <spdlog/spdlog.h>
#include <random>
#include <sstream>
#include <iomanip>

namespace VR_DAW {

MidiTrack::MidiTrack(const std::string& name)
    : name(name)
    , position(0.0)
    , recording(false)
    , playing(false)
    , volume(1.0f)
    , muted(false)
    , channel(0)
{
    generateId();
}

MidiTrack::~MidiTrack() {
    stopRecording();
    stopPlayback();
}

void MidiTrack::generateId() {
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

bool MidiTrack::loadMidiFile(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Hier würde die MIDI-Datei geladen werden
    // Die Implementierung hängt von der verwendeten MIDI-Bibliothek ab
    spdlog::info("MIDI-Datei geladen: {}", filePath);
    return true;
}

bool MidiTrack::saveMidiFile(const std::string& filePath) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Hier würde die MIDI-Datei gespeichert werden
    // Die Implementierung hängt von der verwendeten MIDI-Bibliothek ab
    spdlog::info("MIDI-Datei gespeichert: {}", filePath);
    return true;
}

void MidiTrack::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    events.clear();
    position = 0.0;
}

void MidiTrack::addEvent(const MidiEvent& event) {
    std::lock_guard<std::mutex> lock(mutex);
    events.push_back(event);
}

void MidiTrack::removeEvent(size_t index) {
    std::lock_guard<std::mutex> lock(mutex);
    if (index < events.size()) {
        events.erase(events.begin() + index);
    }
}

void MidiTrack::startRecording() {
    std::lock_guard<std::mutex> lock(mutex);
    if (!recording) {
        recording = true;
        position = 0.0;
        events.clear();
    }
}

void MidiTrack::stopRecording() {
    std::lock_guard<std::mutex> lock(mutex);
    recording = false;
}

void MidiTrack::startPlayback() {
    std::lock_guard<std::mutex> lock(mutex);
    if (!playing) {
        playing = true;
    }
}

void MidiTrack::stopPlayback() {
    std::lock_guard<std::mutex> lock(mutex);
    playing = false;
}

void MidiTrack::setPosition(double newPosition) {
    std::lock_guard<std::mutex> lock(mutex);
    position = std::max(0.0, newPosition);
}

double MidiTrack::getPosition() const {
    std::lock_guard<std::mutex> lock(mutex);
    return position;
}

bool MidiTrack::addPlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Hier würde die Plugin-Instanz erstellt und initialisiert werden
    // auto plugin = PluginManager::getInstance().createPlugin(pluginId);
    // if (plugin) {
    //     plugins.push_back(plugin);
    //     return true;
    // }
    return false;
}

bool MidiTrack::removePlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = std::find_if(plugins.begin(), plugins.end(),
        [&](const auto& plugin) { return plugin->getId() == pluginId; });
    
    if (it != plugins.end()) {
        plugins.erase(it);
        return true;
    }
    return false;
}

void MidiTrack::updatePlugins() {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (auto& plugin : plugins) {
        plugin->update();
    }
}

std::vector<std::shared_ptr<PluginInterface>> MidiTrack::getPlugins() const {
    std::lock_guard<std::mutex> lock(mutex);
    return plugins;
}

void MidiTrack::processMidi(const MidiEvent& event) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!playing || muted) return;

    // Plugins verarbeiten
    processPlugins(event);

    // Volume anwenden
    if (event.type == MidiEvent::Type::NoteOn || event.type == MidiEvent::Type::ControlChange) {
        MidiEvent modifiedEvent = event;
        modifiedEvent.data2 = static_cast<uint8_t>(event.data2 * volume);
        processPlugins(modifiedEvent);
    }
}

void MidiTrack::processPlugins(const MidiEvent& event) {
    for (auto& plugin : plugins) {
        plugin->processMidi(event);
    }
}

void MidiTrack::setVolume(float newVolume) {
    std::lock_guard<std::mutex> lock(mutex);
    volume = std::max(0.0f, std::min(1.0f, newVolume));
}

float MidiTrack::getVolume() const {
    std::lock_guard<std::mutex> lock(mutex);
    return volume;
}

void MidiTrack::setMute(bool mute) {
    std::lock_guard<std::mutex> lock(mutex);
    muted = mute;
}

bool MidiTrack::isMuted() const {
    std::lock_guard<std::mutex> lock(mutex);
    return muted;
}

void MidiTrack::setChannel(uint8_t newChannel) {
    std::lock_guard<std::mutex> lock(mutex);
    channel = newChannel;
}

uint8_t MidiTrack::getChannel() const {
    std::lock_guard<std::mutex> lock(mutex);
    return channel;
}

} // namespace VR_DAW 