#pragma once

namespace VRMusicStudio {

class ConfigManager {
public:
    ConfigManager() = default;
    virtual ~ConfigManager() = default;
    bool initialize() { return true; }
    void shutdown() {}
    void update() {}
    bool isInitialized() const { return true; }
};

} // namespace VRMusicStudio 