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
    if (!element->visible) return;
    
    ImGui::SetCursorPos(ImVec2(element->position.x, element->position.y));
    
    switch (element->type) {
        case UIElement::Type::Button:
            if (ImGui::Button(element->label.c_str(),
                            ImVec2(element->size.x, element->size.y))) {
                if (element->callback) {
                    element->callback();
                }
            }
            break;
            
        case UIElement::Type::Slider:
            ImGui::PushItemWidth(element->size.x);
            if (ImGui::SliderFloat(element->label.c_str(), 
                                 element->value, 
                                 element->min, 
                                 element->max)) {
                if (element->callback) {
                    element->callback();
                }
            }
            ImGui::PopItemWidth();
            break;
            
        case UIElement::Type::Toggle:
            if (ImGui::Checkbox(element->label.c_str(), element->value)) {
                if (element->callback) {
                    element->callback();
                }
            }
            break;
            
        case UIElement::Type::Text:
            ImGui::Text("%s", element->label.c_str());
            break;
            
        case UIElement::Type::Input:
            ImGui::PushItemWidth(element->size.x);
            if (ImGui::InputText(element->label.c_str(), 
                               element->buffer, 
                               element->bufferSize)) {
                if (element->callback) {
                    element->callback();
                }
            }
            ImGui::PopItemWidth();
            break;
            
        default:
            break;
    }
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

void VRUI::setTheme(const std::string& theme) {
    pImpl->currentTheme = theme;
    
    if (theme == "dark") {
        ImGui::StyleColorsDark();
        pImpl->style.backgroundColor = glm::vec4(0.1f, 0.1f, 0.1f, 0.9f);
        pImpl->style.textColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        pImpl->style.accentColor = glm::vec4(0.2f, 0.6f, 1.0f, 1.0f);
    } else if (theme == "light") {
        ImGui::StyleColorsLight();
        pImpl->style.backgroundColor = glm::vec4(0.9f, 0.9f, 0.9f, 0.9f);
        pImpl->style.textColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
        pImpl->style.accentColor = glm::vec4(0.2f, 0.4f, 0.8f, 1.0f);
        }
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(pImpl->scale);
    style.Colors[ImGuiCol_WindowBg] = pImpl->style.backgroundColor;
    style.Colors[ImGuiCol_Text] = pImpl->style.textColor;
    style.Colors[ImGuiCol_Button] = pImpl->style.accentColor;
}

void VRUI::setFont(const std::string& fontPath, float size) {
    pImpl->currentFont = fontPath;
    pImpl->currentFontSize = size;
    
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), size);
    if (font) {
        io.FontDefault = font;
    }
}

void VRUI::setColor(const std::string& elementId, const glm::vec4& color) {
    auto it = std::find_if(pImpl->elements.begin(), pImpl->elements.end(),
                          [&](const UIElement& e) { return e.id == elementId; });
    if (it != pImpl->elements.end()) {
        it->color = color;
    }
}

void VRUI::setOpacity(const std::string& elementId, float opacity) {
    auto it = std::find_if(pImpl->elements.begin(), pImpl->elements.end(),
                          [&](const UIElement& e) { return e.id == elementId; });
    if (it != pImpl->elements.end()) {
        it->opacity = opacity;
        }
    }
    
bool VRUI::isHovered(const std::string& elementId) const {
    auto it = std::find_if(pImpl->elements.begin(), pImpl->elements.end(),
                          [&](const UIElement& e) { return e.id == elementId; });
    if (it != pImpl->elements.end()) {
        ImVec2 mousePos = ImGui::GetMousePos();
        return mousePos.x >= it->position.x && 
               mousePos.x <= it->position.x + it->size.x &&
               mousePos.y >= it->position.y && 
               mousePos.y <= it->position.y + it->size.y;
    }
    return false;
            }

bool VRUI::isClicked(const std::string& elementId) const {
    auto it = std::find_if(pImpl->elements.begin(), pImpl->elements.end(),
                          [&](const UIElement& e) { return e.id == elementId; });
    if (it != pImpl->elements.end()) {
        return ImGui::IsMouseClicked(ImGuiMouseButton_Left) && isHovered(elementId);
    }
    return false;
            }

void VRUI::setInteractive(const std::string& elementId, bool interactive) {
    auto it = std::find_if(pImpl->elements.begin(), pImpl->elements.end(),
                          [&](const UIElement& e) { return e.id == elementId; });
    if (it != pImpl->elements.end()) {
        it->interactive = interactive;
    }
}

void VRUI::setCallback(const std::string& elementId, std::function<void()> callback) {
    auto it = std::find_if(pImpl->elements.begin(), pImpl->elements.end(),
                          [&](const UIElement& e) { return e.id == elementId; });
    if (it != pImpl->elements.end()) {
        it->callback = callback;
    }
}

void VRUI::setLayout(const std::string& panelId, const std::string& layout) {
    auto it = pImpl->panels.find(panelId);
    if (it != pImpl->panels.end()) {
        if (layout == "vertical") {
            float currentY = it->second.position.y + pImpl->style.padding;
            for (auto& element : it->second.elements) {
                element.position.y = currentY;
                currentY += element.size.y + pImpl->style.spacing;
        }
        } else if (layout == "horizontal") {
            float currentX = it->second.position.x + pImpl->style.padding;
            for (auto& element : it->second.elements) {
                element.position.x = currentX;
                currentX += element.size.x + pImpl->style.spacing;
            }
        } else if (layout == "grid") {
            int columns = static_cast<int>(std::sqrt(it->second.elements.size()));
            float cellWidth = (it->second.size.x - (columns + 1) * pImpl->style.spacing) / columns;
            float cellHeight = cellWidth;
        
            for (size_t i = 0; i < it->second.elements.size(); ++i) {
                int row = i / columns;
                int col = i % columns;
                it->second.elements[i].position.x = it->second.position.x + 
                    col * (cellWidth + pImpl->style.spacing) + pImpl->style.spacing;
                it->second.elements[i].position.y = it->second.position.y + 
                    row * (cellHeight + pImpl->style.spacing) + pImpl->style.spacing;
                it->second.elements[i].size = glm::vec2(cellWidth, cellHeight);
            }
        }
    }
}

glm::vec2 VRUI::calculateElementPosition(const UIElement& element,
                                       const UIPanel& panel) {
    glm::vec2 position = element.position;
    
    // Berücksichtige Panel-Position
    position += panel.position;
    
    // Berücksichtige Padding
    position += glm::vec2(pImpl->style.padding);
    
    // Berücksichtige Alignment
    if (panel.alignment == "center") {
        position.x += (panel.size.x - element.size.x) / 2.0f;
    } else if (panel.alignment == "right") {
        position.x += panel.size.x - element.size.x - pImpl->style.padding;
    }
    
    return position;
}

} // namespace VR_DAW
