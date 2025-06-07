#pragma once

#include "HolographicAI.hpp"
#include "../audio/AudioEngine.hpp"
#include "../instruments/VRInstruments.hpp"
#include <functional>
#include <map>

namespace VR_DAW {

// Revolutionäre Aufgaben-Typen
enum class TaskType {
    AudioProcessing,
    InstrumentSetup,
    Mixing,
    Mastering,
    Recording,
    Editing,
    PluginManagement,
    Custom
};

// Revolutionäre Aufgaben-Daten
struct TaskData {
    TaskType type;
    std::string description;
    std::function<void()> execute;
    std::function<void()> undo;
    bool isCompleted;
    float progress;
};

// Revolutionäre Assistenten-Klasse
class AIAssistant {
public:
    AIAssistant();
    ~AIAssistant();

    // Revolutionäre Initialisierung
    void initialize();
    void shutdown();
    void update();

    // Revolutionäre Aufgaben-Verwaltung
    void assignTask(const TaskData& task);
    void cancelTask(const std::string& taskId);
    void pauseTask(const std::string& taskId);
    void resumeTask(const std::string& taskId);

    // Revolutionäre Audio-Aufgaben
    void processAudio(const std::string& trackId, const std::string& effect);
    void mixTrack(const std::string& trackId, float volume, float pan);
    void masterTrack(const std::string& trackId, const std::string& preset);

    // Revolutionäre Instrumenten-Aufgaben
    void setupInstrument(const std::string& instrumentId, const std::string& preset);
    void tuneInstrument(const std::string& instrumentId);
    void adjustInstrument(const std::string& instrumentId, const std::string& parameter);

    // Revolutionäre Aufnahme-Aufgaben
    void startRecording(const std::string& trackId);
    void stopRecording(const std::string& trackId);
    void editRecording(const std::string& trackId, const std::string& editType);

    // Revolutionäre Plugin-Aufgaben
    void loadPlugin(const std::string& pluginId, const std::string& trackId);
    void configurePlugin(const std::string& pluginId, const std::string& parameter, float value);
    void savePluginPreset(const std::string& pluginId, const std::string& presetName);

    // Revolutionäre Assistenten-Interaktion
    void respondToCommand(const std::string& command);
    void provideFeedback(const std::string& feedback);
    void suggestImprovements(const std::string& context);

private:
    std::shared_ptr<HolographicAI> hologram;
    std::map<std::string, TaskData> activeTasks;
    bool isBusy;
    float lastUpdateTime;

    // Revolutionäre interne Funktionen
    void processTask(const TaskData& task);
    void updateTaskProgress(const std::string& taskId, float progress);
    void completeTask(const std::string& taskId);
    void handleTaskError(const std::string& taskId, const std::string& error);
    void optimizeTaskExecution(const TaskData& task);
    void learnFromTask(const TaskData& task);
};

} // namespace VR_DAW 