#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "../core/Logger.hpp"

namespace VR_DAW {

class UIShader {
public:
    static UIShader& getInstance();

    bool initialize();
    void shutdown();
    bool isInitialized() const;

    // Shader-Programme
    bool loadShaderProgram(const std::string& name, 
                          const std::string& vertexPath,
                          const std::string& fragmentPath);
    void useShaderProgram(const std::string& name);
    
    // Uniform-Setter
    void setUniformMat4(const std::string& name, const glm::mat4& matrix);
    void setUniformVec3(const std::string& name, const glm::vec3& vector);
    void setUniformVec4(const std::string& name, const glm::vec4& vector);
    void setUniformFloat(const std::string& name, float value);
    void setUniformInt(const std::string& name, int value);
    void setUniformBool(const std::string& name, bool value);
    
    // UI-spezifische Shader
    bool loadButtonShader();
    bool loadSliderShader();
    bool loadKnobShader();
    bool loadWaveformShader();
    bool loadSpectrumShader();
    bool loadTextShader();
    bool loadPanelShader();
    
    // Effekt-Shader
    bool loadBlurShader();
    bool loadGlowShader();
    bool loadOutlineShader();
    bool loadGradientShader();
    
    // Material-Shader
    bool loadMetallicShader();
    bool loadGlassShader();
    bool loadHolographicShader();
    
    // Animation-Shader
    bool loadPulseShader();
    bool loadFadeShader();
    bool loadTransitionShader();

private:
    UIShader();
    ~UIShader();
    
    bool compileShader(unsigned int& shader, const std::string& source, GLenum type);
    bool linkProgram(unsigned int& program, unsigned int vertexShader, unsigned int fragmentShader);
    std::string loadShaderSource(const std::string& filePath);
    void checkShaderErrors(unsigned int shader, const std::string& type);
    
    std::mutex mutex;
    std::atomic<bool> initialized;
    Logger logger;
    
    std::map<std::string, unsigned int> shaderPrograms;
    std::string currentProgram;
    
    // UI-Shader-Quellen
    const std::string buttonVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        out vec2 TexCoord;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
            TexCoord = aTexCoord;
        }
    )";
    
    const std::string buttonFragmentShader = R"(
        #version 330 core
        in vec2 TexCoord;
        out vec4 FragColor;
        
        uniform sampler2D texture1;
        uniform vec4 color;
        uniform float hover;
        uniform float pressed;
        
        void main() {
            vec4 texColor = texture(texture1, TexCoord);
            vec4 finalColor = mix(color, color * 1.2, hover);
            finalColor = mix(finalColor, color * 0.8, pressed);
            FragColor = finalColor * texColor;
        }
    )";
    
    // Weitere Shader-Quellen hier...
};

} // namespace VR_DAW 