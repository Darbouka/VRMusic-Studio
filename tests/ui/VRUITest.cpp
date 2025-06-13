#include "ui/VRUI.hpp"
#include "ui/UI.hpp"
#include "vr/VRInteraction.hpp"
#include "vr/VRManager.hpp"
#include <gtest/gtest.h>
#include <memory>

namespace VRMusicStudio {
namespace Tests {

class VRUITest : public ::testing::Test {
protected:
    void SetUp() override {
        vrManager = std::make_unique<VR::VRManager>();
        ASSERT_TRUE(vrManager->initialize());
        vrInteraction = std::make_unique<VR::VRInteraction>(*vrManager);
        ASSERT_TRUE(vrInteraction->initialize());
        ui = std::make_unique<UI::UI>();
        ASSERT_TRUE(ui->initialize());
        vrUI = std::make_unique<UI::VRUI>(*ui, *vrInteraction);
        ASSERT_TRUE(vrUI->initialize());
    }

    void TearDown() override {
        vrUI->shutdown();
        ui->shutdown();
        vrInteraction->shutdown();
        vrManager->shutdown();
    }

    std::unique_ptr<VR::VRManager> vrManager;
    std::unique_ptr<VR::VRInteraction> vrInteraction;
    std::unique_ptr<UI::UI> ui;
    std::unique_ptr<UI::VRUI> vrUI;
};

TEST_F(VRUITest, Initialization) {
    EXPECT_TRUE(vrUI->initialize());
}

TEST_F(VRUITest, PanelCreation) {
    // Erstelle ein Panel
    vrUI->createPanel("testPanel", glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.5f, 0.5f));

    // Test Panel-Position
    vrUI->setPanelPosition("testPanel", glm::vec3(1.0f, 1.0f, -1.0f));
    glm::vec3 position = vrUI->getPanelPosition("testPanel");
    EXPECT_FLOAT_EQ(position.x, 1.0f);
    EXPECT_FLOAT_EQ(position.y, 1.0f);
    EXPECT_FLOAT_EQ(position.z, -1.0f);

    // Test Panel-Größe
    vrUI->setPanelSize("testPanel", glm::vec2(1.0f, 1.0f));
    glm::vec2 size = vrUI->getPanelSize("testPanel");
    EXPECT_FLOAT_EQ(size.x, 1.0f);
    EXPECT_FLOAT_EQ(size.y, 1.0f);

    // Lösche das Panel
    vrUI->destroyPanel("testPanel");
}

TEST_F(VRUITest, PanelVisibility) {
    // Erstelle ein Panel
    vrUI->createPanel("testPanel", glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.5f, 0.5f));

    // Test Sichtbarkeit
    vrUI->setPanelVisible("testPanel", false);
    EXPECT_FALSE(vrUI->isPanelVisible("testPanel"));

    vrUI->setPanelVisible("testPanel", true);
    EXPECT_TRUE(vrUI->isPanelVisible("testPanel"));

    // Lösche das Panel
    vrUI->destroyPanel("testPanel");
}

TEST_F(VRUITest, PanelInteraction) {
    // Erstelle ein Panel
    vrUI->createPanel("testPanel", glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.5f, 0.5f));

    // Test Interaktivität
    vrUI->setPanelInteractive("testPanel", false);
    EXPECT_FALSE(vrUI->isPanelInteractive("testPanel"));

    vrUI->setPanelInteractive("testPanel", true);
    EXPECT_TRUE(vrUI->isPanelInteractive("testPanel"));

    // Lösche das Panel
    vrUI->destroyPanel("testPanel");
}

TEST_F(VRUITest, PanelStyling) {
    // Erstelle ein Panel
    vrUI->createPanel("testPanel", glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.5f, 0.5f));

    // Test Farbe
    vrUI->setPanelColor("testPanel", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    glm::vec4 color = vrUI->getPanelColor("testPanel");
    EXPECT_FLOAT_EQ(color.r, 1.0f);
    EXPECT_FLOAT_EQ(color.g, 0.0f);
    EXPECT_FLOAT_EQ(color.b, 0.0f);
    EXPECT_FLOAT_EQ(color.a, 1.0f);

    // Test Textur
    vrUI->setPanelTexture("testPanel", "textures/panel.png");
    std::string texture = vrUI->getPanelTexture("testPanel");
    EXPECT_EQ(texture, "textures/panel.png");

    // Test Schriftart
    vrUI->setPanelFont("testPanel", "fonts/arial.ttf", 0.02f);
    std::string font = vrUI->getPanelFont("testPanel");
    float fontSize = vrUI->getPanelFontSize("testPanel");
    EXPECT_EQ(font, "fonts/arial.ttf");
    EXPECT_FLOAT_EQ(fontSize, 0.02f);

    // Test Text
    vrUI->setPanelText("testPanel", "Test Text");
    std::string text = vrUI->getPanelText("testPanel");
    EXPECT_EQ(text, "Test Text");

    // Test Textfarbe
    vrUI->setPanelTextColor("testPanel", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    glm::vec4 textColor = vrUI->getPanelTextColor("testPanel");
    EXPECT_FLOAT_EQ(textColor.r, 0.0f);
    EXPECT_FLOAT_EQ(textColor.g, 1.0f);
    EXPECT_FLOAT_EQ(textColor.b, 0.0f);
    EXPECT_FLOAT_EQ(textColor.a, 1.0f);

    // Lösche das Panel
    vrUI->destroyPanel("testPanel");
}

TEST_F(VRUITest, PanelHover) {
    // Erstelle ein Panel
    vrUI->createPanel("testPanel", glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.5f, 0.5f));

    // Setze Controller-Position über dem Panel
    vrInteraction->setDeviceTransform(0, glm::vec3(0.0f, 0.0f, -0.5f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

    // Aktualisiere UI
    vrUI->update();

    // Test Hover-Status
    EXPECT_TRUE(vrUI->isPanelHovered("testPanel"));

    // Setze Controller-Position außerhalb des Panels
    vrInteraction->setDeviceTransform(0, glm::vec3(1.0f, 1.0f, -0.5f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

    // Aktualisiere UI
    vrUI->update();

    // Test Hover-Status
    EXPECT_FALSE(vrUI->isPanelHovered("testPanel"));

    // Lösche das Panel
    vrUI->destroyPanel("testPanel");
}

TEST_F(VRUITest, PanelClick) {
    // Erstelle ein Panel
    vrUI->createPanel("testPanel", glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.5f, 0.5f));

    // Setze Controller-Position über dem Panel
    vrInteraction->setDeviceTransform(0, glm::vec3(0.0f, 0.0f, -0.5f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

    // Simuliere Button-Druck
    vrInteraction->setControllerButtonPressed(0, vr::k_EButton_SteamVR_Trigger, true);

    // Aktualisiere UI
    vrUI->update();

    // Test Click-Status
    EXPECT_TRUE(vrUI->isPanelClicked("testPanel"));

    // Simuliere Button-Release
    vrInteraction->setControllerButtonPressed(0, vr::k_EButton_SteamVR_Trigger, false);

    // Aktualisiere UI
    vrUI->update();

    // Test Click-Status
    EXPECT_FALSE(vrUI->isPanelClicked("testPanel"));

    // Lösche das Panel
    vrUI->destroyPanel("testPanel");
}

TEST_F(VRUITest, PanelCursorPosition) {
    // Erstelle ein Panel
    vrUI->createPanel("testPanel", glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.5f, 0.5f));

    // Setze Controller-Position über dem Panel
    vrInteraction->setDeviceTransform(0, glm::vec3(0.25f, 0.25f, -0.5f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

    // Aktualisiere UI
    vrUI->update();

    // Test Cursor-Position
    glm::vec2 cursorPos = vrUI->getPanelCursorPosition("testPanel");
    EXPECT_FLOAT_EQ(cursorPos.x, 0.75f);
    EXPECT_FLOAT_EQ(cursorPos.y, 0.75f);

    // Lösche das Panel
    vrUI->destroyPanel("testPanel");
}

} // namespace Tests
} // namespace VRMusicStudio 