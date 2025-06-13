#include "vr/VRInteraction.hpp"
#include "vr/VRManager.hpp"
#include <gtest/gtest.h>
#include <memory>

namespace VRMusicStudio {
namespace Tests {

class VRInteractionTest : public ::testing::Test {
protected:
    void SetUp() override {
        vrManager = std::make_unique<VR::VRManager>();
        ASSERT_TRUE(vrManager->initialize());
        vrInteraction = std::make_unique<VR::VRInteraction>(*vrManager);
        ASSERT_TRUE(vrInteraction->initialize());
    }

    void TearDown() override {
        vrInteraction->shutdown();
        vrManager->shutdown();
    }

    std::unique_ptr<VR::VRManager> vrManager;
    std::unique_ptr<VR::VRInteraction> vrInteraction;
};

TEST_F(VRInteractionTest, Initialization) {
    EXPECT_TRUE(vrInteraction->initialize());
}

TEST_F(VRInteractionTest, ControllerTracking) {
    // Test Controller-Position
    glm::vec3 position = vrInteraction->getControllerPosition(0);
    EXPECT_FLOAT_EQ(position.x, 0.0f);
    EXPECT_FLOAT_EQ(position.y, 0.0f);
    EXPECT_FLOAT_EQ(position.z, 0.0f);

    // Test Controller-Rotation
    glm::quat rotation = vrInteraction->getControllerRotation(0);
    EXPECT_FLOAT_EQ(rotation.w, 1.0f);
    EXPECT_FLOAT_EQ(rotation.x, 0.0f);
    EXPECT_FLOAT_EQ(rotation.y, 0.0f);
    EXPECT_FLOAT_EQ(rotation.z, 0.0f);
}

TEST_F(VRInteractionTest, HMDTracking) {
    // Test HMD-Position
    glm::vec3 position = vrInteraction->getHMDPosition();
    EXPECT_FLOAT_EQ(position.x, 0.0f);
    EXPECT_FLOAT_EQ(position.y, 0.0f);
    EXPECT_FLOAT_EQ(position.z, 0.0f);

    // Test HMD-Rotation
    glm::quat rotation = vrInteraction->getHMDRotation();
    EXPECT_FLOAT_EQ(rotation.w, 1.0f);
    EXPECT_FLOAT_EQ(rotation.x, 0.0f);
    EXPECT_FLOAT_EQ(rotation.y, 0.0f);
    EXPECT_FLOAT_EQ(rotation.z, 0.0f);
}

TEST_F(VRInteractionTest, IPD) {
    // Test IPD-Get/Set
    float ipd = vrInteraction->getIPD();
    EXPECT_FLOAT_EQ(ipd, 0.064f);

    vrInteraction->setIPD(0.068f);
    ipd = vrInteraction->getIPD();
    EXPECT_FLOAT_EQ(ipd, 0.068f);

    // Test IPD-Limits
    vrInteraction->setIPD(0.04f);
    ipd = vrInteraction->getIPD();
    EXPECT_FLOAT_EQ(ipd, 0.05f);

    vrInteraction->setIPD(0.09f);
    ipd = vrInteraction->getIPD();
    EXPECT_FLOAT_EQ(ipd, 0.08f);
}

TEST_F(VRInteractionTest, PlayArea) {
    // Test PlayArea-Größe
    vrInteraction->setPlayAreaSize(3.0f, 4.0f);
    glm::vec2 size = vrInteraction->getPlayAreaSize();
    EXPECT_FLOAT_EQ(size.x, 3.0f);
    EXPECT_FLOAT_EQ(size.y, 4.0f);

    // Test Position im PlayArea
    glm::vec3 position(1.0f, 0.0f, 1.0f);
    EXPECT_TRUE(vrInteraction->isPositionInPlayArea(position));

    position = glm::vec3(2.0f, 0.0f, 3.0f);
    EXPECT_FALSE(vrInteraction->isPositionInPlayArea(position));
}

TEST_F(VRInteractionTest, Chaperone) {
    // Test Chaperone-Sichtbarkeit
    vrInteraction->setChaperoneVisibility(false);
    EXPECT_FALSE(vrInteraction->isChaperoneVisible());

    vrInteraction->setChaperoneVisibility(true);
    EXPECT_TRUE(vrInteraction->isChaperoneVisible());
}

TEST_F(VRInteractionTest, HapticFeedback) {
    // Test Haptisches Feedback
    vrInteraction->triggerHapticPulse(0, 0.1f, 100.0f, 0.5f);
    // Warte kurz
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    vrInteraction->stopHapticPulse(0);
}

TEST_F(VRInteractionTest, DeviceTransform) {
    // Test Device-Transform
    glm::vec3 position(1.0f, 2.0f, 3.0f);
    glm::quat rotation(0.707f, 0.0f, 0.707f, 0.0f);

    vrInteraction->setDeviceTransform(0, position, rotation);

    glm::vec3 newPosition = vrInteraction->getDevicePosition(0);
    EXPECT_FLOAT_EQ(newPosition.x, position.x);
    EXPECT_FLOAT_EQ(newPosition.y, position.y);
    EXPECT_FLOAT_EQ(newPosition.z, position.z);

    glm::quat newRotation = vrInteraction->getDeviceRotation(0);
    EXPECT_FLOAT_EQ(newRotation.w, rotation.w);
    EXPECT_FLOAT_EQ(newRotation.x, rotation.x);
    EXPECT_FLOAT_EQ(newRotation.y, rotation.y);
    EXPECT_FLOAT_EQ(newRotation.z, rotation.z);
}

} // namespace Tests
} // namespace VRMusicStudio 