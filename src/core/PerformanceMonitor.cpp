#include "PerformanceMonitor.hpp"

namespace VRMusicStudio {
namespace Core {

PerformanceMonitor& PerformanceMonitor::getInstance() {
    static PerformanceMonitor instance;
    return instance;
}

void PerformanceMonitor::initialize() {}
void PerformanceMonitor::shutdown() {}
void PerformanceMonitor::start() {}
void PerformanceMonitor::stop() {}
float PerformanceMonitor::getCPUUsage() const { return 0.0f; }
float PerformanceMonitor::getMemoryUsage() const { return 0.0f; }

} // namespace Core
} // namespace VRMusicStudio 