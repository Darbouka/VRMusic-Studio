#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include "../core/Logger.hpp"
#include "../core/EventSystem.hpp"
#include "../audio/AudioEngine.hpp"

namespace VR_DAW {

struct VoiceProfile {
    std::string id;
    std::string name;
    std::string originalActor;  // Original-Schauspieler
    std::string germanVoice;    // Deutsche Synchronstimme
    std::vector<float> characteristics;
    float pitch;
    float speed;
    float emotion;
    std::map<std::string, float> parameters;
    std::vector<std::string> knownPhrases;  // Bekannte Phrasen des Schauspielers
    std::map<std::string, float> voiceModifiers;  // Stimmenmodifikatoren
};

struct AIConversation {
    std::string id;
    std::vector<std::string> messages;
    std::string currentContext;
    std::map<std::string, float> emotions;
    std::chrono::system_clock::time_point lastUpdate;
    std::string activeVoiceProfile;  // Aktives Voice-Profil
    std::vector<std::string> conversationHistory;  // Gesprächsverlauf
};

class AIIntegration {
public:
    static AIIntegration& getInstance();

    bool initialize();
    void shutdown();
    bool isInitialized() const;

    // Sprachverarbeitung
    bool startVoiceRecognition();
    void stopVoiceRecognition();
    bool isVoiceRecognitionActive() const;
    
    // Sprachsynthese
    bool synthesizeSpeech(const std::string& text, const std::string& voiceId);
    void stopSpeechSynthesis();
    bool isSpeechSynthesisActive() const;
    
    // Chatbot
    void processUserInput(const std::string& input);
    std::string getBotResponse();
    void setConversationContext(const std::string& context);
    
    // Voice Profiles
    bool createVoiceProfile(const std::string& name, 
                          const std::string& originalActor,
                          const std::string& germanVoice,
                          const std::vector<float>& characteristics);
    bool modifyVoiceProfile(const std::string& voiceId, 
                           const std::map<std::string, float>& parameters);
    std::shared_ptr<VoiceProfile> getVoiceProfile(const std::string& voiceId);
    bool importVoiceProfile(const std::string& filePath);
    bool exportVoiceProfile(const std::string& voiceId, const std::string& filePath);
    
    // Synchronstimmen-Management
    bool loadSynchronizationDatabase(const std::string& filePath);
    bool addSynchronizationEntry(const std::string& originalActor, 
                               const std::string& germanVoice,
                               const std::vector<std::string>& knownPhrases);
    std::vector<std::string> getAvailableGermanVoices() const;
    std::vector<std::string> getAvailableOriginalActors() const;
    
    // Vocoder
    bool applyVocoder(const std::string& inputVoiceId, const std::string& carrierVoiceId);
    void setVocoderParameters(const std::map<std::string, float>& parameters);
    bool applyVoiceModification(const std::string& voiceId, 
                               const std::map<std::string, float>& modifiers);
    
    // Emotion Analysis
    std::map<std::string, float> analyzeEmotions(const std::string& text);
    void setEmotionThresholds(const std::map<std::string, float>& thresholds);
    bool applyEmotionalVoiceModification(const std::string& voiceId, 
                                       const std::map<std::string, float>& emotions);
    
    // Synchronization
    bool synchronizeVoices(const std::vector<std::string>& voiceIds);
    void setSyncParameters(const std::map<std::string, float>& parameters);
    bool createVoiceEnsemble(const std::vector<std::string>& voiceIds);
    
    // Voice Training
    bool trainVoiceModel(const std::string& voiceId, 
                        const std::vector<std::string>& trainingData);
    bool updateVoiceCharacteristics(const std::string& voiceId, 
                                  const std::vector<float>& newCharacteristics);
    
    // Event Handling
    void registerVoiceCallback(std::function<void(const std::string&)> callback);
    void registerEmotionCallback(std::function<void(const std::map<std::string, float>&)> callback);
    void registerVoiceProfileCallback(std::function<void(const std::string&)> callback);

private:
    AIIntegration();
    ~AIIntegration();
    
    void processVoiceInput();
    void updateConversation();
    void generateResponse();
    void applyVoiceEffects();
    void updateEmotions();
    void loadDefaultVoiceProfiles();
    void initializeSynchronizationDatabase();
    bool validateVoiceProfile(const VoiceProfile& profile);
    void updateVoiceModifiers(const std::string& voiceId);
    
    std::mutex mutex;
    std::atomic<bool> initialized;
    std::atomic<bool> voiceRecognitionActive;
    std::atomic<bool> speechSynthesisActive;
    
    Logger logger;
    std::shared_ptr<AudioEngine> audioEngine;
    
    std::map<std::string, std::shared_ptr<VoiceProfile>> voiceProfiles;
    std::map<std::string, AIConversation> conversations;
    std::queue<std::string> inputQueue;
    std::queue<std::string> responseQueue;
    
    std::map<std::string, float> emotionThresholds;
    std::map<std::string, float> vocoderParameters;
    std::map<std::string, float> syncParameters;
    
    std::vector<std::function<void(const std::string&)>> voiceCallbacks;
    std::vector<std::function<void(const std::map<std::string, float>&)>> emotionCallbacks;
    std::vector<std::function<void(const std::string&)>> voiceProfileCallbacks;
    
    std::thread voiceProcessingThread;
    std::thread conversationThread;
    std::atomic<bool> shouldStop;
    
    // Synchronisationsdatenbank
    std::map<std::string, std::string> actorToGermanVoice;
    std::map<std::string, std::vector<std::string>> voicePhrases;
    std::map<std::string, std::map<std::string, float>> voiceCharacteristics;
};

} // namespace VR_DAW 