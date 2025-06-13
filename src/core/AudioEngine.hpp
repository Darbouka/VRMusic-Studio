#pragma once

namespace VRMusicStudio {
namespace Core {

class AudioEngine {
public:
    static AudioEngine& getInstance();
    void initialize();
    void shutdown();
    void update();
};

} // namespace Core
} // namespace VRMusicStudio 