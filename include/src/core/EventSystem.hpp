#pragma once

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <condition_variable>

namespace VRMusicStudio {

class Logger;

class EventSystem {
public:
    using EventCallback = std::function<void(const std::string&)>;

    EventSystem();
    ~EventSystem();

    // Event-System initialisieren/beenden
    void initialize();
    void shutdown();

    // Event-Callbacks registrieren/entfernen
    void registerCallback(const std::string& eventName, EventCallback callback);
    void unregisterCallback(const std::string& eventName, const EventCallback& callback);

    // Events auslösen
    void triggerEvent(const std::string& eventName, const std::string& eventData);

    // Event-System aktualisieren
    void update();

    // Event-Queue leeren
    void clearEventQueue();

    void processEvent(const Event& event);
    bool isEventValid(const Event& event) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<Logger> logger;

    std::unordered_map<std::string, std::vector<EventCallback>> callbacks;
    std::vector<Event> eventQueue;
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<bool> initialized;
    std::atomic<bool> processing;
    std::atomic<bool> shouldShutdown;
};

} // namespace VRMusicStudio 