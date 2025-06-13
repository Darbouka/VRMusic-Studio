#pragma once

namespace VRMusicStudio {

class ResourceManager {
public:
    ResourceManager() = default;
    virtual ~ResourceManager() = default;
    bool initialize() { return true; }
    void shutdown() {}
    void update() {}
    bool isInitialized() const { return true; }
};

} // namespace VRMusicStudio 