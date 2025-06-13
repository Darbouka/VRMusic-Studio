#pragma once

#include <memory>
#include "BackendService.hpp"
#include "../audio/AudioEngine.hpp"
#include "../vr/VREngine.hpp"
#include "../network/NetworkEngine.hpp"

namespace VRMusicStudio {

class VRMusicStudioApp {
public:
    VRMusicStudioApp();
    ~VRMusicStudioApp();

    bool initialize();
    void shutdown();
    void run();

    // Zugriff auf Subsysteme
    BackendService& backend();
    AudioEngine& audio();
    VREngine& vr();
    NetworkEngine& network();

private:
    std::unique_ptr<BackendService> backendService_;
    std::unique_ptr<AudioEngine> audioEngine_;
    std::unique_ptr<VREngine> vrEngine_;
    std::unique_ptr<NetworkEngine> networkEngine_;

    bool initialized_ = false;
};

} // namespace VRMusicStudio 