#include "EventSystem.hpp"
#include "Logger.hpp"
#include <chrono>

namespace VR_DAW {

EventSystem::EventSystem()
    : initialized(false)
    , processing(false)
    , shouldShutdown(false) {
}

EventSystem::~EventSystem() {
    shutdown();
}

bool EventSystem::initialize() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (initialized) {
        return true;
    }

    try {
        callbacks.clear();
        eventQueue.clear();
        initialized = true;
        processing = false;
        shouldShutdown = false;
        Logger("EventSystem").info("Event-System erfolgreich initialisiert");
        return true;
    }
    catch (const std::exception& e) {
        Logger("EventSystem").error("Fehler bei der Initialisierung des Event-Systems: {}", e.what());
        return false;
    }
}

void EventSystem::shutdown() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (!initialized) {
            return;
        }
        shouldShutdown = true;
    }
    
    cv.notify_all();

    try {
        std::lock_guard<std::mutex> lock(mutex);
        callbacks.clear();
        eventQueue.clear();
        initialized = false;
        processing = false;
        Logger("EventSystem").info("Event-System erfolgreich beendet");
    }
    catch (const std::exception& e) {
        Logger("EventSystem").error("Fehler beim Beenden des Event-Systems: {}", e.what());
    }
}

void EventSystem::registerCallback(const std::string& eventName, EventCallback callback) {
    if (!initialized || shouldShutdown) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex);
    
    try {
        auto callbackPtr = std::make_shared<EventCallback>(callback);
        callbacks[eventName].push_back(callbackPtr);
        Logger("EventSystem").debug("Callback für Event '{}' registriert", eventName);
    }
    catch (const std::exception& e) {
        Logger("EventSystem").error("Fehler beim Registrieren des Callbacks für Event '{}': {}", eventName, e.what());
    }
}

void EventSystem::unregisterCallback(const std::string& eventName, EventCallback callback) {
    if (!initialized || shouldShutdown) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex);
    
    try {
        auto it = callbacks.find(eventName);
        if (it != callbacks.end()) {
            auto& callbackList = it->second;
            callbackList.erase(
                std::remove_if(callbackList.begin(), callbackList.end(),
                    [&callback](const EventCallbackPtr& ptr) {
                        return *ptr == callback;
                    }
                ),
                callbackList.end()
            );
            Logger("EventSystem").debug("Callback für Event '{}' entfernt", eventName);
        }
    }
    catch (const std::exception& e) {
        Logger("EventSystem").error("Fehler beim Entfernen des Callbacks für Event '{}': {}", eventName, e.what());
    }
}

void EventSystem::triggerEvent(const std::string& eventName, void* data) {
    if (!initialized || shouldShutdown) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex);
    
    try {
        Event event{eventName, data};
        if (isEventValid(event)) {
            eventQueue.push_back(event);
            cv.notify_one();
            Logger("EventSystem").debug("Event '{}' zur Queue hinzugefügt", eventName);
        }
    }
    catch (const std::exception& e) {
        Logger("EventSystem").error("Fehler beim Auslösen des Events '{}': {}", eventName, e.what());
    }
}

void EventSystem::update() {
    if (!initialized || shouldShutdown) {
        return;
    }

    std::vector<Event> eventsToProcess;
    
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (eventQueue.empty()) {
            return;
        }
        
        processing = true;
        eventsToProcess = std::move(eventQueue);
    }

    for (const auto& event : eventsToProcess) {
        if (shouldShutdown) {
            break;
        }
        processEvent(event);
    }

    processing = false;
}

void EventSystem::clearEventQueue() {
    if (!initialized || shouldShutdown) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex);
    eventQueue.clear();
}

void EventSystem::processEvent(const Event& event) {
    if (!initialized || shouldShutdown) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex);
    
    try {
        auto it = callbacks.find(event.name);
        if (it != callbacks.end()) {
            for (const auto& callback : it->second) {
                if (shouldShutdown) {
                    break;
                }
                (*callback)(event.name, event.data);
            }
        }
    }
    catch (const std::exception& e) {
        Logger("EventSystem").error("Fehler beim Verarbeiten des Events '{}': {}", event.name, e.what());
    }
}

bool EventSystem::isEventValid(const Event& event) const {
    return !event.name.empty() && callbacks.find(event.name) != callbacks.end();
}

} // namespace VR_DAW 