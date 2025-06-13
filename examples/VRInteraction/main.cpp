#include "vr/VRInteraction.hpp"
#include "vr/VRManager.hpp"
#include "core/Logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>

namespace VRMusicStudio {
namespace Examples {

class VRInteractionDemo {
public:
    VRInteractionDemo() {
        vrManager = std::make_unique<VR::VRManager>();
        vrInteraction = std::make_unique<VR::VRInteraction>(*vrManager);
    }

    bool initialize() {
        auto& logger = Core::Logger::getInstance();
        logger.info("Initialisiere VR-Interaktions-Demo...");

        if (!vrManager->initialize()) {
            logger.error("Fehler bei der VR-Manager-Initialisierung");
            return false;
        }

        if (!vrInteraction->initialize()) {
            logger.error("Fehler bei der VR-Interaktions-Initialisierung");
            return false;
        }

        logger.info("VR-Interaktions-Demo erfolgreich initialisiert");
        return true;
    }

    void shutdown() {
        auto& logger = Core::Logger::getInstance();
        logger.info("Beende VR-Interaktions-Demo...");

        vrInteraction->shutdown();
        vrManager->shutdown();

        logger.info("VR-Interaktions-Demo erfolgreich beendet");
    }

    void run() {
        auto& logger = Core::Logger::getInstance();
        logger.info("Starte VR-Interaktions-Demo...");

        // Hauptschleife
        while (true) {
            // Update VR-System
            vrInteraction->update();

            // Controller-Status ausgeben
            for (uint32_t i = 0; i < 2; ++i) {
                if (vrInteraction->isDeviceConnected(i)) {
                    glm::vec3 position = vrInteraction->getControllerPosition(i);
                    glm::quat rotation = vrInteraction->getControllerRotation(i);

                    std::cout << "Controller " << i << ":" << std::endl;
                    std::cout << "  Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
                    std::cout << "  Rotation: (" << rotation.w << ", " << rotation.x << ", " << rotation.y << ", " << rotation.z << ")" << std::endl;

                    // Button-Status
                    if (vrInteraction->isControllerButtonPressed(i, vr::k_EButton_SteamVR_Trigger)) {
                        std::cout << "  Trigger gedrückt" << std::endl;
                        // Haptisches Feedback
                        vrInteraction->triggerHapticPulse(i, 0.1f, 100.0f, 0.5f);
                    }

                    if (vrInteraction->isControllerButtonPressed(i, vr::k_EButton_Grip)) {
                        std::cout << "  Grip gedrückt" << std::endl;
                    }

                    // Trackpad
                    glm::vec2 axis = vrInteraction->getControllerAxis(i, vr::k_EAxis_Trackpad);
                    if (std::abs(axis.x) > 0.1f || std::abs(axis.y) > 0.1f) {
                        std::cout << "  Trackpad: (" << axis.x << ", " << axis.y << ")" << std::endl;
                    }
                }
            }

            // HMD-Status ausgeben
            glm::vec3 hmdPosition = vrInteraction->getHMDPosition();
            glm::quat hmdRotation = vrInteraction->getHMDRotation();

            std::cout << "HMD:" << std::endl;
            std::cout << "  Position: (" << hmdPosition.x << ", " << hmdPosition.y << ", " << hmdPosition.z << ")" << std::endl;
            std::cout << "  Rotation: (" << hmdRotation.w << ", " << hmdRotation.x << ", " << hmdRotation.y << ", " << hmdRotation.z << ")" << std::endl;

            // PlayArea-Status
            glm::vec2 playAreaSize = vrInteraction->getPlayAreaSize();
            std::cout << "PlayArea: " << playAreaSize.x << "m x " << playAreaSize.y << "m" << std::endl;

            // Prüfe Position im PlayArea
            if (vrInteraction->isPositionInPlayArea(hmdPosition)) {
                std::cout << "HMD ist im PlayArea" << std::endl;
            } else {
                std::cout << "WARNUNG: HMD ist außerhalb des PlayArea!" << std::endl;
            }

            // Kurze Pause
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // Konsolenausgabe löschen
            std::cout << "\033[2J\033[1;1H";
        }
    }

private:
    std::unique_ptr<VR::VRManager> vrManager;
    std::unique_ptr<VR::VRInteraction> vrInteraction;
};

} // namespace Examples
} // namespace VRMusicStudio

int main() {
    try {
        // Logger initialisieren
        auto& logger = VRMusicStudio::Core::Logger::getInstance();
        if (!logger.initialize()) {
            std::cerr << "Fehler bei der Logger-Initialisierung" << std::endl;
            return 1;
        }

        // Demo erstellen und starten
        VRMusicStudio::Examples::VRInteractionDemo demo;
        if (!demo.initialize()) {
            std::cerr << "Fehler bei der Demo-Initialisierung" << std::endl;
            return 1;
        }

        demo.run();

        // Aufräumen
        demo.shutdown();
        logger.shutdown();

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fehler: " << e.what() << std::endl;
        return 1;
    }
} 