#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out float SequencerState;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float hover;
uniform float active;
uniform float currentStep;
uniform float totalSteps;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    
    // Sequencer-Zustand
    SequencerState = active > 0.5 ? 2.0 : (hover > 0.5 ? 1.0 : 0.0);
    
    // Position basierend auf aktuellem Schritt
    vec3 pos = aPos;
    if (aPos.x > 0.0) {
        pos.x = (currentStep / totalSteps) * 2.0 - 1.0;
    }
    
    gl_Position = projection * view * model * vec4(pos, 1.0);
} 