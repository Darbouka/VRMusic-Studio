#pragma once

namespace VRMusicStudio {
namespace Core {

class VRManager {
public:
    static VRManager& getInstance();
    void initialize();
    void shutdown();
    void update();
};

} // namespace Core
} // namespace VRMusicStudio 