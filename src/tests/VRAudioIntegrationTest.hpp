#pragma once

#include <gtest/gtest.h>
#include "../audio/AudioEngine.hpp"
#include "../audio/OpenALIntegration.hpp"
#include "../vr/VRManager.hpp"
#include "../core/Logger.hpp"
#include <memory>
#include <vector>
#include <string>

namespace VR_DAW {
namespace Tests {

class VRAudioIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialisiere Logger
        logger.initialize();
        
        // Initialisiere VR-Manager
        vrManager = std::make_shared<VRManager>();
        ASSERT_TRUE(vrManager->initialize());
        
        // Initialisiere Audio-Engine
        audioEngine = std::make_shared<AudioEngine>();
        ASSERT_TRUE(audioEngine->initialize());
        
        // Initialisiere OpenAL-Integration
        openAL = std::make_shared<OpenALIntegration>();
        ASSERT_TRUE(openAL->initialize());
    }
    
    void TearDown() override {
        // Beende OpenAL-Integration
        if (openAL) {
            openAL->shutdown();
        }
        
        // Beende Audio-Engine
        if (audioEngine) {
            audioEngine->shutdown();
        }
        
        // Beende VR-Manager
        if (vrManager) {
            vrManager->shutdown();
        }
        
        // Beende Logger
        logger.shutdown();
    }
    
    // Hilfsfunktionen für Tests
    bool createTestAudioSource(const std::string& name, const glm::vec3& position);
    bool createTestVRController(const std::string& name);
    void simulateVRMovement(const glm::vec3& position, const glm::quat& orientation);
    void simulateAudioPlayback(const std::string& sourceName);
    bool verifyAudioPosition(const std::string& sourceName, const glm::vec3& expectedPosition);
    bool verifyAudioVolume(const std::string& sourceName, float expectedVolume);
    
    std::shared_ptr<VRManager> vrManager;
    std::shared_ptr<AudioEngine> audioEngine;
    std::shared_ptr<OpenALIntegration> openAL;
    Logger logger;
    
    // Test-Daten
    std::vector<std::string> testAudioSources;
    std::vector<std::string> testVRControllers;
};

} // namespace Tests
} // namespace VR_DAW 