#pragma once

namespace VRMusicStudio {

class Version {
public:
    Version() = default;
    virtual ~Version() = default;
    bool initialize() { return true; }
    void shutdown() {}
    void update() {}
    bool isInitialized() const { return true; }
};

} // namespace VRMusicStudio

#define VERSION_STRING "0.1.0-dummy" 