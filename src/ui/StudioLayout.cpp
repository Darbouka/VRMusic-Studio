#include "StudioLayout.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <imgui.h>

namespace VRMusicStudio {

struct StudioLayout::Impl {
    // Hauptsektionen
    std::map<std::string, Section> sections;
    
    // Bibliotheken
    InstrumentLibrary instrumentLibrary;
    SampleLibrary sampleLibrary;
    
    // Mixer
    MixerSection mixer;
    
    // Arranger
    ArrangerSection arranger;
    
    // Transport
    TransportSection transport;
    
    // UI-Zustand
    bool isInitialized;
    glm::vec3 windowSize;
    
    Impl() : isInitialized(false) {
        // Initialisiere Standard-Sektionen
        sections["instrumentLibrary"] = {"Instrumenten-Bibliothek", {0.0f, 0.0f, 0.0f}, {300.0f, 600.0f, 0.0f}, true, true};
        sections["sampleLibrary"] = {"Sample-Bibliothek", {300.0f, 0.0f, 0.0f}, {300.0f, 600.0f, 0.0f}, true, true};
        sections["mixer"] = {"Mixer", {0.0f, 600.0f, 0.0f}, {800.0f, 200.0f, 0.0f}, true, true};
        sections["arranger"] = {"Arranger", {600.0f, 0.0f, 0.0f}, {800.0f, 600.0f, 0.0f}, true, true};
        sections["transport"] = {"Transport", {0.0f, 800.0f, 0.0f}, {800.0f, 100.0f, 0.0f}, true, true};
        
        // Initialisiere Instrumenten-Kategorien
        instrumentLibrary.categories = {
            {"Synthesizer", {}, true},
            {"Sampler", {}, true},
            {"Drum Machine", {}, true},
            {"Orchester", {}, true},
            {"Weltweit", {}, true},
            {"Effekte", {}, true}
        };
        
        // Initialisiere Sample-Kategorien
        sampleLibrary.categories = {
            {"Drums", {}, true},
            {"Bass", {}, true},
            {"Keys", {}, true},
            {"Guitar", {}, true},
            {"Vocals", {}, true},
            {"FX", {}, true}
        };
    }
};

StudioLayout::StudioLayout() : pImpl(std::make_unique<Impl>()) {}

StudioLayout::~StudioLayout() = default;

void StudioLayout::initialize() {
    pImpl->isInitialized = true;
}

void StudioLayout::update() {
    if (!pImpl->isInitialized) return;
    
    // Update UI-Zustand
    ImGui::Begin("VRMusic Studio");
    
    // Instrumenten-Bibliothek
    if (pImpl->sections["instrumentLibrary"].visible) {
        ImGui::BeginChild("InstrumentLibrary", ImVec2(300, 600), true);
        ImGui::Text("Instrumenten-Bibliothek");
        
        // Suchleiste
        char searchBuffer[256] = "";
        if (ImGui::InputText("Suchen", searchBuffer, 256)) {
            pImpl->instrumentLibrary.searchQuery = searchBuffer;
        }
        
        // Kategorien
        for (auto& category : pImpl->instrumentLibrary.categories) {
            if (ImGui::CollapsingHeader(category.name.c_str(), &category.expanded)) {
                for (const auto& instrument : category.instruments) {
                    if (ImGui::Selectable(instrument.c_str(), 
                        instrument == pImpl->instrumentLibrary.selectedInstrument)) {
                        pImpl->instrumentLibrary.selectedInstrument = instrument;
                    }
                }
            }
        }
        ImGui::EndChild();
    }
    
    // Sample-Bibliothek
    if (pImpl->sections["sampleLibrary"].visible) {
        ImGui::BeginChild("SampleLibrary", ImVec2(300, 600), true);
        ImGui::Text("Sample-Bibliothek");
        
        // Suchleiste
        char searchBuffer[256] = "";
        if (ImGui::InputText("Suchen", searchBuffer, 256)) {
            pImpl->sampleLibrary.searchQuery = searchBuffer;
        }
        
        // Kategorien
        for (auto& category : pImpl->sampleLibrary.categories) {
            if (ImGui::CollapsingHeader(category.name.c_str(), &category.expanded)) {
                for (const auto& sample : category.samples) {
                    if (ImGui::Selectable(sample.c_str(), 
                        sample == pImpl->sampleLibrary.selectedSample)) {
                        pImpl->sampleLibrary.selectedSample = sample;
                    }
                }
            }
        }
        ImGui::EndChild();
    }
    
    // Mixer
    if (pImpl->sections["mixer"].visible) {
        ImGui::BeginChild("Mixer", ImVec2(800, 200), true);
        ImGui::Text("Mixer");
        
        // Kanäle
        for (auto& channel : pImpl->mixer.channels) {
            ImGui::BeginGroup();
            ImGui::Text(channel.name.c_str());
            ImGui::VSliderFloat("##volume", ImVec2(20, 100), &channel.volume, 0.0f, 1.0f);
            ImGui::SliderFloat("##pan", ImVec2(20, 20), &channel.pan, -1.0f, 1.0f);
            ImGui::Checkbox("M", &channel.muted);
            ImGui::SameLine();
            ImGui::Checkbox("S", &channel.solo);
            ImGui::EndGroup();
            ImGui::SameLine();
        }
        
        // Master
        ImGui::BeginGroup();
        ImGui::Text("Master");
        ImGui::VSliderFloat("##master", ImVec2(20, 100), &pImpl->mixer.masterVolume, 0.0f, 1.0f);
        ImGui::EndGroup();
        
        ImGui::EndChild();
    }
    
    // Arranger
    if (pImpl->sections["arranger"].visible) {
        ImGui::BeginChild("Arranger", ImVec2(800, 600), true);
        ImGui::Text("Arranger");
        
        // Tracks
        for (auto& track : pImpl->arranger.tracks) {
            ImGui::BeginGroup();
            ImGui::Text(track.name.c_str());
            ImGui::Checkbox("M", &track.muted);
            ImGui::SameLine();
            ImGui::Checkbox("S", &track.solo);
            
            // Regions
            for (size_t i = 0; i < track.regions.size(); i += 2) {
                float start = track.regions[i];
                float end = track.regions[i + 1];
                ImGui::GetWindowDrawList()->AddRectFilled(
                    ImVec2(start * pImpl->arranger.zoom, ImGui::GetCursorScreenPos().y),
                    ImVec2(end * pImpl->arranger.zoom, ImGui::GetCursorScreenPos().y + 20),
                    IM_COL32(255, 255, 255, 128)
                );
            }
            ImGui::EndGroup();
        }
        
        ImGui::EndChild();
    }
    
    // Transport
    if (pImpl->sections["transport"].visible) {
        ImGui::BeginChild("Transport", ImVec2(800, 100), true);
        
        // Transport-Kontrollen
        if (ImGui::Button(pImpl->transport.playing ? "Stop" : "Play")) {
            pImpl->transport.playing = !pImpl->transport.playing;
        }
        ImGui::SameLine();
        if (ImGui::Button(pImpl->transport.recording ? "Stop Recording" : "Record")) {
            pImpl->transport.recording = !pImpl->transport.recording;
        }
        ImGui::SameLine();
        ImGui::SliderFloat("Tempo", &pImpl->transport.tempo, 20.0f, 300.0f);
        ImGui::SameLine();
        ImGui::Text("Position: %.2f", pImpl->transport.position);
        
        ImGui::EndChild();
    }
    
    ImGui::End();
}

void StudioLayout::render() {
    if (!pImpl->isInitialized) return;
    
    // Rendere UI-Elemente
    update();
}

// Implementierung der Setter-Methoden...

void StudioLayout::showSection(const std::string& name) {
    if (pImpl->sections.find(name) != pImpl->sections.end()) {
        pImpl->sections[name].visible = true;
    }
}

void StudioLayout::hideSection(const std::string& name) {
    if (pImpl->sections.find(name) != pImpl->sections.end()) {
        pImpl->sections[name].visible = false;
    }
}

void StudioLayout::setSectionPosition(const std::string& name, const glm::vec3& position) {
    if (pImpl->sections.find(name) != pImpl->sections.end()) {
        pImpl->sections[name].position = position;
    }
}

void StudioLayout::setSectionSize(const std::string& name, const glm::vec3& size) {
    if (pImpl->sections.find(name) != pImpl->sections.end()) {
        pImpl->sections[name].size = size;
    }
}

// Implementierung der Bibliotheks-Methoden...

void StudioLayout::addInstrumentCategory(const std::string& name) {
    pImpl->instrumentLibrary.categories.push_back({name, {}, true});
}

void StudioLayout::addInstrument(const std::string& category, const std::string& instrument) {
    for (auto& cat : pImpl->instrumentLibrary.categories) {
        if (cat.name == category) {
            cat.instruments.push_back(instrument);
            break;
        }
    }
}

void StudioLayout::selectInstrument(const std::string& instrument) {
    pImpl->instrumentLibrary.selectedInstrument = instrument;
}

void StudioLayout::searchInstruments(const std::string& query) {
    pImpl->instrumentLibrary.searchQuery = query;
}

// Implementierung der Sample-Bibliotheks-Methoden...

void StudioLayout::addSampleCategory(const std::string& name) {
    pImpl->sampleLibrary.categories.push_back({name, {}, true});
}

void StudioLayout::addSample(const std::string& category, const std::string& sample) {
    for (auto& cat : pImpl->sampleLibrary.categories) {
        if (cat.name == category) {
            cat.samples.push_back(sample);
            break;
        }
    }
}

void StudioLayout::selectSample(const std::string& sample) {
    pImpl->sampleLibrary.selectedSample = sample;
}

void StudioLayout::searchSamples(const std::string& query) {
    pImpl->sampleLibrary.searchQuery = query;
}

// Implementierung der Mixer-Methoden...

void StudioLayout::addChannel(const std::string& name) {
    pImpl->mixer.channels.push_back({name, 1.0f, 0.0f, false, false, {}});
}

void StudioLayout::setChannelVolume(const std::string& name, float volume) {
    for (auto& channel : pImpl->mixer.channels) {
        if (channel.name == name) {
            channel.volume = volume;
            break;
        }
    }
}

void StudioLayout::setChannelPan(const std::string& name, float pan) {
    for (auto& channel : pImpl->mixer.channels) {
        if (channel.name == name) {
            channel.pan = pan;
            break;
        }
    }
}

void StudioLayout::muteChannel(const std::string& name, bool mute) {
    for (auto& channel : pImpl->mixer.channels) {
        if (channel.name == name) {
            channel.muted = mute;
            break;
        }
    }
}

void StudioLayout::soloChannel(const std::string& name, bool solo) {
    for (auto& channel : pImpl->mixer.channels) {
        if (channel.name == name) {
            channel.solo = solo;
            break;
        }
    }
}

void StudioLayout::addEffectToChannel(const std::string& channel, const std::string& effect) {
    for (auto& ch : pImpl->mixer.channels) {
        if (ch.name == channel) {
            ch.effects.push_back(effect);
            break;
        }
    }
}

// Implementierung der Arranger-Methoden...

void StudioLayout::addTrack(const std::string& name, const std::string& type) {
    pImpl->arranger.tracks.push_back({name, type, {}, false, false});
}

void StudioLayout::addRegion(const std::string& track, float start, float end) {
    for (auto& t : pImpl->arranger.tracks) {
        if (t.name == track) {
            t.regions.push_back(start);
            t.regions.push_back(end);
            break;
        }
    }
}

void StudioLayout::setTrackMute(const std::string& track, bool mute) {
    for (auto& t : pImpl->arranger.tracks) {
        if (t.name == track) {
            t.muted = mute;
            break;
        }
    }
}

void StudioLayout::setTrackSolo(const std::string& track, bool solo) {
    for (auto& t : pImpl->arranger.tracks) {
        if (t.name == track) {
            t.solo = solo;
            break;
        }
    }
}

void StudioLayout::setZoom(float zoom) {
    pImpl->arranger.zoom = zoom;
}

void StudioLayout::setScroll(float scroll) {
    pImpl->arranger.scroll = scroll;
}

// Implementierung der Transport-Methoden...

void StudioLayout::setPlaying(bool playing) {
    pImpl->transport.playing = playing;
}

void StudioLayout::setRecording(bool recording) {
    pImpl->transport.recording = recording;
}

void StudioLayout::setTempo(float tempo) {
    pImpl->transport.tempo = tempo;
}

void StudioLayout::setTimeSignature(int numerator, int denominator) {
    pImpl->transport.timeSignature = numerator;
}

void StudioLayout::setPosition(float position) {
    pImpl->transport.position = position;
}

} // namespace VRMusicStudio 