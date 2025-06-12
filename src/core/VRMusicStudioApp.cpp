#include "VRMusicStudioApp.hpp"
#include <spdlog/spdlog.h>

namespace VRMusicStudio {

VRMusicStudioApp::VRMusicStudioApp()
    : backendService_(std::make_unique<BackendService>()),
      audioEngine_(std::make_unique<AudioEngine>()),
      vrEngine_(std::make_unique<VREngine>()),
      networkEngine_(std::make_unique<NetworkEngine>()) {}

VRMusicStudioApp::~VRMusicStudioApp() {
    shutdown();
}

bool VRMusicStudioApp::initialize() {
    spdlog::info("Initialisiere BackendService...");
    if (!backendService_->initialize()) return false;
    spdlog::info("Initialisiere AudioEngine...");
    if (!audioEngine_->initialize()) return false;
    spdlog::info("Initialisiere VREngine...");
    if (!vrEngine_->initialize()) return false;
    spdlog::info("Initialisiere NetworkEngine...");
    if (!networkEngine_->initialize()) return false;
    initialized_ = true;
    spdlog::info("Alle Module erfolgreich initialisiert.");
    return true;
}

void VRMusicStudioApp::shutdown() {
    if (!initialized_) return;
    spdlog::info("Fahre alle Module herunter...");
    networkEngine_->shutdown();
    vrEngine_->shutdown();
    audioEngine_->shutdown();
    backendService_->shutdown();
    initialized_ = false;
    spdlog::info("Alle Module wurden heruntergefahren.");
}

void VRMusicStudioApp::run() {
    if (!initialized_) return;
    spdlog::info("Starte Hauptloop...");
    // Hauptloop (Platzhalter)
    while (true) {
        vrEngine_->update();
        networkEngine_->pollEvents();
        // ... weitere Logik ...
        // Zum Testen: break nach einer Iteration
        break;
    }
    spdlog::info("Beende Hauptloop.");
}

BackendService& VRMusicStudioApp::backend() { return *backendService_; }
AudioEngine& VRMusicStudioApp::audio() { return *audioEngine_; }
VREngine& VRMusicStudioApp::vr() { return *vrEngine_; }
NetworkEngine& VRMusicStudioApp::network() { return *networkEngine_; }

} // namespace VRMusicStudio 