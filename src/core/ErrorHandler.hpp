#pragma once

#include <exception>

namespace VRMusicStudio {
namespace Core {

class ErrorHandler {
public:
    static ErrorHandler& getInstance();
    void initialize();
    void shutdown();
    void update();
    void handleError(const std::exception& e);
};

} // namespace Core
} // namespace VRMusicStudio 