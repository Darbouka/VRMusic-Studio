#pragma once

namespace VRMusicStudio {
namespace Core {

class PerformanceMonitor {
public:
    static PerformanceMonitor& getInstance();
    void initialize();
    void shutdown();
    void update();
    void start();
    void stop();
    float getCPUUsage() const;
    float getMemoryUsage() const;
};

} // namespace Core
} // namespace VRMusicStudio 