#pragma once

#include <memory>
#include <string>

namespace VRMusicStudio {

class VRController {
public:
    VRController() = default;
    virtual ~VRController() = default;

    bool initialize() { return true; }
    void shutdown() {}
    void update() {}
    bool isInitialized() const { return true; }
};

} // namespace VRMusicStudio 