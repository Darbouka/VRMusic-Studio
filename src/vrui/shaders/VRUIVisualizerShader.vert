#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out float VisualizerState;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float hover;
uniform float active;
uniform float audioLevel;
uniform float spectrum[1024];
uniform float waveform[1024];

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    
    // Visualizer-Zustand
    VisualizerState = active > 0.5 ? 2.0 : (hover > 0.5 ? 1.0 : 0.0);
    
    // Position basierend auf Audio-Level
    vec3 pos = aPos;
    if (aPos.y > 0.0) {
        pos.y = audioLevel * 2.0 - 1.0;
    }
    
    gl_Position = projection * view * model * vec4(pos, 1.0);
} 