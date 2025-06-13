#include "ProjectManager.hpp"

namespace VRMusicStudio {
namespace Core {

ProjectManager& ProjectManager::getInstance() {
    static ProjectManager instance;
    return instance;
}

void ProjectManager::initialize() {}
void ProjectManager::shutdown() {}
bool ProjectManager::loadProject(const std::string&) { return true; }
bool ProjectManager::saveProject(const std::string&) { return true; }
std::vector<std::string> ProjectManager::listRecentProjects() const { return {}; }

} // namespace Core
} // namespace VRMusicStudio 