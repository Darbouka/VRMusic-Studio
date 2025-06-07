#include "VRUI.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace VR_DAW {

VRUI& VRUI::getInstance() {
    static VRUI instance;
    return instance;
}

VRUI::VRUI()
    : initialized(false)
    , logger("VRUI") {
}

VRUI::~VRUI() {
    shutdown();
}

bool VRUI::initialize() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (initialized) {
        return true;
    }

    try {
        if (!initializeRenderer()) {
            logger.error("Fehler bei der Renderer-Initialisierung");
            return false;
        }

        initialize();
        initialized = true;
        logger.info("VR-UI erfolgreich initialisiert");
        return true;
    }
    catch (const std::exception& e) {
        logger.error("Fehler bei der VR-UI-Initialisierung: {}", e.what());
        return false;
    }
}

void VRUI::shutdown() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        shutdownRenderer();
        elements.clear();
        tracks.clear();
        plugins.clear();
        initialized = false;
        logger.info("VR-UI erfolgreich beendet");
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Beenden der VR-UI: {}", e.what());
    }
}

bool VRUI::isInitialized() const {
    std::lock_guard<std::mutex> lock(mutex);
    return initialized;
}

void VRUI::update() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        updateElementTransforms();
        handleInput();
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim VR-UI-Update: {}", e.what());
    }
}

void VRUI::render() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        for (const auto& element : elements) {
            if (element->visible) {
                renderElement(element);
            }
        }
        
        for (const auto& track : tracks) {
            renderTrackView(track);
        }
        
        for (const auto& plugin : plugins) {
            renderPluginView(plugin);
        }
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim VR-UI-Rendering: {}", e.what());
    }
}

void VRUI::handleInput() {
    auto& vrManager = VRManager::getInstance();
    auto controllers = vrManager.getControllers();

    for (const auto& controller : controllers) {
        if (controller.isConnected && controller.isTracking) {
            glm::vec3 controllerPosition = controller.position;
            
            for (const auto& element : elements) {
                if (element->visible && element->interactive) {
                    if (checkInteraction(element, controllerPosition)) {
                        if (element->onHover) {
                            element->onHover();
                        }
                    }
                }
            }
        }
    }
}

std::shared_ptr<UIElement> VRUI::createElement(const UIElement& element) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return nullptr;
    }

    try {
        auto newElement = std::make_shared<UIElement>(element);
        elements.push_back(newElement);
        logger.info("UI-Element erstellt: {}", element.id);
        return newElement;
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Erstellen des UI-Elements: {}", e.what());
        return nullptr;
    }
}

void VRUI::removeElement(const std::string& elementId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        auto it = std::find_if(elements.begin(), elements.end(),
            [&elementId](const auto& element) { return element->id == elementId; });
        
        if (it != elements.end()) {
            elements.erase(it);
            logger.info("UI-Element entfernt: {}", elementId);
        }
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Entfernen des UI-Elements: {}", e.what());
    }
}

std::shared_ptr<UIElement> VRUI::getElement(const std::string& elementId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return nullptr;
    }

    auto it = std::find_if(elements.begin(), elements.end(),
        [&elementId](const auto& element) { return element->id == elementId; });
    
    return (it != elements.end()) ? *it : nullptr;
}

std::shared_ptr<TrackView> VRUI::createTrack(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return nullptr;
    }

    try {
        auto track = std::make_shared<TrackView>();
        track->name = name;
        track->isSelected = false;
        track->volume = 1.0f;
        track->isMuted = false;
        
        tracks.push_back(track);
        logger.info("Track erstellt: {}", name);
        return track;
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Erstellen des Tracks: {}", e.what());
        return nullptr;
    }
}

void VRUI::removeTrack(const std::string& trackId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        auto it = std::find_if(tracks.begin(), tracks.end(),
            [&trackId](const auto& track) { return track->name == trackId; });
        
        if (it != tracks.end()) {
            tracks.erase(it);
            logger.info("Track entfernt: {}", trackId);
        }
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Entfernen des Tracks: {}", e.what());
    }
}

std::shared_ptr<TrackView> VRUI::getTrack(const std::string& trackId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return nullptr;
    }

    auto it = std::find_if(tracks.begin(), tracks.end(),
        [&trackId](const auto& track) { return track->name == trackId; });
    
    return (it != tracks.end()) ? *it : nullptr;
}

std::shared_ptr<PluginView> VRUI::createPluginView(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return nullptr;
    }

    try {
        auto plugin = std::make_shared<PluginView>();
        plugin->name = name;
        plugin->isExpanded = false;
        
        plugins.push_back(plugin);
        logger.info("Plugin-Ansicht erstellt: {}", name);
        return plugin;
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Erstellen der Plugin-Ansicht: {}", e.what());
        return nullptr;
    }
}

void VRUI::removePluginView(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        auto it = std::find_if(plugins.begin(), plugins.end(),
            [&pluginId](const auto& plugin) { return plugin->name == pluginId; });
        
        if (it != plugins.end()) {
            plugins.erase(it);
            logger.info("Plugin-Ansicht entfernt: {}", pluginId);
        }
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Entfernen der Plugin-Ansicht: {}", e.what());
    }
}

std::shared_ptr<PluginView> VRUI::getPluginView(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return nullptr;
    }

    auto it = std::find_if(plugins.begin(), plugins.end(),
        [&pluginId](const auto& plugin) { return plugin->name == pluginId; });
    
    return (it != plugins.end()) ? *it : nullptr;
}

bool VRUI::initializeRenderer() {
    // Hier wird der OpenGL-Renderer initialisiert
    // z.B. Shader-Programme, Texturen, etc.
    return true;
}

void VRUI::shutdownRenderer() {
    // Hier wird der OpenGL-Renderer beendet
    // z.B. Ressourcen freigeben
}

void VRUI::updateElementTransforms() {
    // Hier werden die Transformationen der UI-Elemente aktualisiert
    // z.B. Position, Rotation, Skalierung
}

void VRUI::renderElement(const std::shared_ptr<UIElement>& element) {
    // Hier wird ein UI-Element gerendert
    // z.B. mit OpenGL-Shadern
}

bool VRUI::checkInteraction(const std::shared_ptr<UIElement>& element, const glm::vec3& controllerPosition) {
    // Hier wird die Interaktion zwischen Controller und UI-Element geprüft
    // z.B. Kollisionserkennung, Hover-Effekte
    return false;
}

// Revolutionäre UI-Initialisierung
void VRUI::initialize() {
    // Revolutionäre UI-Komponenten initialisieren
    initializeFontSystem();
    initializeControls();
    initializeLayouts();
    initializeAnimations();
    initializeInteractions();
}

// Revolutionäre UI-Steuerelemente
void VRUI::initializeControls() {
    // Revolutionäre Steuerelemente erstellen
    createButton("play", glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.1f));
    createSlider("volume", glm::vec3(0.0f, 0.8f, 0.0f), glm::vec3(0.2f));
    createKnob("pan", glm::vec3(0.0f, 0.6f, 0.0f), 0.05f);
    createWaveform("waveform", glm::vec3(0.0f, 0.4f, 0.0f), glm::vec3(0.3f));
}

// Revolutionäre UI-Layouts
void VRUI::initializeLayouts() {
    // Revolutionäre Layouts definieren
    setLayout("default");
    arrangeTracks();
    arrangePlugins();
    updateLayout();
}

// Revolutionäre UI-Animationen
void VRUI::initializeAnimations() {
    // Revolutionäre Animationen initialisieren
    updateAnimations();
    animateElement(*findElementAtPosition(glm::vec3(0.0f)), glm::vec3(1.0f), 1.0f);
}

// Revolutionäre UI-Interaktionen
void VRUI::initializeInteractions() {
    // Revolutionäre Interaktionen initialisieren
    handleControllerInput(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    handleGesture("grab", glm::vec3(0.0f));
    handleVoiceCommand("play");
}

// Revolutionäre UI-Interaktion
void VRUI::handleControllerInput(const glm::vec3& position, const glm::vec3& direction) {
    // Revolutionäre Controller-Interaktion
    auto element = findElementAtPosition(position);
    if (element) {
        if (isPointInElement(position, *element)) {
            element->onHover();
        }
    }
}

// Revolutionäre UI-Gesten
void VRUI::handleGesture(const std::string& gestureType, const glm::vec3& position) {
    // Revolutionäre Gestenerkennung
    if (gestureType == "grab") {
        auto element = findElementAtPosition(position);
        if (element) {
            element->onClick();
        }
    }
}

// Revolutionäre UI-Sprachsteuerung
void VRUI::handleVoiceCommand(const std::string& command) {
    // Revolutionäre Sprachsteuerung
    if (command == "play") {
        // Implementiere Play-Funktion
    } else if (command == "stop") {
        // Implementiere Stop-Funktion
    }
}

// Revolutionäre UI-Layout-Management
void VRUI::setLayout(const std::string& layoutType) {
    // Revolutionäres Layout-Management
    if (layoutType == "default") {
        // Implementiere Standard-Layout
    } else if (layoutType == "compact") {
        // Implementiere Kompakt-Layout
    }
}

// Revolutionäre UI-Element-Erstellung
UIElement* VRUI::createButton(const std::string& id, const glm::vec3& position, const glm::vec3& scale) {
    // Revolutionäre Button-Erstellung
    UIElement element;
    element.id = id;
    element.position = position;
    element.scale = scale;
    element.visible = true;
    element.interactive = true;
    elements.push_back(element);
    return &elements.back();
}

// Revolutionäre UI-Track-Erstellung
TrackView* VRUI::createTrackView(int trackId, const std::string& name) {
    // Revolutionäre Track-Erstellung
    TrackView view;
    view.trackId = trackId;
    view.name = name;
    view.volume = 1.0f;
    view.muted = false;
    view.solo = false;
    tracks.push_back(view);
    return &tracks.back();
}

// Revolutionäre UI-Plugin-Erstellung
PluginView* VRUI::createPluginView(const std::string& pluginId, const std::string& name) {
    // Revolutionäre Plugin-Erstellung
    PluginView view;
    view.pluginId = pluginId;
    view.name = name;
    view.isExpanded = false;
    plugins.push_back(view);
    return &plugins.back();
}

// Revolutionäre UI-Text-Erstellung
TextElement* VRUI::createText(const std::string& text, const glm::vec3& position, float fontSize) {
    // Revolutionäre Text-Erstellung
    TextElement element;
    element.text = text;
    element.position = position;
    element.fontSize = fontSize;
    element.visible = true;
    textElements.push_back(element);
    return &textElements.back();
}

} // namespace VR_DAW 