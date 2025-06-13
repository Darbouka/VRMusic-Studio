#pragma once

#include <string>
#include <unordered_map>

namespace VRMusicStudio {
namespace Core {

class ConfigManager {
public:
    static ConfigManager& getInstance();
    void initialize();
    void shutdown();
    void update();
    void setValue(const std::string& key, const std::string& value);
    std::string getValue(const std::string& key) const;
private:
    ConfigManager();
    ~ConfigManager();
    std::unordered_map<std::string, std::string> config_;
};

} // namespace Core
} // namespace VRMusicStudio 