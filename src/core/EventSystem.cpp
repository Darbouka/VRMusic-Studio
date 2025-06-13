#include "EventSystem.hpp"
#include "Logger.hpp"
#include <algorithm>
#include <stdexcept>

namespace VRMusicStudio {
namespace Core {

struct EventSystem::Impl {
    std::unordered_map<std::string, std::vector<EventCallback>> callbacks;
    std::mutex mutex;
    bool processing = false;
    bool shouldShutdown = false;
    std::condition_variable cv;
};

EventSystem::EventSystem() : pImpl(std::make_unique<Impl>()) {}

EventSystem::~EventSystem() = default;

EventSystem& EventSystem::getInstance() {
    static EventSystem instance;
    return instance;
}

void EventSystem::initialize() {
    try {
        pImpl->processing = false;
        pImpl->shouldShutdown = false;
        Logger::getInstance().info("Event-System erfolgreich initialisiert");
    } catch (const std::exception& e) {
        Logger::getInstance().error("Fehler bei der Initialisierung des Event-Systems: {}", e.what());
        throw;
    }
}

void EventSystem::shutdown() {
    try {
        pImpl->shouldShutdown = true;
        pImpl->cv.notify_all();
        Logger::getInstance().info("Event-System erfolgreich beendet");
    } catch (const std::exception& e) {
        Logger::getInstance().error("Fehler beim Beenden des Event-Systems: {}", e.what());
        throw;
    }
}

void EventSystem::registerCallback(const std::string& eventName, EventCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->callbacks[eventName].push_back(std::move(callback));
}

void EventSystem::unregisterCallback(const std::string& eventName, const EventCallback& callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto& callbacks = pImpl->callbacks[eventName];
    callbacks.erase(
        std::remove_if(callbacks.begin(), callbacks.end(),
            [&](const EventCallback& cb) {
                return cb.target_type() == callback.target_type();
            }),
        callbacks.end()
    );
}

void EventSystem::triggerEvent(const std::string& eventName, const std::string& eventData) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->callbacks.find(eventName);
    if (it != pImpl->callbacks.end()) {
        for (const auto& callback : it->second) {
            try {
                callback(eventData);
            } catch (const std::exception& e) {
                Logger::getInstance().error("Fehler beim Ausführen des Event-Callbacks für '{}': {}", eventName, e.what());
            }
        }
    }
}

void EventSystem::update() {}
void EventSystem::clear() {
    pImpl->callbacks.clear();
}

} // namespace Core
} // namespace VRMusicStudio 