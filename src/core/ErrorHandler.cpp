#include "ErrorHandler.hpp"

namespace VRMusicStudio {
namespace Core {

ErrorHandler& ErrorHandler::getInstance() {
    static ErrorHandler instance;
    return instance;
}

void ErrorHandler::initialize() {}
void ErrorHandler::shutdown() {}
void ErrorHandler::setErrorCallback(std::function<void(const std::string&)> callback) {
    errorCallback_ = callback;
}
void ErrorHandler::handleError(const std::string& errorMessage) {
    if (errorCallback_) errorCallback_(errorMessage);
}

} // namespace Core
} // namespace VRMusicStudio 