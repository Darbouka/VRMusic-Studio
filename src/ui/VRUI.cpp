#include "VRUI.hpp"
<<<<<<< HEAD
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
=======
#include <spdlog/spdlog.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>

namespace VRMusicStudio {

struct VRUI::Impl {
    // UI-Komponenten
    struct Panel {
        std::string name;
        bool visible;
        ImVec2 position;
        ImVec2 size;
        std::function<void()> renderCallback;
    };
    
    struct Menu {
        std::string name;
        bool open;
        std::vector<std::string> items;
        std::function<void(int)> callback;
    };
    
    struct Button {
        std::string name;
        bool pressed;
        ImVec2 position;
        ImVec2 size;
        std::function<void()> callback;
    };
    
    struct Slider {
        std::string name;
        float value;
        float min;
        float max;
        ImVec2 position;
        ImVec2 size;
        std::function<void(float)> callback;
    };
    
    struct Knob {
        std::string name;
        float value;
        float min;
        float max;
        ImVec2 position;
        float size;
        std::function<void(float)> callback;
    };
    
    struct Waveform {
        std::string name;
        std::vector<float> data;
        ImVec2 position;
        ImVec2 size;
        bool autoScale;
    };
    
    struct Spectrum {
        std::string name;
        std::vector<float> data;
        ImVec2 position;
        ImVec2 size;
        bool autoScale;
    };
    
    std::vector<Panel> panels;
    std::vector<Menu> menus;
    std::vector<Button> buttons;
    std::vector<Slider> sliders;
    std::vector<Knob> knobs;
    std::vector<Waveform> waveforms;
    std::vector<Spectrum> spectrums;
    
    // UI-Zustand
    bool isInitialized;
    GLFWwindow* window;
    ImGuiContext* context;
    
    // UI-Parameter
    float scale;
    ImVec4 backgroundColor;
    ImVec4 textColor;
    ImVec4 accentColor;
    
    // Callbacks
    std::function<void()> renderCallback;
    std::function<void()> updateCallback;
    
    Impl() : isInitialized(false), window(nullptr), context(nullptr),
             scale(1.0f), backgroundColor(0.1f, 0.1f, 0.1f, 1.0f),
             textColor(1.0f, 1.0f, 1.0f, 1.0f),
             accentColor(0.2f, 0.6f, 1.0f, 1.0f) {}
};

VRUI::VRUI() : pImpl(std::make_unique<Impl>()) {}
>>>>>>> 0dff1c4 (init 2)

VRUI::~VRUI() {
    shutdown();
}

bool VRUI::initialize() {
<<<<<<< HEAD
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
=======
    if (pImpl->isInitialized) return true;
    
    // Initialisiere GLFW
    if (!glfwInit()) {
        spdlog::error("Failed to initialize GLFW");
        return false;
    }
    
    // Erstelle Fenster
    pImpl->window = glfwCreateWindow(1280, 720, "VR Music Studio", nullptr, nullptr);
    if (!pImpl->window) {
        spdlog::error("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(pImpl->window);
    glfwSwapInterval(1);
    
    // Initialisiere ImGui
    IMGUI_CHECKVERSION();
    pImpl->context = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(pImpl->scale);
    style.Colors[ImGuiCol_WindowBg] = pImpl->backgroundColor;
    style.Colors[ImGuiCol_Text] = pImpl->textColor;
    style.Colors[ImGuiCol_Button] = pImpl->accentColor;
    
    ImGui_ImplGlfw_InitForOpenGL(pImpl->window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    pImpl->isInitialized = true;
    return true;
}

void VRUI::shutdown() {
    if (!pImpl->isInitialized) return;
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(pImpl->context);
    
    glfwDestroyWindow(pImpl->window);
    glfwTerminate();
    
    pImpl->isInitialized = false;
}

void VRUI::createPanel(const std::string& name, const ImVec2& position,
                      const ImVec2& size, std::function<void()> renderCallback) {
    Impl::Panel panel;
    panel.name = name;
    panel.visible = true;
    panel.position = position;
    panel.size = size;
    panel.renderCallback = renderCallback;
    
    pImpl->panels.push_back(panel);
}

void VRUI::createMenu(const std::string& name, const std::vector<std::string>& items,
                     std::function<void(int)> callback) {
    Impl::Menu menu;
    menu.name = name;
    menu.open = false;
    menu.items = items;
    menu.callback = callback;
    
    pImpl->menus.push_back(menu);
}

void VRUI::createButton(const std::string& name, const ImVec2& position,
                       const ImVec2& size, std::function<void()> callback) {
    Impl::Button button;
    button.name = name;
    button.pressed = false;
    button.position = position;
    button.size = size;
    button.callback = callback;
    
    pImpl->buttons.push_back(button);
}

void VRUI::createSlider(const std::string& name, float value, float min, float max,
                       const ImVec2& position, const ImVec2& size,
                       std::function<void(float)> callback) {
    Impl::Slider slider;
    slider.name = name;
    slider.value = value;
    slider.min = min;
    slider.max = max;
    slider.position = position;
    slider.size = size;
    slider.callback = callback;
    
    pImpl->sliders.push_back(slider);
}

void VRUI::createKnob(const std::string& name, float value, float min, float max,
                     const ImVec2& position, float size,
                     std::function<void(float)> callback) {
    Impl::Knob knob;
    knob.name = name;
    knob.value = value;
    knob.min = min;
    knob.max = max;
    knob.position = position;
    knob.size = size;
    knob.callback = callback;
    
    pImpl->knobs.push_back(knob);
}

void VRUI::createWaveform(const std::string& name, const std::vector<float>& data,
                         const ImVec2& position, const ImVec2& size, bool autoScale) {
    Impl::Waveform waveform;
    waveform.name = name;
    waveform.data = data;
    waveform.position = position;
    waveform.size = size;
    waveform.autoScale = autoScale;
    
    pImpl->waveforms.push_back(waveform);
}

void VRUI::createSpectrum(const std::string& name, const std::vector<float>& data,
                         const ImVec2& position, const ImVec2& size, bool autoScale) {
    Impl::Spectrum spectrum;
    spectrum.name = name;
    spectrum.data = data;
    spectrum.position = position;
    spectrum.size = size;
    spectrum.autoScale = autoScale;
    
    pImpl->spectrums.push_back(spectrum);
}

void VRUI::setPanelVisible(const std::string& name, bool visible) {
    for (auto& panel : pImpl->panels) {
        if (panel.name == name) {
            panel.visible = visible;
            break;
>>>>>>> 0dff1c4 (init 2)
        }
    }
}

<<<<<<< HEAD
// Revolutionäre UI-Gesten
void VRUI::handleGesture(const std::string& gestureType, const glm::vec3& position) {
    // Revolutionäre Gestenerkennung
    if (gestureType == "grab") {
        auto element = findElementAtPosition(position);
        if (element) {
            element->onClick();
=======
void VRUI::setMenuOpen(const std::string& name, bool open) {
    for (auto& menu : pImpl->menus) {
        if (menu.name == name) {
            menu.open = open;
            break;
>>>>>>> 0dff1c4 (init 2)
        }
    }
}

<<<<<<< HEAD
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
=======
void VRUI::setButtonPressed(const std::string& name, bool pressed) {
    for (auto& button : pImpl->buttons) {
        if (button.name == name) {
            button.pressed = pressed;
            break;
        }
    }
}

void VRUI::setSliderValue(const std::string& name, float value) {
    for (auto& slider : pImpl->sliders) {
        if (slider.name == name) {
            slider.value = std::clamp(value, slider.min, slider.max);
            break;
        }
    }
}

void VRUI::setKnobValue(const std::string& name, float value) {
    for (auto& knob : pImpl->knobs) {
        if (knob.name == name) {
            knob.value = std::clamp(value, knob.min, knob.max);
            break;
        }
    }
}

void VRUI::updateWaveform(const std::string& name, const std::vector<float>& data) {
    for (auto& waveform : pImpl->waveforms) {
        if (waveform.name == name) {
            waveform.data = data;
            break;
        }
    }
}

void VRUI::updateSpectrum(const std::string& name, const std::vector<float>& data) {
    for (auto& spectrum : pImpl->spectrums) {
        if (spectrum.name == name) {
            spectrum.data = data;
            break;
        }
    }
}

void VRUI::setScale(float scale) {
    pImpl->scale = scale;
    ImGui::GetStyle().ScaleAllSizes(scale);
}

void VRUI::setBackgroundColor(const ImVec4& color) {
    pImpl->backgroundColor = color;
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = color;
}

void VRUI::setTextColor(const ImVec4& color) {
    pImpl->textColor = color;
    ImGui::GetStyle().Colors[ImGuiCol_Text] = color;
}

void VRUI::setAccentColor(const ImVec4& color) {
    pImpl->accentColor = color;
    ImGui::GetStyle().Colors[ImGuiCol_Button] = color;
}

void VRUI::setRenderCallback(std::function<void()> callback) {
    pImpl->renderCallback = callback;
}

void VRUI::setUpdateCallback(std::function<void()> callback) {
    pImpl->updateCallback = callback;
}

void VRUI::render() {
    if (!pImpl->isInitialized) return;
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Rendere Panels
    for (auto& panel : pImpl->panels) {
        if (panel.visible) {
            ImGui::SetNextWindowPos(panel.position);
            ImGui::SetNextWindowSize(panel.size);
            
            ImGui::Begin(panel.name.c_str(), nullptr,
                        ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove);
            
            if (panel.renderCallback) {
                panel.renderCallback();
            }
            
            ImGui::End();
        }
    }
    
    // Rendere Menüs
    for (auto& menu : pImpl->menus) {
        if (ImGui::BeginMenu(menu.name.c_str())) {
            for (size_t i = 0; i < menu.items.size(); ++i) {
                if (ImGui::MenuItem(menu.items[i].c_str())) {
                    if (menu.callback) {
                        menu.callback(i);
                    }
                }
            }
            ImGui::EndMenu();
        }
    }
    
    // Rendere Buttons
    for (auto& button : pImpl->buttons) {
        ImGui::SetCursorPos(button.position);
        if (ImGui::Button(button.name.c_str(), button.size)) {
            if (button.callback) {
                button.callback();
            }
        }
    }
    
    // Rendere Slider
    for (auto& slider : pImpl->sliders) {
        ImGui::SetCursorPos(slider.position);
        if (ImGui::SliderFloat(slider.name.c_str(), &slider.value,
                              slider.min, slider.max)) {
            if (slider.callback) {
                slider.callback(slider.value);
            }
        }
    }
    
    // Rendere Knobs
    for (auto& knob : pImpl->knobs) {
        ImGui::SetCursorPos(knob.position);
        if (ImGui::VSliderFloat(knob.name.c_str(), ImVec2(knob.size, knob.size),
                               &knob.value, knob.min, knob.max)) {
            if (knob.callback) {
                knob.callback(knob.value);
            }
        }
    }
    
    // Rendere Waveforms
    for (auto& waveform : pImpl->waveforms) {
        ImGui::SetCursorPos(waveform.position);
        ImGui::BeginChild(waveform.name.c_str(), waveform.size, true);
        
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        
        float maxValue = waveform.autoScale ?
            *std::max_element(waveform.data.begin(), waveform.data.end()) : 1.0f;
        
        for (size_t i = 0; i < waveform.data.size(); ++i) {
            float x = p.x + (i * waveform.size.x / waveform.data.size());
            float y = p.y + (waveform.data[i] * waveform.size.y / (2.0f * maxValue));
            drawList->AddLine(ImVec2(x, p.y + waveform.size.y / 2.0f),
                            ImVec2(x, y), IM_COL32(255, 255, 255, 255));
        }
        
        ImGui::EndChild();
    }
    
    // Rendere Spectrums
    for (auto& spectrum : pImpl->spectrums) {
        ImGui::SetCursorPos(spectrum.position);
        ImGui::BeginChild(spectrum.name.c_str(), spectrum.size, true);
        
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        
        float maxValue = spectrum.autoScale ?
            *std::max_element(spectrum.data.begin(), spectrum.data.end()) : 1.0f;
        
        for (size_t i = 0; i < spectrum.data.size(); ++i) {
            float x = p.x + (i * spectrum.size.x / spectrum.data.size());
            float height = spectrum.data[i] * spectrum.size.y / maxValue;
            drawList->AddRectFilled(ImVec2(x, p.y + spectrum.size.y - height),
                                  ImVec2(x + spectrum.size.x / spectrum.data.size(),
                                        p.y + spectrum.size.y),
                                  IM_COL32(255, 255, 255, 255));
        }
        
        ImGui::EndChild();
    }
    
    if (pImpl->renderCallback) {
        pImpl->renderCallback();
    }
    
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(pImpl->window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(pImpl->backgroundColor.x, pImpl->backgroundColor.y,
                 pImpl->backgroundColor.z, pImpl->backgroundColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    glfwSwapBuffers(pImpl->window);
}

void VRUI::update() {
    if (!pImpl->isInitialized) return;
    
    glfwPollEvents();
    
    if (pImpl->updateCallback) {
        pImpl->updateCallback();
    }
}

bool VRUI::shouldClose() const {
    return glfwWindowShouldClose(pImpl->window);
}

} // namespace VRMusicStudio 
>>>>>>> 0dff1c4 (init 2)
