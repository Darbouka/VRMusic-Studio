#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out float ButtonState;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float isHovered;
uniform float isPressed;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    ButtonState = isPressed > 0.5 ? 2.0 : (isHovered > 0.5 ? 1.0 : 0.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
} 