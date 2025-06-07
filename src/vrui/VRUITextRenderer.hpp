#pragma once

#include <string>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace VR_DAW {

struct Character {
    unsigned int TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    unsigned int Advance;
};

class VRUITextRenderer {
public:
    VRUITextRenderer();
    ~VRUITextRenderer();

    // Initialisierung
    bool initialize();
    void shutdown();

    // Text-Rendering
    void renderText(const std::string& text, float x, float y, float scale, const glm::vec4& color);
    void setFont(const std::string& fontPath, unsigned int fontSize);
    void setProjection(const glm::mat4& projection);
    void setView(const glm::mat4& view);

private:
    // OpenGL-Zustand
    unsigned int vao;
    unsigned int vbo;
    unsigned int shaderProgram;

    // Freetype
    FT_Library ft;
    FT_Face face;
    std::map<char, Character> characters;

    // Matrizen
    glm::mat4 projection;
    glm::mat4 view;

    // Hilfsfunktionen
    void loadCharacters();
    void createShaderProgram();
};

} // namespace VR_DAW 