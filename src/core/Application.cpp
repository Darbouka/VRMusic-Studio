#include "Application.hpp"
#include "Logger.hpp"
#include "Config.hpp"
#include "PluginManager.hpp"
#include "ResourceManager.hpp"
#include "EventSystem.hpp"

namespace VR_DAW {

struct Application::Impl {
    std::unique_ptr<Logger> logger;
    std::unique_ptr<Config> config;
    std::unique_ptr<PluginManager> pluginManager;
    std::unique_ptr<ResourceManager> resourceManager;
    std::unique_ptr<EventSystem> eventSystem;
};

Application& Application::getInstance() {
    static Application instance;
    return instance;
}

Application::Application()
    : pImpl(std::make_unique<Impl>())
    , initialized(false)
    , running(false) {
}

Application::~Application() = default;

bool Application::initialize() {
    if (initialized) {
        return true;
    }

    try {
        // Logger initialisieren
        pImpl->logger = std::make_unique<Logger>("VR-DAW");
        pImpl->logger->info("Initialisiere VR-DAW...");

        // Konfiguration laden
        pImpl->config = std::make_unique<Config>();
        if (!pImpl->config->load(configPath)) {
            pImpl->logger->error("Fehler beim Laden der Konfiguration");
            return false;
        }

        // Plugin-Manager initialisieren
        pImpl->pluginManager = std::make_unique<PluginManager>();
        if (!pImpl->pluginManager->initialize()) {
            pImpl->logger->error("Fehler beim Initialisieren des Plugin-Managers");
            return false;
        }

        // Ressourcen-Manager initialisieren
        pImpl->resourceManager = std::make_unique<ResourceManager>();
        if (!pImpl->resourceManager->initialize()) {
            pImpl->logger->error("Fehler beim Initialisieren des Ressourcen-Managers");
            return false;
        }

        // Event-System initialisieren
        pImpl->eventSystem = std::make_unique<EventSystem>();
        if (!pImpl->eventSystem->initialize()) {
            pImpl->logger->error("Fehler beim Initialisieren des Event-Systems");
            return false;
        }

        initialized = true;
        pImpl->logger->info("VR-DAW erfolgreich initialisiert");
        return true;
    }
    catch (const std::exception& e) {
        if (pImpl->logger) {
            pImpl->logger->error("Fehler bei der Initialisierung: {}", e.what());
        }
        return false;
    }
}

void Application::shutdown() {
    if (!initialized) {
        return;
    }

    try {
        pImpl->logger->info("Beende VR-DAW...");

        // Event-System beenden
        if (pImpl->eventSystem) {
            pImpl->eventSystem->shutdown();
        }

        // Ressourcen-Manager beenden
        if (pImpl->resourceManager) {
            pImpl->resourceManager->shutdown();
        }

        // Plugin-Manager beenden
        if (pImpl->pluginManager) {
            pImpl->pluginManager->shutdown();
        }

        // Konfiguration speichern
        if (pImpl->config) {
            pImpl->config->save(configPath);
        }

        initialized = false;
        running = false;
        pImpl->logger->info("VR-DAW erfolgreich beendet");
    }
    catch (const std::exception& e) {
        if (pImpl->logger) {
            pImpl->logger->error("Fehler beim Beenden: {}", e.what());
        }
    }
}

void Application::run() {
    if (!initialized || running) {
        return;
    }

    running = true;
    pImpl->logger->info("Starte VR-DAW Hauptschleife...");

    try {
        while (running) {
            // Event-System aktualisieren
            pImpl->eventSystem->update();

            // Plugin-Manager aktualisieren
            pImpl->pluginManager->update();

            // Ressourcen-Manager aktualisieren
            pImpl->resourceManager->update();
        }
    }
    catch (const std::exception& e) {
        pImpl->logger->error("Fehler in der Hauptschleife: {}", e.what());
        running = false;
    }
}

void Application::stop() {
    running = false;
}

void Application::registerEventCallback(const std::string& eventName, EventCallback callback) {
    if (pImpl->eventSystem) {
        pImpl->eventSystem->registerCallback(eventName, callback);
    }
}

void Application::unregisterEventCallback(const std::string& eventName, EventCallback callback) {
    if (pImpl->eventSystem) {
        pImpl->eventSystem->unregisterCallback(eventName, callback);
    }
}

void Application::loadConfig(const std::string& configPath) {
    this->configPath = configPath;
    if (pImpl->config) {
        pImpl->config->load(configPath);
    }
}

void Application::saveConfig(const std::string& configPath) {
    if (pImpl->config) {
        pImpl->config->save(configPath);
    }
}

void Application::setConfigValue(const std::string& key, const std::string& value) {
    if (pImpl->config) {
        pImpl->config->setValue(key, value);
    }
}

std::string Application::getConfigValue(const std::string& key) const {
    if (pImpl->config) {
<<<<<<< HEAD
        return pImpl->config->getValue(key);
=======
        return pImpl->config->getValue(key, std::string());
>>>>>>> 0dff1c4 (init 2)
    }
    return "";
}

bool Application::loadPlugin(const std::string& pluginPath) {
    if (pImpl->pluginManager) {
        return pImpl->pluginManager->loadPlugin(pluginPath);
    }
    return false;
}

void Application::unloadPlugin(const std::string& pluginId) {
    if (pImpl->pluginManager) {
        pImpl->pluginManager->unloadPlugin(pluginId);
    }
}

std::vector<std::string> Application::getLoadedPlugins() const {
    if (pImpl->pluginManager) {
        return pImpl->pluginManager->getLoadedPlugins();
    }
    return {};
}

bool Application::loadResource(const std::string& resourcePath) {
    if (pImpl->resourceManager) {
        return pImpl->resourceManager->loadResource(resourcePath);
    }
    return false;
}

void Application::unloadResource(const std::string& resourceId) {
    if (pImpl->resourceManager) {
        pImpl->resourceManager->unloadResource(resourceId);
    }
}

std::vector<std::string> Application::getLoadedResources() const {
    if (pImpl->resourceManager) {
        return pImpl->resourceManager->getLoadedResources();
    }
    return {};
}

} // namespace VR_DAW 