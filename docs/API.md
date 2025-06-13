# VRMusicStudio API-Dokumentation

## Inhaltsverzeichnis
1. [Core-Komponenten](#core-komponenten)
2. [Audio-Komponenten](#audio-komponenten)
3. [VR-Komponenten](#vr-komponenten)
4. [UI-Komponenten](#ui-komponenten)

## Core-Komponenten

### Logger
Der Logger bietet eine zentrale Logging-Funktionalität für die Anwendung.

```cpp
namespace VRMusicStudio::Core {
    class Logger {
    public:
        static Logger& getInstance();
        bool initialize();
        void shutdown();
        void setLogLevel(LogLevel level);
        void log(LogLevel level, const std::string& message);
        template<typename... Args>
        void log(LogLevel level, const std::string& format, Args&&... args);
    };
}
```

### ConfigManager
Verwaltet die Konfiguration der Anwendung.

```cpp
namespace VRMusicStudio::Core {
    class ConfigManager {
    public:
        static ConfigManager& getInstance();
        bool initialize();
        void shutdown();
        void update();
        bool loadConfig(const std::string& path);
        bool saveConfig(const std::string& path);
        template<typename T>
        T getValue(const std::string& key, const T& defaultValue);
        template<typename T>
        void setValue(const std::string& key, const T& value);
    };
}
```

### ResourceManager
Verwaltet Ressourcen wie Texturen, Audio-Dateien und Modelle.

```cpp
namespace VRMusicStudio::Core {
    class ResourceManager {
    public:
        static ResourceManager& getInstance();
        bool initialize();
        void shutdown();
        void update();
        bool loadResource(const std::string& path);
        void unloadResource(const std::string& path);
        bool isResourceLoaded(const std::string& path);
        ResourceInfo getResourceInfo(const std::string& path);
    };
}
```

## Audio-Komponenten

### AudioEngine
Die Audio-Engine verwaltet die Audio-Verarbeitung.

```cpp
namespace VRMusicStudio::Audio {
    class AudioEngine {
    public:
        static AudioEngine& getInstance();
        bool initialize();
        void shutdown();
        void update();
        void process(float* input, float* output, size_t numFrames);
        void setSampleRate(float rate);
        void setBufferSize(size_t size);
        void setInputDevice(const std::string& device);
        void setOutputDevice(const std::string& device);
    };
}
```

### AudioPlugin
Basisklasse für Audio-Plugins.

```cpp
namespace VRMusicStudio::Audio {
    class AudioPlugin {
    public:
        virtual ~AudioPlugin() = default;
        virtual bool initialize() = 0;
        virtual void shutdown() = 0;
        virtual void update() = 0;
        virtual void process(float* input, float* output, size_t numFrames) = 0;
    };
}
```

### ReverbPlugin
Implementiert einen Hall-Effekt.

```cpp
namespace VRMusicStudio::Audio {
    class ReverbPlugin : public AudioPlugin {
    public:
        void setRoomSize(float size);
        void setDamping(float damping);
        void setWetLevel(float level);
        void setDryLevel(float level);
        void setWidth(float width);
        void setFreeze(bool freeze);
    };
}
```

### DelayPlugin
Implementiert einen Delay-Effekt.

```cpp
namespace VRMusicStudio::Audio {
    class DelayPlugin : public AudioPlugin {
    public:
        void setDelayTime(float timeInSeconds);
        void setFeedback(float feedback);
        void setMix(float mix);
        void setLowPassCutoff(float cutoff);
        void setHighPassCutoff(float cutoff);
        void setPingPong(bool enabled);
    };
}
```

## VR-Komponenten

### VRManager
Verwaltet die VR-Hardware und -Tracking.

```cpp
namespace VRMusicStudio::VR {
    class VRManager {
    public:
        static VRManager& getInstance();
        bool initialize();
        void shutdown();
        void update();
        bool isDeviceConnected(uint32_t deviceIndex);
        bool getDevicePose(uint32_t deviceIndex, vr::TrackedDevicePose_t* pose);
        bool getControllerState(uint32_t deviceIndex, vr::VRControllerState_t* state);
        void triggerHapticPulse(uint32_t deviceIndex, float duration, float frequency, float amplitude);
        void stopHapticPulse(uint32_t deviceIndex);
    };
}
```

### VRInteraction
Verwaltet VR-Interaktionen wie Controller-Input und Tracking.

```cpp
namespace VRMusicStudio::VR {
    class VRInteraction {
    public:
        VRInteraction(VRManager& vrManager);
        bool initialize();
        void shutdown();
        void update();
        bool isControllerButtonPressed(uint32_t controllerIndex, uint32_t button);
        bool isControllerButtonTouched(uint32_t controllerIndex, uint32_t button);
        glm::vec2 getControllerAxis(uint32_t controllerIndex, uint32_t axis);
        glm::vec3 getControllerPosition(uint32_t controllerIndex);
        glm::quat getControllerRotation(uint32_t controllerIndex);
    };
}
```

## UI-Komponenten

### UI
Verwaltet die 2D-Benutzeroberfläche.

```cpp
namespace VRMusicStudio::UI {
    class UI {
    public:
        static UI& getInstance();
        bool initialize();
        void shutdown();
        void update();
        void render();
        void setWindowSize(int width, int height);
        void setWindowTitle(const std::string& title);
        void setClearColor(const glm::vec4& color);
        void setVSync(bool enabled);
    };
}
```

### VRUI
Verwaltet die 3D-Benutzeroberfläche in VR.

```cpp
namespace VRMusicStudio::UI {
    class VRUI {
    public:
        VRUI(UI& ui, VR::VRInteraction& vrInteraction);
        bool initialize();
        void shutdown();
        void update();
        void render();
        void createPanel(const std::string& id, const glm::vec3& position, const glm::vec2& size);
        void destroyPanel(const std::string& id);
        void setPanelPosition(const std::string& id, const glm::vec3& position);
        void setPanelSize(const std::string& id, const glm::vec2& size);
        void setPanelVisible(const std::string& id, bool visible);
        void setPanelInteractive(const std::string& id, bool interactive);
    };
}
```

## Beispiele

### Audio-Plugin erstellen
```cpp
class MyPlugin : public Audio::AudioPlugin {
public:
    bool initialize() override {
        // Initialisierung
        return true;
    }

    void shutdown() override {
        // Aufräumen
    }

    void update() override {
        // Parameter aktualisieren
    }

    void process(float* input, float* output, size_t numFrames) override {
        // Audio verarbeiten
        for (size_t i = 0; i < numFrames; ++i) {
            output[i] = input[i] * 0.5f;
        }
    }
};
```

### VR-Interaktion implementieren
```cpp
void handleVRInteraction() {
    auto& vrInteraction = VR::VRInteraction::getInstance();
    
    // Controller-Input prüfen
    if (vrInteraction.isControllerButtonPressed(0, vr::k_EButton_SteamVR_Trigger)) {
        // Aktion ausführen
    }

    // Controller-Position abrufen
    glm::vec3 position = vrInteraction.getControllerPosition(0);
    
    // Haptisches Feedback
    vrInteraction.triggerHapticPulse(0, 0.1f, 100.0f, 0.5f);
}
```

### VR-UI erstellen
```cpp
void createVRUI() {
    auto& vrUI = UI::VRUI::getInstance();
    
    // Panel erstellen
    vrUI.createPanel("mainPanel", glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.5f, 0.5f));
    
    // Panel konfigurieren
    vrUI.setPanelColor("mainPanel", glm::vec4(0.2f, 0.2f, 0.2f, 0.8f));
    vrUI.setPanelText("mainPanel", "Hallo VR!");
    vrUI.setPanelFont("mainPanel", "fonts/arial.ttf", 0.02f);
    
    // Panel interaktiv machen
    vrUI.setPanelInteractive("mainPanel", true);
} 