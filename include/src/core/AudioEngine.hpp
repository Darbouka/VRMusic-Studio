#pragma once

namespace VRMusicStudio {

class AudioEngine {
public:
    AudioEngine() = default;
    ~AudioEngine() = default;
    bool initialize() { return true; }
    void shutdown() {}
    void update() {}
};

} // namespace VRMusicStudio 