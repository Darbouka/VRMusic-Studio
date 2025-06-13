#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include "Logger.hpp"
#include "EventSystem.hpp"
#include <vector>

namespace VRMusicStudio {
namespace Core {

class Application {
public:
    static Application& getInstance();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    bool initialize();
    void shutdown();
    void run();
    void update();
    static void handleSignal(int signal);
    
    // Konfiguration
    void setConfigValue(const std::string& key, const std::string& value);
    std::string getConfigValue(const std::string& key) const;
    
    // Plugin-Management
    bool loadPlugin(const std::string& pluginPath);
    void unloadPlugin(const std::string& pluginName);
    
    // Ressourcen-Management
    bool loadResource(const std::string& resourcePath);
    void unloadResource(const std::string& resourceName);
    
    // Audio-Engine
    void initializeAudioEngine();
    void shutdownAudioEngine();
    
    // VR-Integration
    void initializeVR();
    void shutdownVR();
    
    // UI-Management
    void initializeUI();
    void shutdownUI();
    
    // Performance-Monitoring
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    
    // Error-Handling
    void handleError(const std::string& errorMessage);
    void setErrorCallback(std::function<void(const std::string&)> callback);

    bool isRunning() const;

private:
    Application();
    ~Application();

    void processEvents();
    void updateComponents();
    void render();

    bool running;
    std::chrono::steady_clock::time_point lastUpdateTime;
    float deltaTime;

    bool isRunning_;
    bool shouldShutdown_;
    std::chrono::steady_clock::time_point lastUpdateTime_;
    std::chrono::milliseconds targetFrameTime_;
    std::mutex mutex_;
    std::condition_variable cv_;
    Logger& logger_;
    EventSystem& eventSystem_;
    std::function<void(const std::string&)> errorCallback_;
};

} // namespace Core
} // namespace VRMusicStudio 