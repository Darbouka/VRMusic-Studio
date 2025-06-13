#include "audio/plugins/DelayPlugin.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <cmath>

namespace VRMusicStudio {
namespace Tests {

class DelayPluginTest : public ::testing::Test {
protected:
    void SetUp() override {
        plugin = std::make_unique<Audio::DelayPlugin>();
        ASSERT_TRUE(plugin->initialize());
    }

    void TearDown() override {
        plugin->shutdown();
    }

    std::unique_ptr<Audio::DelayPlugin> plugin;
    static constexpr size_t BUFFER_SIZE = 1024;
    std::vector<float> inputBuffer;
    std::vector<float> outputBuffer;
};

TEST_F(DelayPluginTest, Initialization) {
    EXPECT_TRUE(plugin->initialize());
    EXPECT_FLOAT_EQ(plugin->getDelayTime(), 0.5f);
    EXPECT_FLOAT_EQ(plugin->getFeedback(), 0.3f);
    EXPECT_FLOAT_EQ(plugin->getMix(), 0.5f);
    EXPECT_FLOAT_EQ(plugin->getLowPassCutoff(), 20000.0f);
    EXPECT_FLOAT_EQ(plugin->getHighPassCutoff(), 20.0f);
    EXPECT_FALSE(plugin->isPingPongEnabled());
}

TEST_F(DelayPluginTest, ParameterLimits) {
    // Test DelayTime
    plugin->setDelayTime(-1.0f);
    EXPECT_FLOAT_EQ(plugin->getDelayTime(), 0.0f);
    plugin->setDelayTime(3.0f);
    EXPECT_FLOAT_EQ(plugin->getDelayTime(), 2.0f);

    // Test Feedback
    plugin->setFeedback(-1.0f);
    EXPECT_FLOAT_EQ(plugin->getFeedback(), 0.0f);
    plugin->setFeedback(2.0f);
    EXPECT_FLOAT_EQ(plugin->getFeedback(), 0.99f);

    // Test Mix
    plugin->setMix(-1.0f);
    EXPECT_FLOAT_EQ(plugin->getMix(), 0.0f);
    plugin->setMix(2.0f);
    EXPECT_FLOAT_EQ(plugin->getMix(), 1.0f);

    // Test LowPassCutoff
    plugin->setLowPassCutoff(10.0f);
    EXPECT_FLOAT_EQ(plugin->getLowPassCutoff(), 20.0f);
    plugin->setLowPassCutoff(30000.0f);
    EXPECT_FLOAT_EQ(plugin->getLowPassCutoff(), 20000.0f);

    // Test HighPassCutoff
    plugin->setHighPassCutoff(10.0f);
    EXPECT_FLOAT_EQ(plugin->getHighPassCutoff(), 20.0f);
    plugin->setHighPassCutoff(30000.0f);
    EXPECT_FLOAT_EQ(plugin->getHighPassCutoff(), 20000.0f);
}

TEST_F(DelayPluginTest, ProcessSilence) {
    inputBuffer.resize(BUFFER_SIZE, 0.0f);
    outputBuffer.resize(BUFFER_SIZE, 0.0f);

    plugin->process(inputBuffer.data(), outputBuffer.data(), BUFFER_SIZE);

    // Bei Stille sollte der Output auch still sein
    for (size_t i = 0; i < BUFFER_SIZE; ++i) {
        EXPECT_FLOAT_EQ(outputBuffer[i], 0.0f);
    }
}

TEST_F(DelayPluginTest, ProcessImpulse) {
    inputBuffer.resize(BUFFER_SIZE, 0.0f);
    outputBuffer.resize(BUFFER_SIZE, 0.0f);

    // Erzeuge einen Impuls
    inputBuffer[0] = 1.0f;

    plugin->setDelayTime(0.1f);
    plugin->setFeedback(0.5f);
    plugin->setMix(0.5f);

    plugin->process(inputBuffer.data(), outputBuffer.data(), BUFFER_SIZE);

    // Der Output sollte den Impuls enthalten
    EXPECT_FLOAT_EQ(outputBuffer[0], 0.5f); // Direktes Signal

    // Der Output sollte verzögerte Echos zeigen
    size_t delaySamples = static_cast<size_t>(0.1f * 44100.0f); // Beispiel-Samplerate
    bool hasDelay = false;
    for (size_t i = delaySamples; i < BUFFER_SIZE; ++i) {
        if (std::abs(outputBuffer[i]) > 0.001f) {
            hasDelay = true;
            break;
        }
    }
    EXPECT_TRUE(hasDelay);
}

TEST_F(DelayPluginTest, PingPongMode) {
    inputBuffer.resize(BUFFER_SIZE * 2, 0.0f); // Stereo
    outputBuffer.resize(BUFFER_SIZE * 2, 0.0f);

    // Erzeuge einen Impuls
    inputBuffer[0] = 1.0f;
    inputBuffer[1] = 1.0f;

    plugin->setDelayTime(0.1f);
    plugin->setFeedback(0.5f);
    plugin->setMix(0.5f);
    plugin->setPingPong(true);

    plugin->process(inputBuffer.data(), outputBuffer.data(), BUFFER_SIZE);

    // Der Output sollte den Impuls enthalten
    EXPECT_FLOAT_EQ(outputBuffer[0], 0.5f); // Linker Kanal
    EXPECT_FLOAT_EQ(outputBuffer[1], 0.5f); // Rechter Kanal

    // Die verzögerten Echos sollten zwischen den Kanälen wechseln
    size_t delaySamples = static_cast<size_t>(0.1f * 44100.0f);
    bool hasPingPong = false;
    for (size_t i = delaySamples; i < BUFFER_SIZE; i += 2) {
        if (std::abs(outputBuffer[i] - outputBuffer[i + 1]) > 0.001f) {
            hasPingPong = true;
            break;
        }
    }
    EXPECT_TRUE(hasPingPong);
}

TEST_F(DelayPluginTest, FilterEffects) {
    inputBuffer.resize(BUFFER_SIZE, 0.0f);
    outputBuffer.resize(BUFFER_SIZE, 0.0f);

    // Erzeuge einen Impuls
    inputBuffer[0] = 1.0f;

    // Erste Verarbeitung ohne Filter
    plugin->setLowPassCutoff(20000.0f);
    plugin->setHighPassCutoff(20.0f);
    plugin->process(inputBuffer.data(), outputBuffer.data(), BUFFER_SIZE);
    float unfilteredOutput = outputBuffer[0];

    // Zweite Verarbeitung mit Filter
    plugin->setLowPassCutoff(1000.0f);
    plugin->setHighPassCutoff(500.0f);
    plugin->process(inputBuffer.data(), outputBuffer.data(), BUFFER_SIZE);
    float filteredOutput = outputBuffer[0];

    // Die Ausgaben sollten sich unterscheiden
    EXPECT_NE(unfilteredOutput, filteredOutput);
}

TEST_F(DelayPluginTest, ParameterChanges) {
    inputBuffer.resize(BUFFER_SIZE, 0.0f);
    outputBuffer.resize(BUFFER_SIZE, 0.0f);

    // Erzeuge einen Impuls
    inputBuffer[0] = 1.0f;

    // Erste Verarbeitung mit Standard-Parametern
    plugin->process(inputBuffer.data(), outputBuffer.data(), BUFFER_SIZE);
    float firstOutput = outputBuffer[0];

    // Ändere Parameter
    plugin->setDelayTime(0.2f);
    plugin->setFeedback(0.7f);
    plugin->setMix(0.8f);
    plugin->setPingPong(true);

    // Zweite Verarbeitung
    plugin->process(inputBuffer.data(), outputBuffer.data(), BUFFER_SIZE);
    float secondOutput = outputBuffer[0];

    // Die Ausgaben sollten sich unterscheiden
    EXPECT_NE(firstOutput, secondOutput);
}

} // namespace Tests
} // namespace VRMusicStudio 