#pragma once

namespace VRMusicStudio {

class UIManager {
public:
    UIManager() = default;
    virtual ~UIManager() = default;
    bool initialize() { return true; }
    void shutdown() {}
    void update() {}
    bool isInitialized() const { return true; }
};

} // namespace VRMusicStudio 