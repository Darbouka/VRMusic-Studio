#include "Application.hpp"
#include "Logger.hpp"
#include "EventSystem.hpp"
#include "audio/AudioEngine.hpp"
#include "VRController.hpp"
#include "VRManager.hpp"
#include "ConfigManager.hpp"
#include "PluginManager.hpp"
#include "ProjectManager.hpp"
#include "UIManager.hpp"
#include "ResourceManager.hpp"
#include "PerformanceMonitor.hpp"
#include "ErrorHandler.hpp"
#include "Version.hpp"
#include <chrono>
#include <thread>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <csignal>
#include <spdlog/spdlog.h>

using namespace VRMusicStudio::Audio;

namespace VRMusicStudio {
namespace Core {

Application& Application::getInstance() {
    static Application instance;
    return instance;
}

Application::Application()
    : running(false)
    , lastUpdateTime(std::chrono::steady_clock::now())
    , deltaTime(0.0f) {
}

Application::~Application() {
    shutdown();
}

bool Application::initialize() {
    try {
        auto& logger = Logger::getInstance();
        logger.info("Initialisiere Anwendung...");

        running = true;
        lastUpdateTime = std::chrono::steady_clock::now();
        deltaTime = 0.0f;

        logger.info("Anwendung erfolgreich initialisiert");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Anwendungsinitialisierung: {}", e.what());
        return false;
    }
}

void Application::shutdown() {
    if (!running) {
        return;
    }

    try {
        auto& logger = Logger::getInstance();
        logger.info("Beende Anwendung...");

        running = false;
        logger.info("Anwendung erfolgreich beendet");
    } catch (const std::exception& e) {
        spdlog::error("Fehler beim Beenden der Anwendung: {}", e.what());
    }
}

void Application::update() {
    if (!running) {
        return;
    }

    try {
        // Berechne Delta-Zeit
        auto currentTime = std::chrono::steady_clock::now();
        deltaTime = std::chrono::duration<float>(currentTime - lastUpdateTime).count();
        lastUpdateTime = currentTime;

        // Aktualisiere Komponenten
        processEvents();
        updateComponents();
        render();
    } catch (const std::exception& e) {
        spdlog::error("Fehler im Update-Zyklus: {}", e.what());
        running = false;
    }
}

bool Application::isRunning() const {
    return running;
}

void Application::processEvents() {
    // Event-Verarbeitung hier implementieren
}

void Application::updateComponents() {
    // Komponenten-Update hier implementieren
}

void Application::render() {
    // Rendering hier implementieren
}

void Application::handleSignal(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        Application::getInstance().shutdown();
    }
}

// Konfigurations-Methoden
void Application::setConfigValue(const std::string& key, const std::string& value) {
    ConfigManager::getInstance().setValue(key, value);
}

std::string Application::getConfigValue(const std::string& key) const {
    return ConfigManager::getInstance().getValue(key);
}

// Plugin-Management
bool Application::loadPlugin(const std::string& pluginPath) {
    return PluginManager::getInstance().loadPlugin(pluginPath);
}

void Application::unloadPlugin(const std::string& pluginName) {
    PluginManager::getInstance().unloadPlugin(pluginName);
}

// Ressourcen-Management
bool Application::loadResource(const std::string& resourcePath) {
    return ResourceManager::getInstance().loadResource(resourcePath);
}

void Application::unloadResource(const std::string& resourceName) {
    ResourceManager::getInstance().unloadResource(resourceName);
}

// Audio-Engine
void Application::initializeAudioEngine() {
    AudioEngine::getInstance().initialize();
}

void Application::shutdownAudioEngine() {
    AudioEngine::getInstance().shutdown();
}

// VR-Integration
void Application::initializeVR() {
    VRManager::getInstance().initialize();
}

void Application::shutdownVR() {
    VRManager::getInstance().shutdown();
}

// UI-Management
void Application::initializeUI() {
    UIManager::getInstance().initialize();
}

void Application::shutdownUI() {
    UIManager::getInstance().shutdown();
}

// Performance-Monitoring
void Application::startPerformanceMonitoring() {
    PerformanceMonitor::getInstance().start();
}

void Application::stopPerformanceMonitoring() {
    PerformanceMonitor::getInstance().stop();
}

} // namespace Core
} // namespace VRMusicStudio 
} // namespace VRMusicStudio 