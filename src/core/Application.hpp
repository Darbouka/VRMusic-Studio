#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace VR_DAW {

class Application {
public:
    static Application& getInstance();

    bool initialize();
    void shutdown();
    void run();
    void stop();

    // Event-System
    using EventCallback = std::function<void(const std::string&)>;
    void registerEventCallback(const std::string& eventName, EventCallback callback);
    void unregisterEventCallback(const std::string& eventName, EventCallback callback);

    // Konfiguration
    void loadConfig(const std::string& configPath);
    void saveConfig(const std::string& configPath);
    void setConfigValue(const std::string& key, const std::string& value);
    std::string getConfigValue(const std::string& key) const;

    // Plugin-Management
    bool loadPlugin(const std::string& pluginPath);
    void unloadPlugin(const std::string& pluginId);
    std::vector<std::string> getLoadedPlugins() const;

    // Ressourcen-Management
    bool loadResource(const std::string& resourcePath);
    void unloadResource(const std::string& resourceId);
    std::vector<std::string> getLoadedResources() const;

private:
    Application();
    ~Application();

    // Singleton-Pattern
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // Implementierungsdetails
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    bool initialized;
    bool running;
    std::string configPath;
    std::vector<std::string> loadedPlugins;
    std::vector<std::string> loadedResources;
};

} // namespace VR_DAW 