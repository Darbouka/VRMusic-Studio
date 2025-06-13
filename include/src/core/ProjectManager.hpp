#pragma once

namespace VRMusicStudio {

class ProjectManager {
public:
    ProjectManager() = default;
    virtual ~ProjectManager() = default;
    bool initialize() { return true; }
    void shutdown() {}
    void update() {}
    bool isInitialized() const { return true; }
};

} // namespace VRMusicStudio 