#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>

namespace VRMusicStudio {
namespace Core {

using EventCallback = std::function<void(const std::string&)>;

class EventSystem {
public:
    static EventSystem& getInstance();
    void initialize();
    void shutdown();
    void update();
    void clear();
    void registerCallback(const std::string& eventName, EventCallback callback);
    void unregisterCallback(const std::string& eventName, const EventCallback& callback);
    void triggerEvent(const std::string& eventName, const std::string& eventData = "");
private:
    EventSystem();
    ~EventSystem();
    EventSystem(const EventSystem&) = delete;
    EventSystem& operator=(const EventSystem&) = delete;
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace Core
} // namespace VRMusicStudio 