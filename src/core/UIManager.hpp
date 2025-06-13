#pragma once

#include <string>

namespace VRMusicStudio {
namespace Core {

class UIManager {
public:
    static UIManager& getInstance();
    void initialize();
    void shutdown();
    void update();
    void render();
    void handleInput(const std::string& input);
};

} // namespace Core
} // namespace VRMusicStudio 