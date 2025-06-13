#pragma once

namespace VRMusicStudio {

class VRManager {
public:
    VRManager() = default;
    virtual ~VRManager() = default;
    bool initialize() { return true; }
    void shutdown() {}
    void update() {}
    bool isInitialized() const { return true; }
};

} // namespace VRMusicStudio 