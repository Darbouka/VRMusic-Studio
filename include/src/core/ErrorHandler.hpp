#pragma once

#include <exception>

namespace VRMusicStudio {

class ErrorHandler {
public:
    ErrorHandler() = default;
    virtual ~ErrorHandler() = default;
    bool initialize() { return true; }
    void shutdown() {}
    void update() {}
    bool isInitialized() const { return true; }
    void handleError(const std::exception& e) { (void)e; /* Dummy-Handler */ }
};

} // namespace VRMusicStudio 