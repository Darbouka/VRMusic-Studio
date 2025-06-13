#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <mutex>
#include <condition_variable>

namespace VRMusicStudio {

class EventSystem;
class AudioEngine;
class VRController;
class VRManager;
class ConfigManager;
class PluginManager;
class ProjectManager;
class UIManager;
class ResourceManager;
class PerformanceMonitor;
class ErrorHandler;

class Application {
public:
    Application();
    ~Application();

    bool initialize();
    void shutdown();
    void run();
    void update();
    static void handleSignal(int signal);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::chrono::steady_clock::time_point lastUpdateTime;
    std::chrono::milliseconds targetFrameTime;
    std::mutex mutex;
    std::condition_variable cv;
    bool shouldShutdown;
};

} // namespace VRMusicStudio 