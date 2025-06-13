#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out float PanelState;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float hover;
uniform float active;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    
    // Panel-Zustand
    PanelState = active > 0.5 ? 2.0 : (hover > 0.5 ? 1.0 : 0.0);
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
} 