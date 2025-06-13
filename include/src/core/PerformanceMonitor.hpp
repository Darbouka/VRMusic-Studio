#pragma once

namespace VRMusicStudio {

class PerformanceMonitor {
public:
    PerformanceMonitor() = default;
    virtual ~PerformanceMonitor() = default;
    bool initialize() { return true; }
    void shutdown() {}
    void update() {}
    bool isInitialized() const { return true; }
};

} // namespace VRMusicStudio 