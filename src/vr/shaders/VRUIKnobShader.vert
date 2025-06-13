#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out float KnobValue;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float value;
uniform float hover;
uniform float active;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    
    // Knob-Wert
    KnobValue = value;
    
    // Rotation basierend auf Knob-Wert
    float angle = value * 2.0 * 3.14159;
    mat4 rotation = mat4(
        cos(angle), -sin(angle), 0.0, 0.0,
        sin(angle), cos(angle), 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
    
    gl_Position = projection * view * model * rotation * vec4(aPos, 1.0);
} 