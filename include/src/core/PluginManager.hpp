#pragma once

namespace VRMusicStudio {

class PluginManager {
public:
    PluginManager() = default;
    virtual ~PluginManager() = default;
    bool initialize() { return true; }
    void shutdown() {}
    void update() {}
    bool isInitialized() const { return true; }
};

} // namespace VRMusicStudio 