#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in float PianoRollState;

out vec4 FragColor;

uniform vec4 color;
uniform vec4 hoverColor;
uniform vec4 activeColor;
uniform sampler2D texture1;
uniform bool useTexture;
uniform bool useLighting;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform float specularStrength;
uniform float currentTime;
uniform float totalTime;
uniform float currentNote;
uniform float totalNotes;
uniform vec4 noteColor;
uniform vec4 activeNoteColor;
uniform vec4 gridColor;

void main() {
    vec4 finalColor = color;
    
    // Piano Roll-Zustand
    if (PianoRollState > 1.5) {
        finalColor = activeColor;
    } else if (PianoRollState > 0.5) {
        finalColor = hoverColor;
    }
    
    if (useTexture) {
        finalColor = texture(texture1, TexCoord);
    }
    
    // Noten-Markierung
    float time = TexCoord.x * totalTime;
    float note = TexCoord.y * totalNotes;
    if (fract(time) < 0.1 && fract(note) < 0.1) {
        if (floor(time) == currentTime && floor(note) == currentNote) {
            finalColor = mix(finalColor, activeNoteColor, 0.7);
        } else {
            finalColor = mix(finalColor, noteColor, 0.5);
        }
    }
    
    // Grid-Linien
    if (fract(time) < 0.1 || fract(note) < 0.1) {
        finalColor = mix(finalColor, gridColor, 0.3);
    }
    
    if (useLighting) {
        // Ambient
        vec3 ambient = ambientStrength * lightColor;
        
        // Diffuse
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        
        // Specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * lightColor;
        
        // Combine
        vec3 result = (ambient + diffuse + specular) * finalColor.rgb;
        finalColor = vec4(result, finalColor.a);
    }
    
    FragColor = finalColor;
} 