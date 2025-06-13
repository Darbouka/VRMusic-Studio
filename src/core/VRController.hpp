#pragma once

#include <string>
#include <vector>

namespace VRMusicStudio {
namespace Core {

class VRController {
public:
    static VRController& getInstance();
    
    VRController();
    ~VRController();

    void initialize();
    void shutdown();
    void update();

    std::string getControllerName() const;
    std::vector<float> getPosition() const;
    std::vector<float> getOrientation() const;
};

} // namespace Core
} // namespace VRMusicStudio 