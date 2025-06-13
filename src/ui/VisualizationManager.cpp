#include "VisualizationManager.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace VRMusicStudio {

VisualizationManager::VisualizationManager() {
    initialize();
}

VisualizationManager::~VisualizationManager() {
    shutdown();
}

bool VisualizationManager::initialize() {
    // Shader initialisieren
    if (!initializeShaders()) {
        return false;
    }

    // VAO und VBO für Visualisierungen erstellen
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // Standard-Farben und Materialien
    colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),  // Rot
        glm::vec3(0.0f, 1.0f, 0.0f),  // Grün
        glm::vec3(0.0f, 0.0f, 1.0f),  // Blau
        glm::vec3(1.0f, 1.0f, 0.0f),  // Gelb
        glm::vec3(1.0f, 0.0f, 1.0f),  // Magenta
        glm::vec3(0.0f, 1.0f, 1.0f)   // Cyan
    };

    return true;
}

void VisualizationManager::shutdown() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
}

void VisualizationManager::update() {
    // Aktualisiere alle Visualisierungen
    updateStepSequencer();
    updatePatternVisualization();
    updateAudioVisualization();
    updateVRAudioVisualization();
    updateKIVisualization();
}

void VisualizationManager::render() {
    glUseProgram(shaderProgram);

    // Setze View und Projection
    glm::mat4 view = getViewMatrix();
    glm::mat4 projection = getProjectionMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Rendere alle Visualisierungen
    renderStepSequencer();
    renderPatternVisualization();
    renderAudioVisualization();
    renderVRAudioVisualization();
    renderKIVisualization();
}

void VisualizationManager::updateStepSequencer() {
    // Aktualisiere Step-Sequencer Visualisierung
    for (unsigned int step = 0; step < numSteps; ++step) {
        for (unsigned int track = 0; track < numTracks; ++track) {
            bool active = getStepState(step, track);
            float intensity = active ? 1.0f : 0.2f;
            stepColors[step][track] = colors[track] * intensity;
        }
    }
}

void VisualizationManager::updatePatternVisualization() {
    // Aktualisiere Pattern-Visualisierung
    for (const auto& [name, pattern] : patterns) {
        for (unsigned int step = 0; step < pattern.steps.size(); ++step) {
            for (unsigned int track = 0; track < pattern.steps[step].size(); ++track) {
                bool active = pattern.steps[step][track] > 0;
                float intensity = active ? 1.0f : 0.2f;
                patternColors[name][step][track] = colors[track] * intensity;
            }
        }
    }
}

void VisualizationManager::updateAudioVisualization() {
    // Aktualisiere Audio-Visualisierung (Waveform, Spektrum, etc.)
    for (const auto& [name, data] : audioData) {
        // FFT oder Waveform-Analyse
        std::vector<float> spectrum = performFFT(data);
        audioSpectrums[name] = spectrum;
    }
}

void VisualizationManager::updateVRAudioVisualization() {
    // Aktualisiere VR-Audio-Visualisierung (3D-Positionierung)
    for (const auto& [name, position] : vrAudioPositions) {
        // Berechne 3D-Position und Ausrichtung
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        vrAudioModels[name] = model;
    }
}

void VisualizationManager::updateKIVisualization() {
    // Aktualisiere KI-Visualisierung (Pattern-Vorschläge, etc.)
    for (const auto& [name, data] : kiData) {
        // KI-Daten visualisieren
        kiVisualizations[name] = processKIData(data);
    }
}

void VisualizationManager::renderStepSequencer() {
    glBindVertexArray(vao);
    
    // Rendere Steps
    for (unsigned int step = 0; step < numSteps; ++step) {
        for (unsigned int track = 0; track < numTracks; ++track) {
            glm::vec3 position = calculateStepPosition(step, track);
            glm::vec3 color = stepColors[step][track];
            
            // Setze Model-Matrix
            glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            
            // Setze Farbe
            glUniform3fv(colorLoc, 1, glm::value_ptr(color));
            
            // Rendere Step
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
}

void VisualizationManager::renderPatternVisualization() {
    glBindVertexArray(vao);
    
    // Rendere Patterns
    for (const auto& [name, pattern] : patterns) {
        for (unsigned int step = 0; step < pattern.steps.size(); ++step) {
            for (unsigned int track = 0; track < pattern.steps[step].size(); ++track) {
                glm::vec3 position = calculatePatternPosition(name, step, track);
                glm::vec3 color = patternColors[name][step][track];
                
                // Setze Model-Matrix
                glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                
                // Setze Farbe
                glUniform3fv(colorLoc, 1, glm::value_ptr(color));
                
                // Rendere Pattern-Step
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
    }
}

void VisualizationManager::renderAudioVisualization() {
    glBindVertexArray(vao);
    
    // Rendere Audio-Visualisierungen
    for (const auto& [name, spectrum] : audioSpectrums) {
        // Waveform oder Spektrum rendern
        for (size_t i = 0; i < spectrum.size(); ++i) {
            float value = spectrum[i];
            glm::vec3 position = calculateAudioPosition(name, i, value);
            glm::vec3 color = calculateAudioColor(value);
            
            // Setze Model-Matrix
            glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            
            // Setze Farbe
            glUniform3fv(colorLoc, 1, glm::value_ptr(color));
            
            // Rendere Audio-Bar
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
}

void VisualizationManager::renderVRAudioVisualization() {
    glBindVertexArray(vao);
    
    // Rendere VR-Audio-Visualisierungen
    for (const auto& [name, model] : vrAudioModels) {
        // Setze Model-Matrix
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        
        // Setze Farbe
        glm::vec3 color = colors[0]; // Standardfarbe
        glUniform3fv(colorLoc, 1, glm::value_ptr(color));
        
        // Rendere VR-Audio-Source
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void VisualizationManager::renderKIVisualization() {
    glBindVertexArray(vao);
    
    // Rendere KI-Visualisierungen
    for (const auto& [name, visualization] : kiVisualizations) {
        // KI-Daten visualisieren
        for (const auto& element : visualization) {
            glm::vec3 position = element.position;
            glm::vec3 color = element.color;
            
            // Setze Model-Matrix
            glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            
            // Setze Farbe
            glUniform3fv(colorLoc, 1, glm::value_ptr(color));
            
            // Rendere KI-Element
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
}

bool VisualizationManager::initializeShaders() {
    // Vertex Shader
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

    // Fragment Shader
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec3 color;
        void main() {
            FragColor = vec4(color, 1.0);
        }
    )";

    // Shader kompilieren und linken
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Uniform Locations
    modelLoc = glGetUniformLocation(shaderProgram, "model");
    viewLoc = glGetUniformLocation(shaderProgram, "view");
    projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    colorLoc = glGetUniformLocation(shaderProgram, "color");

    // Cleanup
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

glm::mat4 VisualizationManager::getViewMatrix() const {
    // VR-View-Matrix aus der VR-Engine holen
    return glm::mat4(1.0f); // Dummy
}

glm::mat4 VisualizationManager::getProjectionMatrix() const {
    // VR-Projektions-Matrix aus der VR-Engine holen
    return glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
}

glm::vec3 VisualizationManager::calculateStepPosition(unsigned int step, unsigned int track) const {
    float x = step * stepWidth;
    float y = track * trackHeight;
    return glm::vec3(x, y, 0.0f);
}

glm::vec3 VisualizationManager::calculatePatternPosition(const std::string& name, unsigned int step, unsigned int track) const {
    float x = step * stepWidth;
    float y = track * trackHeight;
    float z = 0.1f; // Leicht versetzt
    return glm::vec3(x, y, z);
}

glm::vec3 VisualizationManager::calculateAudioPosition(const std::string& name, size_t index, float value) const {
    float x = index * audioBarWidth;
    float y = value * audioBarHeight;
    return glm::vec3(x, y, 0.0f);
}

glm::vec3 VisualizationManager::calculateAudioColor(float value) const {
    // Farbverlauf basierend auf Amplitude
    return glm::mix(colors[0], colors[1], value);
}

std::vector<float> VisualizationManager::performFFT(const std::vector<float>& data) const {
    // FFT-Implementierung
    return std::vector<float>(data.size(), 0.0f); // Dummy
}

std::vector<VisualizationElement> VisualizationManager::processKIData(const std::vector<float>& data) const {
    // KI-Daten-Verarbeitung
    return std::vector<VisualizationElement>(); // Dummy
}

bool VisualizationManager::getStepState(unsigned int step, unsigned int track) const {
    // Step-Status aus dem Sequencer holen
    return false; // Dummy
}

} // namespace VRMusicStudio 