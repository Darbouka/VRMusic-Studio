#pragma once

#include <string>

namespace VRMusicStudio {
namespace Core {

class Version {
public:
    static std::string getVersion();
    static std::string getBuildDate();
};

} // namespace Core
} // namespace VRMusicStudio 