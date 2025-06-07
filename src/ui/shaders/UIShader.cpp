#include "UIShader.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <glad/glad.h>

namespace VR_DAW {

UIShader& UIShader::getInstance() {
    static UIShader instance;
    return instance;
}

UIShader::UIShader()
    : initialized(false)
    , logger("UIShader") {
}

UIShader::~UIShader() {
    shutdown();
}

bool UIShader::initialize() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (initialized) {
        return true;
    }

    try {
        // Standard-UI-Shader laden
        if (!loadButtonShader() ||
            !loadSliderShader() ||
            !loadKnobShader() ||
            !loadWaveformShader() ||
            !loadSpectrumShader() ||
            !loadTextShader() ||
            !loadPanelShader()) {
            logger.error("Fehler beim Laden der Standard-UI-Shader");
            return false;
        }

        // Effekt-Shader laden
        if (!loadBlurShader() ||
            !loadGlowShader() ||
            !loadOutlineShader() ||
            !loadGradientShader()) {
            logger.error("Fehler beim Laden der Effekt-Shader");
            return false;
        }

        // Material-Shader laden
        if (!loadMetallicShader() ||
            !loadGlassShader() ||
            !loadHolographicShader()) {
            logger.error("Fehler beim Laden der Material-Shader");
            return false;
        }

        // Animation-Shader laden
        if (!loadPulseShader() ||
            !loadFadeShader() ||
            !loadTransitionShader()) {
            logger.error("Fehler beim Laden der Animation-Shader");
            return false;
        }

        initialized = true;
        logger.info("UI-Shader erfolgreich initialisiert");
        return true;
    }
    catch (const std::exception& e) {
        logger.error("Fehler bei der UI-Shader-Initialisierung: {}", e.what());
        return false;
    }
}

void UIShader::shutdown() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        for (const auto& [name, program] : shaderPrograms) {
            glDeleteProgram(program);
        }
        shaderPrograms.clear();
        
        initialized = false;
        logger.info("UI-Shader erfolgreich beendet");
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Beenden der UI-Shader: {}", e.what());
    }
}

bool UIShader::loadShaderProgram(const std::string& name,
                                const std::string& vertexPath,
                                const std::string& fragmentPath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return false;
    }

    try {
        std::string vertexCode = loadShaderSource(vertexPath);
        std::string fragmentCode = loadShaderSource(fragmentPath);
        
        unsigned int vertexShader, fragmentShader;
        if (!compileShader(vertexShader, vertexCode, GL_VERTEX_SHADER) ||
            !compileShader(fragmentShader, fragmentCode, GL_FRAGMENT_SHADER)) {
            return false;
        }
        
        unsigned int program;
        if (!linkProgram(program, vertexShader, fragmentShader)) {
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return false;
        }
        
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
        shaderPrograms[name] = program;
        logger.info("Shader-Programm geladen: {}", name);
        return true;
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Laden des Shader-Programms: {}", e.what());
        return false;
    }
}

void UIShader::useShaderProgram(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }

    try {
        auto it = shaderPrograms.find(name);
        if (it != shaderPrograms.end()) {
            glUseProgram(it->second);
            currentProgram = name;
        }
    }
    catch (const std::exception& e) {
        logger.error("Fehler beim Verwenden des Shader-Programms: {}", e.what());
    }
}

bool UIShader::loadButtonShader() {
    return loadShaderProgram("button", "shaders/button.vert", "shaders/button.frag");
}

bool UIShader::loadSliderShader() {
    return loadShaderProgram("slider", "shaders/slider.vert", "shaders/slider.frag");
}

bool UIShader::loadKnobShader() {
    return loadShaderProgram("knob", "shaders/knob.vert", "shaders/knob.frag");
}

bool UIShader::loadWaveformShader() {
    return loadShaderProgram("waveform", "shaders/waveform.vert", "shaders/waveform.frag");
}

bool UIShader::loadSpectrumShader() {
    return loadShaderProgram("spectrum", "shaders/spectrum.vert", "shaders/spectrum.frag");
}

bool UIShader::loadTextShader() {
    return loadShaderProgram("text", "shaders/text.vert", "shaders/text.frag");
}

bool UIShader::loadPanelShader() {
    return loadShaderProgram("panel", "shaders/panel.vert", "shaders/panel.frag");
}

bool UIShader::loadBlurShader() {
    return loadShaderProgram("blur", "shaders/blur.vert", "shaders/blur.frag");
}

bool UIShader::loadGlowShader() {
    return loadShaderProgram("glow", "shaders/glow.vert", "shaders/glow.frag");
}

bool UIShader::loadOutlineShader() {
    return loadShaderProgram("outline", "shaders/outline.vert", "shaders/outline.frag");
}

bool UIShader::loadGradientShader() {
    return loadShaderProgram("gradient", "shaders/gradient.vert", "shaders/gradient.frag");
}

bool UIShader::loadMetallicShader() {
    return loadShaderProgram("metallic", "shaders/metallic.vert", "shaders/metallic.frag");
}

bool UIShader::loadGlassShader() {
    return loadShaderProgram("glass", "shaders/glass.vert", "shaders/glass.frag");
}

bool UIShader::loadHolographicShader() {
    return loadShaderProgram("holographic", "shaders/holographic.vert", "shaders/holographic.frag");
}

bool UIShader::loadPulseShader() {
    return loadShaderProgram("pulse", "shaders/pulse.vert", "shaders/pulse.frag");
}

bool UIShader::loadFadeShader() {
    return loadShaderProgram("fade", "shaders/fade.vert", "shaders/fade.frag");
}

bool UIShader::loadTransitionShader() {
    return loadShaderProgram("transition", "shaders/transition.vert", "shaders/transition.frag");
}

bool UIShader::compileShader(unsigned int& shader, const std::string& source, GLenum type) {
    shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    checkShaderErrors(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
    return true;
}

bool UIShader::linkProgram(unsigned int& program, unsigned int vertexShader, unsigned int fragmentShader) {
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        logger.error("Shader-Programm-Link-Fehler: {}", infoLog);
        return false;
    }
    return true;
}

std::string UIShader::loadShaderSource(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Shader-Datei konnte nicht geöffnet werden: " + filePath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void UIShader::checkShaderErrors(unsigned int shader, const std::string& type) {
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        logger.error("Shader-Kompilierungsfehler ({}) : {}", type, infoLog);
    }
}

void UIShader::setUniformMat4(const std::string& name, const glm::mat4& matrix) {
    if (!currentProgram.empty()) {
        auto it = shaderPrograms.find(currentProgram);
        if (it != shaderPrograms.end()) {
            glUniformMatrix4fv(glGetUniformLocation(it->second, name.c_str()), 
                              1, GL_FALSE, glm::value_ptr(matrix));
        }
    }
}

void UIShader::setUniformVec3(const std::string& name, const glm::vec3& vector) {
    if (!currentProgram.empty()) {
        auto it = shaderPrograms.find(currentProgram);
        if (it != shaderPrograms.end()) {
            glUniform3fv(glGetUniformLocation(it->second, name.c_str()), 
                        1, glm::value_ptr(vector));
        }
    }
}

void UIShader::setUniformVec4(const std::string& name, const glm::vec4& vector) {
    if (!currentProgram.empty()) {
        auto it = shaderPrograms.find(currentProgram);
        if (it != shaderPrograms.end()) {
            glUniform4fv(glGetUniformLocation(it->second, name.c_str()), 
                        1, glm::value_ptr(vector));
        }
    }
}

void UIShader::setUniformFloat(const std::string& name, float value) {
    if (!currentProgram.empty()) {
        auto it = shaderPrograms.find(currentProgram);
        if (it != shaderPrograms.end()) {
            glUniform1f(glGetUniformLocation(it->second, name.c_str()), value);
        }
    }
}

void UIShader::setUniformInt(const std::string& name, int value) {
    if (!currentProgram.empty()) {
        auto it = shaderPrograms.find(currentProgram);
        if (it != shaderPrograms.end()) {
            glUniform1i(glGetUniformLocation(it->second, name.c_str()), value);
        }
    }
}

void UIShader::setUniformBool(const std::string& name, bool value) {
    if (!currentProgram.empty()) {
        auto it = shaderPrograms.find(currentProgram);
        if (it != shaderPrograms.end()) {
            glUniform1i(glGetUniformLocation(it->second, name.c_str()), value);
        }
    }
}

} // namespace VR_DAW 