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

<<<<<<< HEAD
class EventSystem {
public:
    using EventCallback = std::function<void(const std::string&, void*)>;
    using EventCallbackPtr = std::shared_ptr<EventCallback>;

=======
using EventCallback = std::function<void(const std::string&)>;

class EventSystem {
public:
>>>>>>> 0dff1c4 (init 2)
    EventSystem();
    ~EventSystem();

    // Event-System initialisieren/beenden
    bool initialize();
    void shutdown();

    // Event-Callbacks registrieren/entfernen
    void registerCallback(const std::string& eventName, EventCallback callback);
    void unregisterCallback(const std::string& eventName, EventCallback callback);

    // Events auslösen
<<<<<<< HEAD
    void triggerEvent(const std::string& eventName, void* data = nullptr);
=======
    void triggerEvent(const std::string& eventName, const std::string& data = "");
>>>>>>> 0dff1c4 (init 2)

    // Event-System aktualisieren
    void update();

    // Event-Queue leeren
    void clearEventQueue();

private:
    struct Event {
        std::string name;
<<<<<<< HEAD
        void* data;
    };

    std::unordered_map<std::string, std::vector<EventCallbackPtr>> callbacks;
=======
        std::string data;
    };

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    std::unordered_map<std::string, std::vector<EventCallback>> callbacks;
>>>>>>> 0dff1c4 (init 2)
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