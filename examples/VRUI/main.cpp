#include "ui/VRUI.hpp"
#include "vr/VRInteraction.hpp"
#include "vr/VRManager.hpp"
#include "core/Logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>

namespace VRMusicStudio {
namespace Examples {

class VRUIDemo {
public:
    VRUIDemo() {
        vrManager = std::make_unique<VR::VRManager>();
        vrInteraction = std::make_unique<VR::VRInteraction>(*vrManager);
        ui = std::make_unique<UI::UI>();
        vrUI = std::make_unique<UI::VRUI>(*vrManager, *vrInteraction, *ui);
    }

    bool initialize() {
        auto& logger = Core::Logger::getInstance();
        logger.info("Initialisiere VR-UI-Demo...");

        if (!vrManager->initialize()) {
            logger.error("Fehler bei der VR-Manager-Initialisierung");
            return false;
        }

        if (!vrInteraction->initialize()) {
            logger.error("Fehler bei der VR-Interaktions-Initialisierung");
            return false;
        }

        if (!ui->initialize()) {
            logger.error("Fehler bei der UI-Initialisierung");
            return false;
        }

        if (!vrUI->initialize()) {
            logger.error("Fehler bei der VR-UI-Initialisierung");
            return false;
        }

        logger.info("VR-UI-Demo erfolgreich initialisiert");
        return true;
    }

    void shutdown() {
        auto& logger = Core::Logger::getInstance();
        logger.info("Beende VR-UI-Demo...");

        vrUI->shutdown();
        ui->shutdown();
        vrInteraction->shutdown();
        vrManager->shutdown();

        logger.info("VR-UI-Demo erfolgreich beendet");
    }

    void run() {
        auto& logger = Core::Logger::getInstance();
        logger.info("Starte VR-UI-Demo...");

        // Hauptmenü erstellen
        auto mainMenu = vrUI->createPanel("main_menu", glm::vec3(0.0f, 1.6f, -1.0f), glm::vec2(0.4f, 0.6f));
        mainMenu->setColor(glm::vec4(0.2f, 0.2f, 0.2f, 0.9f));
        mainMenu->setText("Hauptmenü");
        mainMenu->setTextColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        // Buttons erstellen
        auto startButton = vrUI->createButton("start_button", glm::vec3(0.0f, 1.5f, -1.0f), glm::vec2(0.3f, 0.1f));
        startButton->setText("Start");
        startButton->setTextColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        startButton->setOnClick([&]() {
            logger.info("Start-Button geklickt");
        });

        auto optionsButton = vrUI->createButton("options_button", glm::vec3(0.0f, 1.3f, -1.0f), glm::vec2(0.3f, 0.1f));
        optionsButton->setText("Optionen");
        optionsButton->setTextColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        optionsButton->setOnClick([&]() {
            logger.info("Optionen-Button geklickt");
        });

        auto exitButton = vrUI->createButton("exit_button", glm::vec3(0.0f, 1.1f, -1.0f), glm::vec2(0.3f, 0.1f));
        exitButton->setText("Beenden");
        exitButton->setTextColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        exitButton->setOnClick([&]() {
            logger.info("Beenden-Button geklickt");
            shouldExit = true;
        });

        // Hauptschleife
        while (!shouldExit) {
            // Update VR-System
            vrInteraction->update();

            // Update UI
            vrUI->update();

            // Kurze Pause
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

private:
    std::unique_ptr<VR::VRManager> vrManager;
    std::unique_ptr<VR::VRInteraction> vrInteraction;
    std::unique_ptr<UI::UI> ui;
    std::unique_ptr<UI::VRUI> vrUI;
    bool shouldExit = false;
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
        VRMusicStudio::Examples::VRUIDemo demo;
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