#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "core/ConfigManager.hpp"
#include "core/ResourceManager.hpp"
#include "core/PluginManager.hpp"
#include "audio/AudioEngine.hpp"
#include "vr/VRManager.hpp"
#include "ui/UI.hpp"
#include "sequencer/Sequencer.hpp"
#include "midi/MIDIManager.hpp"
#include "network/NetworkManager.hpp"
#include "plugins/PluginHost.hpp"
#include "automation/AutomationManager.hpp"
#include "ai/AIManager.hpp"
#include "ki/KIManager.hpp"
#include "integration/IntegrationManager.hpp"
#include "compatibility/CompatibilityManager.hpp"

#include <spdlog/spdlog.h>
#include <memory>
#include <stdexcept>

using namespace VRMusicStudio;

int main(int argc, char* argv[]) {
    try {
        // Initialisiere Logger
        auto& logger = Core::Logger::getInstance();
        logger.info("Starte VRMusicStudio v{}", "0.1.0");

        // Initialisiere Konfiguration
        auto& config = Core::ConfigManager::getInstance();
        if (!config.initialize()) {
            logger.error("Fehler bei der Konfigurationsinitialisierung");
            return 1;
        }

        // Initialisiere Ressourcen-Manager
        auto& resources = Core::ResourceManager::getInstance();
        if (!resources.initialize()) {
            logger.error("Fehler bei der Ressourceninitialisierung");
            return 1;
        }

        // Initialisiere Plugin-Manager
        auto& plugins = Core::PluginManager::getInstance();
        if (!plugins.initialize()) {
            logger.error("Fehler bei der Plugin-Initialisierung");
            return 1;
        }

        // Initialisiere Audio-Engine
        auto& audio = Audio::AudioEngine::getInstance();
        if (!audio.initialize()) {
            logger.error("Fehler bei der Audio-Engine-Initialisierung");
            return 1;
        }

        // Initialisiere VR-Manager
        auto& vr = VR::VRManager::getInstance();
        if (!vr.initialize()) {
            logger.error("Fehler bei der VR-Initialisierung");
            return 1;
        }

        // Initialisiere UI
        auto& ui = UI::UI::getInstance();
        if (!ui.initialize()) {
            logger.error("Fehler bei der UI-Initialisierung");
            return 1;
        }

        // Initialisiere Sequencer
        auto& sequencer = Sequencer::Sequencer::getInstance();
        if (!sequencer.initialize()) {
            logger.error("Fehler bei der Sequencer-Initialisierung");
            return 1;
        }

        // Initialisiere MIDI-Manager
        auto& midi = MIDI::MIDIManager::getInstance();
        if (!midi.initialize()) {
            logger.error("Fehler bei der MIDI-Initialisierung");
            return 1;
        }

        // Initialisiere Netzwerk-Manager
        auto& network = Network::NetworkManager::getInstance();
        if (!network.initialize()) {
            logger.error("Fehler bei der Netzwerk-Initialisierung");
            return 1;
        }

        // Initialisiere Plugin-Host
        auto& pluginHost = Plugins::PluginHost::getInstance();
        if (!pluginHost.initialize()) {
            logger.error("Fehler bei der Plugin-Host-Initialisierung");
            return 1;
        }

        // Initialisiere Automation-Manager
        auto& automation = Automation::AutomationManager::getInstance();
        if (!automation.initialize()) {
            logger.error("Fehler bei der Automation-Initialisierung");
            return 1;
        }

        // Initialisiere AI-Manager
        auto& ai = AI::AIManager::getInstance();
        if (!ai.initialize()) {
            logger.error("Fehler bei der AI-Initialisierung");
            return 1;
        }

        // Initialisiere KI-Manager
        auto& ki = KI::KIManager::getInstance();
        if (!ki.initialize()) {
            logger.error("Fehler bei der KI-Initialisierung");
            return 1;
        }

        // Initialisiere Integration-Manager
        auto& integration = Integration::IntegrationManager::getInstance();
        if (!integration.initialize()) {
            logger.error("Fehler bei der Integration-Initialisierung");
            return 1;
        }

        // Initialisiere Compatibility-Manager
        auto& compatibility = Compatibility::CompatibilityManager::getInstance();
        if (!compatibility.initialize()) {
            logger.error("Fehler bei der Compatibility-Initialisierung");
            return 1;
        }

        // Starte Hauptanwendung
        Core::Application app;
        if (!app.initialize()) {
            logger.error("Fehler bei der Anwendungsinitialisierung");
            return 1;
        }

        // Hauptschleife
        while (app.isRunning()) {
            app.update();
        }

        // Beende Anwendung
        app.shutdown();

        // Beende alle Komponenten
        compatibility.shutdown();
        integration.shutdown();
        ki.shutdown();
        ai.shutdown();
        automation.shutdown();
        pluginHost.shutdown();
        network.shutdown();
        midi.shutdown();
        sequencer.shutdown();
        ui.shutdown();
        vr.shutdown();
        audio.shutdown();
        plugins.shutdown();
        resources.shutdown();
        config.shutdown();

        logger.info("VRMusicStudio erfolgreich beendet");
        return 0;
    } catch (const std::exception& e) {
        spdlog::error("Unerwarteter Fehler: {}", e.what());
        return 1;
    }
}
