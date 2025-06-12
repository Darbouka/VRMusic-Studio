#include "EventSystem.hpp"
#include "Logger.hpp"
#include <chrono>
<<<<<<< HEAD

namespace VR_DAW {

EventSystem::EventSystem()
    : initialized(false)
=======
#include <algorithm>
#include <mutex>

namespace VR_DAW {

struct EventSystem::Impl {
    std::unordered_map<std::string, std::vector<EventCallback>> callbacks;
    std::vector<Event> eventQueue;
    std::mutex mutex;
};

EventSystem::EventSystem()
    : pImpl(std::make_unique<Impl>())
    , callbacks()
    , eventQueue()
    , mutex()
    , cv()
    , initialized(false)
>>>>>>> 0dff1c4 (init 2)
    , processing(false)
    , shouldShutdown(false) {
}

EventSystem::~EventSystem() {
    shutdown();
}

bool EventSystem::initialize() {
<<<<<<< HEAD
    std::lock_guard<std::mutex> lock(mutex);
=======
    std::lock_guard<std::mutex> lock(pImpl->mutex);
>>>>>>> 0dff1c4 (init 2)
    
    if (initialized) {
        return true;
    }

    try {
<<<<<<< HEAD
        callbacks.clear();
        eventQueue.clear();
=======
        pImpl->callbacks.clear();
        pImpl->eventQueue.clear();
>>>>>>> 0dff1c4 (init 2)
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
<<<<<<< HEAD
        std::lock_guard<std::mutex> lock(mutex);
=======
        std::lock_guard<std::mutex> lock(pImpl->mutex);
>>>>>>> 0dff1c4 (init 2)
        if (!initialized) {
            return;
        }
        shouldShutdown = true;
    }
    
    cv.notify_all();

    try {
<<<<<<< HEAD
        std::lock_guard<std::mutex> lock(mutex);
        callbacks.clear();
        eventQueue.clear();
=======
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->callbacks.clear();
        pImpl->eventQueue.clear();
>>>>>>> 0dff1c4 (init 2)
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

<<<<<<< HEAD
    std::lock_guard<std::mutex> lock(mutex);
    
    try {
        auto callbackPtr = std::make_shared<EventCallback>(callback);
        callbacks[eventName].push_back(callbackPtr);
=======
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        pImpl->callbacks[eventName].push_back(callback);
>>>>>>> 0dff1c4 (init 2)
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

<<<<<<< HEAD
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
=======
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        auto& callbacks = pImpl->callbacks[eventName];
        callbacks.erase(
            std::remove_if(callbacks.begin(), callbacks.end(),
                [&](const EventCallback& cb) {
                    return cb.target_type() == callback.target_type();
                }),
            callbacks.end()
        );
        Logger("EventSystem").debug("Callback für Event '{}' entfernt", eventName);
>>>>>>> 0dff1c4 (init 2)
    }
    catch (const std::exception& e) {
        Logger("EventSystem").error("Fehler beim Entfernen des Callbacks für Event '{}': {}", eventName, e.what());
    }
}

<<<<<<< HEAD
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

=======
void EventSystem::triggerEvent(const std::string& eventName, const std::string& data) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->eventQueue.push_back(Event{eventName, data});
}

void EventSystem::update() {
    std::vector<Event> eventsToProcess;
    {
        std::unique_lock<std::mutex> lock(pImpl->mutex);
        if (pImpl->eventQueue.empty()) {
            return;
        }
        processing = true;
        eventsToProcess = std::move(pImpl->eventQueue);
        pImpl->eventQueue.clear();
    }
    for (const auto& event : eventsToProcess) {
        auto it = pImpl->callbacks.find(event.name);
        if (it != pImpl->callbacks.end()) {
            for (const auto& callback : it->second) {
                if (shouldShutdown) return;
                callback(event.data);
            }
        }
    }
>>>>>>> 0dff1c4 (init 2)
    processing = false;
}

void EventSystem::clearEventQueue() {
    if (!initialized || shouldShutdown) {
        return;
    }

<<<<<<< HEAD
    std::lock_guard<std::mutex> lock(mutex);
    eventQueue.clear();
=======
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->eventQueue.clear();
>>>>>>> 0dff1c4 (init 2)
}

void EventSystem::processEvent(const Event& event) {
    if (!initialized || shouldShutdown) {
        return;
    }

<<<<<<< HEAD
    std::lock_guard<std::mutex> lock(mutex);
    
    try {
        auto it = callbacks.find(event.name);
        if (it != callbacks.end()) {
=======
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        auto it = pImpl->callbacks.find(event.name);
        if (it != pImpl->callbacks.end()) {
>>>>>>> 0dff1c4 (init 2)
            for (const auto& callback : it->second) {
                if (shouldShutdown) {
                    break;
                }
<<<<<<< HEAD
                (*callback)(event.name, event.data);
=======
                callback(event.data);
>>>>>>> 0dff1c4 (init 2)
            }
        }
    }
    catch (const std::exception& e) {
        Logger("EventSystem").error("Fehler beim Verarbeiten des Events '{}': {}", event.name, e.what());
    }
}

bool EventSystem::isEventValid(const Event& event) const {
<<<<<<< HEAD
    return !event.name.empty() && callbacks.find(event.name) != callbacks.end();
=======
    return !event.name.empty() && pImpl->callbacks.find(event.name) != pImpl->callbacks.end();
>>>>>>> 0dff1c4 (init 2)
}

} // namespace VR_DAW 