#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in float MeterState;

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
uniform float level;
uniform float peak;
uniform float rms;
uniform vec4 levelColor;
uniform vec4 peakColor;
uniform vec4 rmsColor;
uniform vec4 clipColor;

void main() {
    vec4 finalColor = color;
    
    // Meter-Zustand
    if (MeterState > 1.5) {
        finalColor = activeColor;
    } else if (MeterState > 0.5) {
        finalColor = hoverColor;
    }
    
    if (useTexture) {
        finalColor = texture(texture1, TexCoord);
    }
    
    // Level-Anzeige
    if (TexCoord.y < level) {
        finalColor = mix(finalColor, levelColor, 0.7);
    }
    
    // Peak-Anzeige
    if (abs(TexCoord.y - peak) < 0.05) {
        finalColor = mix(finalColor, peakColor, 0.7);
    }
    
    // RMS-Anzeige
    if (abs(TexCoord.y - rms) < 0.05) {
        finalColor = mix(finalColor, rmsColor, 0.7);
    }
    
    // Clip-Anzeige
    if (level > 0.95) {
        finalColor = mix(finalColor, clipColor, 0.7);
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