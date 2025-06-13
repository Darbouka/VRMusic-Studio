#pragma once

#include <string>
#include <vector>

namespace VRMusicStudio {
namespace Core {

class ProjectManager {
public:
    static ProjectManager& getInstance();
    void initialize();
    void shutdown();
    void update();
    bool loadProject(const std::string& path);
    bool saveProject(const std::string& path);
    std::vector<std::string> listRecentProjects() const;
};

} // namespace Core
} // namespace VRMusicStudio 