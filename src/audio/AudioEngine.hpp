#pragma once

#include <memory>
#include <vector>
#include <string>
#include <mutex>
#include <portaudio.h>
#include <vst3/plugininterfaces/vst/ivstaudioprocessor.h>
#include <vst3/plugininterfaces/vst/ivsteditcontroller.h>
#include "../core/Logger.hpp"
#include "../core/EventSystem.hpp"
#include "../plugins/PluginInterface.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <map>
#include <functional>

namespace VR_DAW {

struct AudioTrack {
    std::string id;
    std::string name;
    std::vector<float> buffer;
    float volume;
    bool isMuted;
    bool isSolo;
    std::vector<std::shared_ptr<PluginInterface>> plugins;
};

struct MidiTrack {
    std::string id;
    std::string name;
    std::vector<uint8_t> data;
    bool isMuted;
    bool isSolo;
    std::vector<std::shared_ptr<PluginInterface>> plugins;
};

class AudioEngine {
public:
    static AudioEngine& getInstance();

    bool initialize();
    void shutdown();
    bool start();
    void stop();
    bool isRunning() const;

    // Audio-Stream-Verwaltung
    bool createStream(int inputChannels, int outputChannels, double sampleRate);
    void closeStream();
    
    // Audio-Verarbeitung
    void processAudio(const float* input, float* output, unsigned long framesPerBuffer);
    void setSampleRate(double rate);
    double getSampleRate() const;
    
    // 3D-Audio
    void setListenerPosition(const glm::vec3& position);
    void setListenerOrientation(const glm::quat& orientation);
    void setSourcePosition(const std::string& sourceId, const glm::vec3& position);
    void setSourceOrientation(const std::string& sourceId, const glm::quat& orientation);
    void setSourceVolume(const std::string& sourceId, float volume);
    void setSourceMute(const std::string& sourceId, bool mute);
    
    // Track-Management
    std::shared_ptr<AudioTrack> createAudioTrack(const std::string& name);
    std::shared_ptr<MidiTrack> createMidiTrack(const std::string& name);
    void removeTrack(const std::string& trackId);
    std::shared_ptr<AudioTrack> getAudioTrack(const std::string& trackId);
    std::shared_ptr<MidiTrack> getMidiTrack(const std::string& trackId);
    
    // Plugin-Integration
    bool addPluginToTrack(const std::string& trackId, const std::string& pluginId);
    bool removePluginFromTrack(const std::string& trackId, const std::string& pluginId);
    void updatePlugins();
    
    // Aufnahme und Wiedergabe
    bool startRecording(const std::string& trackId);
    void stopRecording(const std::string& trackId);
    bool startPlayback();
    void stopPlayback();
    void setPlaybackPosition(double position);
    double getPlaybackPosition() const;
    
    // MIDI-Verarbeitung
    void processMidiMessage(const uint8_t* data, size_t length);
    void sendMidiMessage(const uint8_t* data, size_t length);
    
    // Projekt-Verwaltung
    bool saveProject(const std::string& path);
    bool loadProject(const std::string& path);
    void createNewProject();
    void closeProject();

    // Lifecycle Management
    void update();

    // Audio Processing
    void processAudio(const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processChannel(const std::string& channelName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);
    void processEffect(const std::string& effectName, const std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);

    // Channel Management
    void createChannel(const std::string& name);
    void deleteChannel(const std::string& name);
    void setChannelVolume(const std::string& name, float volume);
    void setChannelPan(const std::string& name, float pan);
    void setChannelMute(const std::string& name, bool mute);
    void setChannelSolo(const std::string& name, bool solo);

    // Effect Management
    void addEffect(const std::string& channelName, const std::string& effectName);
    void removeEffect(const std::string& channelName, const std::string& effectName);
    void setEffectParameter(const std::string& channelName, const std::string& effectName, const std::string& parameterName, float value);

    // Analysis
    float getChannelLevel(const std::string& name);
    float getChannelSpectrum(const std::string& name);
    float getChannelPhase(const std::string& name);
    float getChannelCorrelation(const std::string& name);
    float getChannelDynamics(const std::string& name);
    float getChannelStereo(const std::string& name);
    float getChannelFrequency(const std::string& name);
    float getChannelTransient(const std::string& name);

    // Visualization
    void updateChannelVisualization(const std::string& name);
    void updateEffectVisualization(const std::string& channelName, const std::string& effectName);
    void updateAnalysisVisualization();

    // Erweiterte Audio-Verarbeitungstools
    struct AdvancedProcessing {
        // Spezialisierte Audio-Tools
        void applySpectralProcessing(const std::vector<float>& input, std::vector<float>& output);
        void applyDynamicProcessing(const std::vector<float>& input, std::vector<float>& output);
        void applySpatialProcessing(const std::vector<float>& input, std::vector<float>& output);
        void applyTemporalProcessing(const std::vector<float>& input, std::vector<float>& output);
        
        // Erweiterte Plugin-Funktionen
        void loadCustomPlugin(const std::string& pluginPath);
        void configurePluginChain(const std::string& chainConfig);
        void applyPluginPreset(const std::string& presetName);
        void savePluginState(const std::string& stateName);
    };

    // VR-spezifische Visualisierungen
    struct VRVisualization {
        void renderSpectralView(const std::vector<float>& spectrum);
        void renderWaveformView(const std::vector<float>& waveform);
        void renderPhaseView(const std::vector<float>& phase);
        void renderSpatialView(const std::vector<float>& spatial);
    };

    // KI-Integration
    struct AIProcessing {
        void applyAIMastering(const std::vector<float>& input, std::vector<float>& output);
        void applyAIMixing(const std::vector<float>& input, std::vector<float>& output);
        void applyAISoundDesign(const std::vector<float>& input, std::vector<float>& output);
        void generateAIPresets(const std::string& style);
    };

    // Erweiterte Audio-Engine
    struct EnhancedEngine {
        void processMultiChannel(const std::vector<std::vector<float>>& input, 
                               std::vector<std::vector<float>>& output);
        void applyAdvancedEffects(const std::vector<float>& input, std::vector<float>& output);
        void handleComplexRouting(const std::string& routingConfig);
        void optimizePerformance();
    };

    // Verbesserte Plugin-Unterstützung
    struct PluginSupport {
        void loadVST3Plugin(const std::string& path);
        void loadAUPlugin(const std::string& path);
        void loadLV2Plugin(const std::string& path);
        void configurePluginParameters(const std::string& pluginId, 
                                     const std::map<std::string, float>& params);
    };

    // Erweiterte Benutzeroberfläche
    struct EnhancedUI {
        void createCustomControl(const std::string& controlType, 
                               const std::map<std::string, std::string>& properties);
        void updateVisualization(const std::string& visualizationType, 
                               const std::vector<float>& data);
        void handleVRInteraction(const std::string& interactionType, 
                               const std::map<std::string, float>& parameters);
    };

    // Neue Member-Variablen
    AdvancedProcessing advancedProcessing;
    VRVisualization vrVisualization;
    AIProcessing aiProcessing;
    EnhancedEngine enhancedEngine;
    PluginSupport pluginSupport;
    EnhancedUI enhancedUI;

private:
    AudioEngine();
    ~AudioEngine();
    AudioEngine(const AudioEngine&) = delete;
    AudioEngine& operator=(const AudioEngine&) = delete;

    static int paCallback(const void* inputBuffer, void* outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void* userData);

    bool initializePortAudio();
    void shutdownPortAudio();
    void processTrack(const std::shared_ptr<AudioTrack>& track, float* output, unsigned long framesPerBuffer);
    void processMidiTrack(const std::shared_ptr<MidiTrack>& track, unsigned long framesPerBuffer);
    void update3DAudio();

    PaStream* stream;
    double sampleRate;
    bool running;
    std::mutex mutex;
    Logger logger;
    
    // 3D-Audio
    glm::vec3 listenerPosition;
    glm::quat listenerOrientation;
    std::map<std::string, glm::vec3> sourcePositions;
    std::map<std::string, glm::quat> sourceOrientations;
    std::map<std::string, float> sourceVolumes;
    std::map<std::string, bool> sourceMutes;
    
    // Track-Verwaltung
    std::vector<std::shared_ptr<AudioTrack>> audioTracks;
    std::vector<std::shared_ptr<MidiTrack>> midiTracks;
    
    // Aufnahme und Wiedergabe
    bool isRecording;
    bool isPlaying;
    double playbackPosition;
    std::string currentProject;

    // Components
    struct {
        std::map<std::string, std::vector<float>> channelBuffers;
        std::map<std::string, std::vector<float>> effectBuffers;
        std::map<std::string, float> channelVolumes;
        std::map<std::string, float> channelPans;
        std::map<std::string, bool> channelMutes;
        std::map<std::string, bool> channelSolos;
    } state;

    // Parameters
    struct {
        float masterVolume = 1.0f;
        float masterPan = 0.0f;
        float sampleRate = 44100.0f;
        int bufferSize = 1024;
        int numChannels = 2;
    } parameters;

    // Analysis
    struct {
        std::map<std::string, float> channelLevels;
        std::map<std::string, float> channelSpectrums;
        std::map<std::string, float> channelPhases;
        std::map<std::string, float> channelCorrelations;
        std::map<std::string, float> channelDynamics;
        std::map<std::string, float> channelStereos;
        std::map<std::string, float> channelFrequencies;
        std::map<std::string, float> channelTransients;
    } analysis;

    // Helper Functions
    void initializeComponents();
    void updateState();
    void processAudioToChannels();
    void processChannelsToEffects();
    void processEffectsToOutput();
    void updateParameters();
    void updateAnalysis();
    void generateVisualization();
    void validateState();
    void handleErrors();

    // Buffer Validation and Calculations
    bool validateAudioBuffer(const std::vector<float>& buffer);
    float calculateLevel(const std::vector<float>& buffer);
    float calculateSpectrum(const std::vector<float>& buffer);
    float calculatePhase(const std::vector<float>& buffer);
    float calculateCorrelation(const std::vector<float>& leftBuffer, const std::vector<float>& rightBuffer);
    float calculateDynamics(const std::vector<float>& buffer);
    float calculateStereo(const std::vector<float>& leftBuffer, const std::vector<float>& rightBuffer);
    float calculateFrequency(const std::vector<float>& buffer);
    float calculateTransient(const std::vector<float>& buffer);
};

} // namespace VR_DAW 