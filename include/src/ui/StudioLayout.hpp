#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

namespace VRMusicStudio {

class StudioLayout {
public:
    // Hauptsektionen
    struct Section {
        std::string name;
        glm::vec3 position;
        glm::vec3 size;
        bool visible;
        bool active;
    };

    // Instrumenten-Bibliothek
    struct InstrumentLibrary {
        struct Category {
            std::string name;
            std::vector<std::string> instruments;
            bool expanded;
        };
        
        std::vector<Category> categories;
        std::string searchQuery;
        std::string selectedInstrument;
    };

    // Sample-Bibliothek
    struct SampleLibrary {
        struct Category {
            std::string name;
            std::vector<std::string> samples;
            bool expanded;
        };
        
        std::vector<Category> categories;
        std::string searchQuery;
        std::string selectedSample;
    };

    // Mixer-Sektion
    struct MixerSection {
        struct Channel {
            std::string name;
            float volume;
            float pan;
            bool muted;
            bool solo;
            std::vector<std::string> effects;
        };
        
        std::vector<Channel> channels;
        std::vector<std::string> buses;
        float masterVolume;
    };

    // Arranger-Sektion
    struct ArrangerSection {
        struct Track {
            std::string name;
            std::string type;
            std::vector<float> regions;
            bool muted;
            bool solo;
        };
        
        std::vector<Track> tracks;
        float zoom;
        float scroll;
    };

    // Transport-Sektion
    struct TransportSection {
        bool playing;
        bool recording;
        float tempo;
        int timeSignature;
        float position;
    };

    StudioLayout();
    ~StudioLayout();

    // Layout-Management
    void initialize();
    void update();
    void render();

    // Sektions-Management
    void showSection(const std::string& name);
    void hideSection(const std::string& name);
    void setSectionPosition(const std::string& name, const glm::vec3& position);
    void setSectionSize(const std::string& name, const glm::vec3& size);

    // Instrumenten-Bibliothek
    void addInstrumentCategory(const std::string& name);
    void addInstrument(const std::string& category, const std::string& instrument);
    void selectInstrument(const std::string& instrument);
    void searchInstruments(const std::string& query);

    // Sample-Bibliothek
    void addSampleCategory(const std::string& name);
    void addSample(const std::string& category, const std::string& sample);
    void selectSample(const std::string& sample);
    void searchSamples(const std::string& query);

    // Mixer-Management
    void addChannel(const std::string& name);
    void setChannelVolume(const std::string& name, float volume);
    void setChannelPan(const std::string& name, float pan);
    void muteChannel(const std::string& name, bool mute);
    void soloChannel(const std::string& name, bool solo);
    void addEffectToChannel(const std::string& channel, const std::string& effect);

    // Arranger-Management
    void addTrack(const std::string& name, const std::string& type);
    void addRegion(const std::string& track, float start, float end);
    void setTrackMute(const std::string& track, bool mute);
    void setTrackSolo(const std::string& track, bool solo);
    void setZoom(float zoom);
    void setScroll(float scroll);

    // Transport-Management
    void setPlaying(bool playing);
    void setRecording(bool recording);
    void setTempo(float tempo);
    void setTimeSignature(int numerator, int denominator);
    void setPosition(float position);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; 