#include "VRManager.hpp"

namespace VRMusicStudio {
namespace Core {

VRManager& VRManager::getInstance() {
    static VRManager instance;
    return instance;
}

void VRManager::initialize() {}
void VRManager::shutdown() {}
void VRManager::update() {}

} // namespace Core
} // namespace VRMusicStudio 