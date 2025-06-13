#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in float VisualizerState;

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
uniform float audioLevel;
uniform float spectrum[1024];
uniform float waveform[1024];
uniform vec4 spectrumColor;
uniform vec4 waveformColor;

void main() {
    vec4 finalColor = color;
    
    // Visualizer-Zustand
    if (VisualizerState > 1.5) {
        finalColor = activeColor;
    } else if (VisualizerState > 0.5) {
        finalColor = hoverColor;
    }
    
    if (useTexture) {
        finalColor = texture(texture1, TexCoord);
    }
    
    // Spektrum-Visualisierung
    float freq = TexCoord.x * 1024.0;
    float level = spectrum[int(freq)];
    if (TexCoord.y < level) {
        finalColor = mix(finalColor, spectrumColor, 0.7);
    }
    
    // Wellenform-Visualisierung
    float time = TexCoord.x * 1024.0;
    float wave = waveform[int(time)];
    if (abs(TexCoord.y - wave) < 0.05) {
        finalColor = mix(finalColor, waveformColor, 0.7);
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