#include "VRUITextRenderer.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

namespace VR_DAW {

VRUITextRenderer::VRUITextRenderer()
    : vao(0)
    , vbo(0)
    , shaderProgram(0)
    , ft(nullptr)
    , face(nullptr)
    , projection(1.0f)
    , view(1.0f)
{
}

VRUITextRenderer::~VRUITextRenderer() {
    shutdown();
}

bool VRUITextRenderer::initialize() {
    // Initialisiere Freetype
    if (FT_Init_FreeType(&ft)) {
        spdlog::error("Failed to initialize FreeType");
        return false;
    }

    // Erstelle VAO und VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Erstelle Shader-Programm
    createShaderProgram();

    return true;
}

void VRUITextRenderer::shutdown() {
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (shaderProgram) glDeleteProgram(shaderProgram);
    if (face) FT_Done_Face(face);
    if (ft) FT_Done_FreeType(ft);
}

void VRUITextRenderer::setFont(const std::string& fontPath, unsigned int fontSize) {
    if (face) {
        FT_Done_Face(face);
        face = nullptr;
    }

    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        spdlog::error("Failed to load font: {}", fontPath);
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    loadCharacters();
}

void VRUITextRenderer::setProjection(const glm::mat4& projection) {
    this->projection = projection;
}

void VRUITextRenderer::setView(const glm::mat4& view) {
    this->view = view;
}

void VRUITextRenderer::renderText(const std::string& text, float x, float y, float scale, const glm::vec4& color) {
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniform4fv(glGetUniformLocation(shaderProgram, "textColor"), 1, glm::value_ptr(color));
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    float xpos = x;
    float ypos = y;

    for (char c : text) {
        Character ch = characters[c];

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        xpos += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void VRUITextRenderer::loadCharacters() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            spdlog::warn("Failed to load Glyph: {}", c);
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };

        characters.insert(std::pair<char, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void VRUITextRenderer::createShaderProgram() {
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec4 vertex;
        out vec2 TexCoord;
        uniform mat4 projection;
        uniform mat4 view;
        void main() {
            gl_Position = projection * view * vec4(vertex.xy, 0.0, 1.0);
            TexCoord = vertex.zw;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoord;
        out vec4 FragColor;
        uniform sampler2D text;
        uniform vec4 textColor;
        void main() {
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoord).r);
            FragColor = textColor * sampled;
        }
    )";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

} // namespace VR_DAW 