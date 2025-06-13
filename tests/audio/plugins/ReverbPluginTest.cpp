#include "audio/plugins/ReverbPlugin.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <cmath>

namespace VRMusicStudio {
namespace Tests {

class ReverbPluginTest : public ::testing::Test {
protected:
    void SetUp() override {
        plugin = std::make_unique<Audio::ReverbPlugin>();
        ASSERT_TRUE(plugin->initialize());
    }

    void TearDown() override {
        plugin->shutdown();
    }

    std::unique_ptr<Audio::ReverbPlugin> plugin;
    static constexpr size_t BUFFER_SIZE = 1024;
    std::vector<float> inputBuffer;
    std::vector<float> outputBuffer;
};

TEST_F(ReverbPluginTest, Initialization) {
    EXPECT_TRUE(plugin->initialize());
    EXPECT_FLOAT_EQ(plugin->getRoomSize(), 0.5f);
    EXPECT_FLOAT_EQ(plugin->getDamping(), 0.5f);
    EXPECT_FLOAT_EQ(plugin->getWetLevel(), 0.33f);
    EXPECT_FLOAT_EQ(plugin->getDryLevel(), 0.4f);
    EXPECT_FLOAT_EQ(plugin->getWidth(), 1.0f);
    EXPECT_FALSE(plugin->isFrozen());
}

TEST_F(ReverbPluginTest, ParameterLimits) {
    // Test RoomSize
    plugin->setRoomSize(-1.0f);
    EXPECT_FLOAT_EQ(plugin->getRoomSize(), 0.0f);
    plugin->setRoomSize(2.0f);
    EXPECT_FLOAT_EQ(plugin->getRoomSize(), 1.0f);

    // Test Damping
    plugin->setDamping(-1.0f);
    EXPECT_FLOAT_EQ(plugin->getDamping(), 0.0f);
    plugin->setDamping(2.0f);
    EXPECT_FLOAT_EQ(plugin->getDamping(), 1.0f);

    // Test WetLevel
    plugin->setWetLevel(-1.0f);
    EXPECT_FLOAT_EQ(plugin->getWetLevel(), 0.0f);
    plugin->setWetLevel(2.0f);
    EXPECT_FLOAT_EQ(plugin->getWetLevel(), 1.0f);

    // Test DryLevel
    plugin->setDryLevel(-1.0f);
    EXPECT_FLOAT_EQ(plugin->getDryLevel(), 0.0f);
    plugin->setDryLevel(2.0f);
    EXPECT_FLOAT_EQ(plugin->getDryLevel(), 1.0f);

    // Test Width
    plugin->setWidth(-1.0f);
    EXPECT_FLOAT_EQ(plugin->getWidth(), 0.0f);
    plugin->setWidth(2.0f);
    EXPECT_FLOAT_EQ(plugin->getWidth(), 1.0f);
}

TEST_F(ReverbPluginTest, ProcessSilence) {
    inputBuffer.resize(BUFFER_SIZE, 0.0f);
    outputBuffer.resize(BUFFER_SIZE, 0.0f);

    plugin->process(inputBuffer.data(), outputBuffer.data(), BUFFER_SIZE);

    // Bei Stille sollte der Output auch still sein
    for (size_t i = 0; i < BUFFER_SIZE; ++i) {
        EXPECT_FLOAT_EQ(outputBuffer[i], 0.0f);
    }
}

TEST_F(ReverbPluginTest, ProcessImpulse) {
    inputBuffer.resize(BUFFER_SIZE, 0.0f);
    outputBuffer.resize(BUFFER_SIZE, 0.0f);

    // Erzeuge einen Impuls
    inputBuffer[0] = 1.0f;

    plugin->setRoomSize(0.8f);
    plugin->setDamping(0.5f);
    plugin->setWetLevel(0.5f);
    plugin->setDryLevel(0.5f);

    plugin->process(inputBuffer.data(), outputBuffer.data(), BUFFER_SIZE);

    // Der Output sollte den Impuls enthalten
    EXPECT_FLOAT_EQ(outputBuffer[0], 0.5f); // Trockenes Signal

    // Der Output sollte einen Hall-Effekt zeigen
    bool hasReverb = false;
    for (size_t i = 1; i < BUFFER_SIZE; ++i) {
        if (std::abs(outputBuffer[i]) > 0.001f) {
            hasReverb = true;
            break;
        }
    }
    EXPECT_TRUE(hasReverb);
}

TEST_F(ReverbPluginTest, FreezeMode) {
    inputBuffer.resize(BUFFER_SIZE, 0.0f);
    outputBuffer.resize(BUFFER_SIZE, 0.0f);

    // Erzeuge einen Impuls
    inputBuffer[0] = 1.0f;

    // Aktiviere Freeze-Modus
    plugin->setFreeze(true);
    plugin->setRoomSize(0.8f);
    plugin->setDamping(0.5f);
    plugin->setWetLevel(0.5f);
    plugin->setDryLevel(0.5f);

    // Verarbeite den Impuls
    plugin->process(inputBuffer.data(), outputBuffer.data(), BUFFER_SIZE);

    // Setze Input auf 0
    std::fill(inputBuffer.begin(), inputBuffer.end(), 0.0f);

    // Verarbeite Stille
    plugin->process(inputBuffer.data(), outputBuffer.data(), BUFFER_SIZE);

    // Im Freeze-Modus sollte der Hall weiterhin hörbar sein
    bool hasReverb = false;
    for (size_t i = 0; i < BUFFER_SIZE; ++i) {
        if (std::abs(outputBuffer[i]) > 0.001f) {
            hasReverb = true;
            break;
        }
    }
    EXPECT_TRUE(hasReverb);
}

TEST_F(ReverbPluginTest, ParameterChanges) {
    inputBuffer.resize(BUFFER_SIZE, 0.0f);
    outputBuffer.resize(BUFFER_SIZE, 0.0f);

    // Erzeuge einen Impuls
    inputBuffer[0] = 1.0f;

    // Erste Verarbeitung mit Standard-Parametern
    plugin->process(inputBuffer.data(), outputBuffer.data(), BUFFER_SIZE);
    float firstOutput = outputBuffer[0];

    // Ändere Parameter
    plugin->setRoomSize(0.9f);
    plugin->setDamping(0.7f);
    plugin->setWetLevel(0.8f);
    plugin->setDryLevel(0.2f);

    // Zweite Verarbeitung
    plugin->process(inputBuffer.data(), outputBuffer.data(), BUFFER_SIZE);
    float secondOutput = outputBuffer[0];

    // Die Ausgaben sollten sich unterscheiden
    EXPECT_NE(firstOutput, secondOutput);
}

} // namespace Tests
} // namespace VRMusicStudio 