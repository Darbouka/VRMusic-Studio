#include "core/HybridMode.hpp"
#include "audio/AudioEngine.hpp"
#include "midi/MIDIController.hpp"
#include "vr/VRIntegration.hpp"
#include "audio/AudioEngine.hpp"
#include "midi/MIDIEngine.hpp"
#include "vr/VRManager.hpp"
#include "ui/UIComponents.hpp"
#include <spdlog/spdlog.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include <memory>
#include <stdexcept>

using namespace VRMusicStudio;

void printUsage() {
    std::cout << "VRMusic Studio - Desktop-Modus\n"
              << "Verwendung:\n"
              << "  --mode [desktop|vr|hybrid]  Startmodus (Standard: desktop)\n"
              << "  --help                      Zeige diese Hilfe\n";
}

int main(int argc, char* argv[]) {
    try {
        // Initialisiere Logger
        spdlog::set_level(spdlog::level::debug);
        spdlog::info("Starting VRMusic Studio...");

        // Erstelle Komponenten
        auto audioEngine = std::make_unique<VRMusicStudio::AudioEngine>();
        auto midiEngine = std::make_unique<VRMusicStudio::MIDIEngine>();
        auto vrManager = std::make_unique<VRMusicStudio::VRManager>();
        auto uiComponents = std::make_unique<VRMusicStudio::UIComponents>();

        // Hauptschleife
        while (!uiComponents->shouldClose()) {
            // Update Komponenten
            audioEngine->update();
            midiEngine->update();
            vrManager->update();
            uiComponents->update();
        }

        spdlog::info("Shutting down VRMusic Studio...");
        return 0;
    } catch (const std::exception& e) {
        spdlog::error("Fatal error: {}", e.what());
        return 1;
    }
}
