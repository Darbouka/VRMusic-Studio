#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

namespace VRMusicStudio {

class HologramRenderer {
public:
    HologramRenderer();
    ~HologramRenderer();
    
    bool initialize(bool useVulkan = false);
    
    void createRenderTarget(const std::string& name, uint32_t width, uint32_t height);
    void createShader(const std::string& name, const std::string& vertexSource,
                     const std::string& fragmentSource);
    void createMaterial(const std::string& name, const std::string& shaderName,
                       const std::map<std::string, float>& parameters,
                       const std::map<std::string, uint32_t>& textures);
    
    void setRenderTarget(const std::string& name);
    void setShader(const std::string& name);
    void setMaterial(const std::string& name);
    
    void setUniform(const std::string& name, float value);
    void setUniform(const std::string& name, const glm::vec3& value);
    void setUniform(const std::string& name, const glm::mat4& value);
    void setTexture(const std::string& name, uint32_t texture);
    
    void beginFrame();
    void endFrame();
    
    void renderHologram(const std::vector<float>& vertices,
                       const std::vector<float>& normals,
                       const std::vector<float>& texcoords,
                       const std::vector<uint32_t>& indices);
    void renderParticleSystem(const std::vector<float>& positions,
                            const std::vector<float>& velocities,
                            const std::vector<float>& colors,
                            const std::vector<float>& sizes);
    void renderLightField(const std::vector<float>& positions,
                         const std::vector<float>& colors,
                         const std::vector<float>& intensities);
    
    void setRenderCallback(std::function<void(const std::vector<float>&)> callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace VRMusicStudio 