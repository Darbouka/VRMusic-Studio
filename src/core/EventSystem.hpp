#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <memory>
#include <atomic>
#include <condition_variable>

namespace VR_DAW {

class EventSystem {
public:
    using EventCallback = std::function<void(const std::string&, void*)>;
    using EventCallbackPtr = std::shared_ptr<EventCallback>;

    EventSystem();
    ~EventSystem();

    // Event-System initialisieren/beenden
    bool initialize();
    void shutdown();

    // Event-Callbacks registrieren/entfernen
    void registerCallback(const std::string& eventName, EventCallback callback);
    void unregisterCallback(const std::string& eventName, EventCallback callback);

    // Events auslösen
    void triggerEvent(const std::string& eventName, void* data = nullptr);

    // Event-System aktualisieren
    void update();

    // Event-Queue leeren
    void clearEventQueue();

private:
    struct Event {
        std::string name;
        void* data;
    };

    std::unordered_map<std::string, std::vector<EventCallbackPtr>> callbacks;
    std::vector<Event> eventQueue;
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<bool> initialized;
    std::atomic<bool> processing;
    std::atomic<bool> shouldShutdown;

    void processEvent(const Event& event);
    bool isEventValid(const Event& event) const;
};

} // namespace VR_DAW 