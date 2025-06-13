#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace VRMusicStudio {

class HologramEngine {
public:
    HologramEngine();
    ~HologramEngine();
    
    bool initialize();
    
    void createHologram(const std::string& name, const std::string& type,
                       const std::map<std::string, float>& parameters);
    void createParticleSystem(const std::string& name, const std::string& type,
                            const std::map<std::string, float>& parameters);
    void createLightField(const std::string& name, const std::string& type,
                         const std::map<std::string, float>& parameters);
    
    void setHologramParameter(const std::string& name, const std::string& parameter, float value);
    void setParticleSystemParameter(const std::string& name, const std::string& parameter, float value);
    void setLightFieldParameter(const std::string& name, const std::string& parameter, float value);
    
    void setHologramEnabled(const std::string& name, bool enabled);
    void setParticleSystemEnabled(const std::string& name, bool enabled);
    void setLightFieldEnabled(const std::string& name, bool enabled);
    
    void updateHologram(const std::string& name, const glm::vec3& position,
                       const glm::quat& rotation, const glm::vec3& scale);
    void updateParticleSystem(const std::string& name,
                            const std::vector<glm::vec3>& positions,
                            const std::vector<glm::vec3>& velocities,
                            const std::vector<glm::vec4>& colors,
                            const std::vector<float>& sizes);
    void updateLightField(const std::string& name, const glm::vec3& position,
                         const glm::quat& rotation, const glm::vec3& color);
    
    void processHolograms();
    
    void setHologramCallback(std::function<void(const std::vector<float>&)> callback);
    void setParticleCallback(std::function<void(const std::vector<float>&)> callback);
    void setLightFieldCallback(std::function<void(const std::vector<float>&)> callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace VRMusicStudio 