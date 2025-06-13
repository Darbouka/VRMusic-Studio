#include "ConfigManager.hpp"

namespace VRMusicStudio {
namespace Core {

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

void ConfigManager::initialize() {}
void ConfigManager::shutdown() {}
void ConfigManager::setValue(const std::string& key, const std::string& value) {
    config_[key] = value;
}
std::string ConfigManager::getValue(const std::string& key) const {
    auto it = config_.find(key);
    return it != config_.end() ? it->second : "";
}

} // namespace Core
} // namespace VRMusicStudio 