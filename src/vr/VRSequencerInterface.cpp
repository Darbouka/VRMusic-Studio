#include "VRSequencerInterface.hpp"
<<<<<<< HEAD
#include <algorithm>
#include <cmath>

namespace VR_DAW {

VRSequencerInterface::VRSequencerInterface() {
    initializePatterns();
}
=======
#include <spdlog/spdlog.h>
#include <RtMidi.h>
#include <algorithm>
#include <cmath>

namespace VRMusicStudio {

struct VRSequencerInterface::Impl {
    // MIDI
    RtMidiIn* midiIn;
    RtMidiOut* midiOut;
    std::vector<unsigned char> midiMessage;
    
    // Sequencer
    struct Pattern {
        std::string name;
        std::vector<MidiNote> notes;
        float tempo;
        int timeSignature;
        bool loop;
    };
    
    struct Track {
        std::string name;
        int channel;
        std::vector<Pattern> patterns;
        bool muted;
        bool solo;
        float volume;
        float pan;
    };
    
    std::vector<Track> tracks;
    float globalTempo;
    int globalTimeSignature;
    bool isPlaying;
    bool isRecording;
    double currentPosition;
    
    // Callbacks
    std::function<void(const MidiNote&)> noteCallback;
    std::function<void(float)> tempoCallback;
    std::function<void(int)> timeSignatureCallback;
    
    Impl() : midiIn(nullptr), midiOut(nullptr),
             globalTempo(120.0f), globalTimeSignature(4),
             isPlaying(false), isRecording(false),
             currentPosition(0.0) {}
};

VRSequencerInterface::VRSequencerInterface() : pImpl(std::make_unique<Impl>()) {}
>>>>>>> 0dff1c4 (init 2)

VRSequencerInterface::~VRSequencerInterface() {
    shutdown();
}

<<<<<<< HEAD
void VRSequencerInterface::initialize() {
    initializePatterns();
    updatePatternPositions();
}

void VRSequencerInterface::update() {
    updatePatternPositions();
    calculateHapticFeedback();
    updateVisualizationData();
    updateKIFeedback();
    validateState();
}

void VRSequencerInterface::shutdown() {
    // Cleanup
}

void VRSequencerInterface::processHandInput(const glm::vec3& position, const glm::quat& rotation, bool isGrabbing) {
    handPositions.push_back(position);
    handRotations.push_back(rotation);
    this->isGrabbing.push_back(isGrabbing);

    if (isGrabbing) {
        // Pattern-Interaktion
        for (auto& pattern : patterns) {
            if (isHandNearPattern(position, pattern)) {
                editPattern(pattern.name);
                updateHapticFeedback();
=======
bool VRSequencerInterface::initialize() {
    try {
        pImpl->midiIn = new RtMidiIn();
        pImpl->midiOut = new RtMidiOut();
        
        // Öffne MIDI-Ports
        unsigned int numPorts = pImpl->midiIn->getPortCount();
        if (numPorts > 0) {
            pImpl->midiIn->openPort(0);
        }
        
        numPorts = pImpl->midiOut->getPortCount();
        if (numPorts > 0) {
            pImpl->midiOut->openPort(0);
        }
        
        // Setze MIDI-Callback
        pImpl->midiIn->setCallback([](double timeStamp, std::vector<unsigned char>* message, void* userData) {
            auto* sequencer = static_cast<VRSequencerInterface*>(userData);
            sequencer->handleMidiMessage(*message);
        }, this);
        
        return true;
    }
    catch (RtMidiError& error) {
        spdlog::error("MIDI initialization failed: {}", error.getMessage());
        return false;
    }
}

void VRSequencerInterface::shutdown() {
    if (pImpl->midiIn) {
        pImpl->midiIn->closePort();
        delete pImpl->midiIn;
        pImpl->midiIn = nullptr;
    }
    
    if (pImpl->midiOut) {
        pImpl->midiOut->closePort();
        delete pImpl->midiOut;
        pImpl->midiOut = nullptr;
    }
}

void VRSequencerInterface::createTrack(const std::string& name, int channel) {
    Impl::Track track;
    track.name = name;
    track.channel = channel;
    track.muted = false;
    track.solo = false;
    track.volume = 1.0f;
    track.pan = 0.0f;
    
    pImpl->tracks.push_back(track);
}

void VRSequencerInterface::createPattern(const std::string& trackName, const std::string& patternName) {
    for (auto& track : pImpl->tracks) {
        if (track.name == trackName) {
            Impl::Pattern pattern;
            pattern.name = patternName;
            pattern.tempo = pImpl->globalTempo;
            pattern.timeSignature = pImpl->globalTimeSignature;
            pattern.loop = true;
            
            track.patterns.push_back(pattern);
            break;
        }
    }
}

void VRSequencerInterface::addNote(const std::string& trackName, const std::string& patternName, const MidiNote& note) {
    for (auto& track : pImpl->tracks) {
        if (track.name == trackName) {
            for (auto& pattern : track.patterns) {
                if (pattern.name == patternName) {
                    pattern.notes.push_back(note);
                    break;
                }
            }
            break;
        }
    }
}

void VRSequencerInterface::removeNote(const std::string& trackName, const std::string& patternName, int noteIndex) {
    for (auto& track : pImpl->tracks) {
        if (track.name == trackName) {
            for (auto& pattern : track.patterns) {
                if (pattern.name == patternName) {
                    if (noteIndex >= 0 && noteIndex < pattern.notes.size()) {
                        pattern.notes.erase(pattern.notes.begin() + noteIndex);
                    }
                    break;
                }
            }
            break;
        }
    }
}

void VRSequencerInterface::play() {
    pImpl->isPlaying = true;
    pImpl->isRecording = false;
    pImpl->currentPosition = 0.0;
    
    // Starte MIDI-Wiedergabe
    for (const auto& track : pImpl->tracks) {
        if (!track.muted) {
            for (const auto& pattern : track.patterns) {
                for (const auto& note : pattern.notes) {
                    std::vector<unsigned char> message = {
                        static_cast<unsigned char>(0x90 + track.channel),
                        static_cast<unsigned char>(note.pitch),
                        static_cast<unsigned char>(note.velocity)
                    };
                    pImpl->midiOut->sendMessage(&message);
                }
>>>>>>> 0dff1c4 (init 2)
            }
        }
    }
}

<<<<<<< HEAD
void VRSequencerInterface::processControllerInput(const glm::vec3& position, const glm::quat& rotation, float triggerValue) {
    triggerValues.push_back(triggerValue);

    // Controller-Interaktion
    for (auto& pattern : patterns) {
        if (isControllerNearPattern(position, pattern)) {
            editPattern(pattern.name);
            updateHapticFeedback();
        }
    }
}

void VRSequencerInterface::updateHapticFeedback() {
    calculateHapticFeedback();
    
    // Aktualisiere Haptisches Feedback
    for (auto& feedback : hapticFeedback) {
        // Implementiere Haptisches Feedback
        feedback.intensity = calculateFeedbackIntensity();
        feedback.frequency = calculateFeedbackFrequency();
        feedback.duration = calculateFeedbackDuration();
    }
}

void VRSequencerInterface::updateVisualization() {
    updatePatternVisualization();
    updateSequenceVisualization();
    updateAutomationVisualization();
    updateAnalysisVisualization();
}

void VRSequencerInterface::createPattern(const std::string& name) {
    Pattern pattern;
    pattern.name = name;
    pattern.isAutomated = false;
    patterns.push_back(pattern);
    
    if (kiIntegration) {
        kiIntegration->createPattern(name);
    }
}

void VRSequencerInterface::editPattern(const std::string& name) {
    auto it = std::find_if(patterns.begin(), patterns.end(),
                          [&name](const Pattern& p) { return p.name == name; });
    
    if (it != patterns.end()) {
        if (kiIntegration) {
            kiIntegration->editPattern(name);
        }
    }
}

void VRSequencerInterface::deletePattern(const std::string& name) {
    auto it = std::find_if(patterns.begin(), patterns.end(),
                          [&name](const Pattern& p) { return p.name == name; });
    
    if (it != patterns.end()) {
        patterns.erase(it);
        if (kiIntegration) {
            kiIntegration->deletePattern(name);
        }
    }
}

void VRSequencerInterface::savePattern(const std::string& name) {
    auto it = std::find_if(patterns.begin(), patterns.end(),
                          [&name](const Pattern& p) { return p.name == name; });
    
    if (it != patterns.end()) {
        if (kiIntegration) {
            kiIntegration->savePattern(name);
        }
    }
}

void VRSequencerInterface::loadPattern(const std::string& name) {
    if (kiIntegration) {
        kiIntegration->loadPattern(name);
    }
}

void VRSequencerInterface::playSequence() {
    currentSequence.isPlaying = true;
    currentSequence.isPaused = false;
}

void VRSequencerInterface::pauseSequence() {
    currentSequence.isPaused = true;
}

void VRSequencerInterface::stopSequence() {
    currentSequence.isPlaying = false;
    currentSequence.isPaused = false;
}

void VRSequencerInterface::setTempo(float tempo) {
    currentSequence.tempo = std::clamp(tempo, 40.0f, 240.0f);
}

void VRSequencerInterface::setSwing(float swing) {
    currentSequence.swing = std::clamp(swing, 0.0f, 1.0f);
}

void VRSequencerInterface::connectKIIntegration(std::shared_ptr<SequencerKIIntegration> kiIntegration) {
    this->kiIntegration = kiIntegration;
}

void VRSequencerInterface::updateKIFeedback() {
    if (kiIntegration) {
        kiIntegration->updateKIFeedback();
    }
}

void VRSequencerInterface::applyKISuggestions() {
    if (kiIntegration) {
        kiIntegration->applyKISuggestions();
    }
}

void VRSequencerInterface::updatePatternVisualization() {
    updatePatternPositions();
}

void VRSequencerInterface::updateSequenceVisualization() {
    // Aktualisiere Sequenz-Visualisierung
}

void VRSequencerInterface::updateAutomationVisualization() {
    // Aktualisiere Automatisierungs-Visualisierung
}

void VRSequencerInterface::updateAnalysisVisualization() {
    // Aktualisiere Analyse-Visualisierung
}

void VRSequencerInterface::initializePatterns() {
    // Initialisiere Patterns
    patterns.clear();
    currentSequence.patterns.clear();
    currentSequence.tempo = 120.0f;
    currentSequence.swing = 0.0f;
    currentSequence.isPlaying = false;
    currentSequence.isPaused = false;
}

void VRSequencerInterface::updatePatternPositions() {
    // Aktualisiere 3D-Positionen der Patterns
    float radius = 2.0f;
    float height = 1.5f;
    int numPatterns = patterns.size();
    
    for (int i = 0; i < numPatterns; ++i) {
        float angle = (2.0f * M_PI * i) / numPatterns;
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        patterns[i].position = glm::vec3(x, height, z);
        patterns[i].rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }
}

void VRSequencerInterface::calculateHapticFeedback() {
    // Berechne Haptisches Feedback basierend auf Pattern-Interaktion
    for (auto& feedback : hapticFeedback) {
        feedback.intensity = calculateFeedbackIntensity();
        feedback.frequency = calculateFeedbackFrequency();
        feedback.duration = calculateFeedbackDuration();
    }
}

void VRSequencerInterface::updateVisualizationData() {
    // Aktualisiere Visualisierungsdaten
    visualization.patternPositions.clear();
    visualization.patternRotations.clear();
    
    for (const auto& pattern : patterns) {
        visualization.patternPositions.push_back(pattern.position);
        visualization.patternRotations.push_back(pattern.rotation);
    }
}

void VRSequencerInterface::validateState() {
    // Validiere Zustand
    for (const auto& pattern : patterns) {
        if (!validatePattern(pattern)) {
            handleErrors();
=======
void VRSequencerInterface::pause() {
    pImpl->isPlaying = false;
    
    // Stoppe MIDI-Wiedergabe
    for (const auto& track : pImpl->tracks) {
        std::vector<unsigned char> message = {
            static_cast<unsigned char>(0xB0 + track.channel),
            0x7B,  // All Notes Off
            0x00
        };
        pImpl->midiOut->sendMessage(&message);
    }
}

void VRSequencerInterface::stop() {
    pause();
    pImpl->currentPosition = 0.0;
}

void VRSequencerInterface::record() {
    pImpl->isRecording = true;
    pImpl->isPlaying = false;
    pImpl->currentPosition = 0.0;
}

void VRSequencerInterface::setTempo(float tempo) {
    pImpl->globalTempo = tempo;
    if (pImpl->tempoCallback) {
        pImpl->tempoCallback(tempo);
    }
}

void VRSequencerInterface::setTimeSignature(int timeSignature) {
    pImpl->globalTimeSignature = timeSignature;
    if (pImpl->timeSignatureCallback) {
        pImpl->timeSignatureCallback(timeSignature);
    }
}

void VRSequencerInterface::setTrackMute(const std::string& trackName, bool muted) {
    for (auto& track : pImpl->tracks) {
        if (track.name == trackName) {
            track.muted = muted;
            break;
>>>>>>> 0dff1c4 (init 2)
        }
    }
}

<<<<<<< HEAD
void VRSequencerInterface::handleErrors() {
    // Implementiere Fehlerbehandlung
}

// Hilfsfunktionen
bool VRSequencerInterface::isHandNearPattern(const glm::vec3& position, const Pattern& pattern) {
    float distance = glm::distance(position, pattern.position);
    return distance < 0.2f; // 20cm Interaktionsradius
}

bool VRSequencerInterface::isControllerNearPattern(const glm::vec3& position, const Pattern& pattern) {
    return isHandNearPattern(position, pattern);
}

float VRSequencerInterface::calculateFeedbackIntensity() {
    // Implementiere Feedback-Intensitäts-Berechnung
    return 0.5f;
}

float VRSequencerInterface::calculateFeedbackFrequency() {
    // Implementiere Feedback-Frequenz-Berechnung
    return 100.0f;
}

float VRSequencerInterface::calculateFeedbackDuration() {
    // Implementiere Feedback-Dauer-Berechnung
    return 0.1f;
}

bool VRSequencerInterface::validatePattern(const Pattern& pattern) {
    // Implementiere Pattern-Validierung
    return true;
}

=======
void VRSequencerInterface::setTrackSolo(const std::string& trackName, bool solo) {
    for (auto& track : pImpl->tracks) {
        if (track.name == trackName) {
            track.solo = solo;
            break;
        }
    }
}

void VRSequencerInterface::setTrackVolume(const std::string& trackName, float volume) {
    for (auto& track : pImpl->tracks) {
        if (track.name == trackName) {
            track.volume = std::clamp(volume, 0.0f, 1.0f);
            break;
        }
    }
}

void VRSequencerInterface::setTrackPan(const std::string& trackName, float pan) {
    for (auto& track : pImpl->tracks) {
        if (track.name == trackName) {
            track.pan = std::clamp(pan, -1.0f, 1.0f);
            break;
        }
    }
}

void VRSequencerInterface::handleMidiMessage(const std::vector<unsigned char>& message) {
    if (message.size() < 2) return;
    
    unsigned char status = message[0] & 0xF0;
    unsigned char channel = message[0] & 0x0F;
    
    if (status == 0x90 && message[2] > 0) { // Note On
        MidiNote note;
        note.pitch = message[1];
        note.velocity = message[2];
        note.startTime = pImpl->currentPosition;
        note.duration = 0.25; // Standard-Dauer
        
        if (pImpl->isRecording) {
            // Füge Note zum aktuellen Pattern hinzu
            for (auto& track : pImpl->tracks) {
                if (track.channel == channel) {
                    for (auto& pattern : track.patterns) {
                        pattern.notes.push_back(note);
                        break;
                    }
                    break;
                }
            }
        }
        
        if (pImpl->noteCallback) {
            pImpl->noteCallback(note);
        }
    }
}

void VRSequencerInterface::setNoteCallback(std::function<void(const MidiNote&)> callback) {
    pImpl->noteCallback = callback;
}

void VRSequencerInterface::setTempoCallback(std::function<void(float)> callback) {
    pImpl->tempoCallback = callback;
}

void VRSequencerInterface::setTimeSignatureCallback(std::function<void(int)> callback) {
    pImpl->timeSignatureCallback = callback;
}

} // namespace VRMusicStudio 
>>>>>>> 0dff1c4 (init 2)
} // namespace VR_DAW 