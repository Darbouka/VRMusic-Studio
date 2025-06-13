#include "UIManager.hpp"

namespace VRMusicStudio {
namespace Core {

UIManager& UIManager::getInstance() {
    static UIManager instance;
    return instance;
}

void UIManager::initialize() {}
void UIManager::shutdown() {}
void UIManager::render() {}
void UIManager::handleInput(const std::string&) {}

} // namespace Core
} // namespace VRMusicStudio 