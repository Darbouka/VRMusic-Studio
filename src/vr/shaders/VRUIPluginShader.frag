#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in float PluginState;

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
uniform float bypass;
uniform float processing;
uniform vec4 bypassColor;
uniform vec4 processingColor;

void main() {
    vec4 finalColor = color;
    
    // Plugin-Zustand
    if (PluginState > 1.5) {
        finalColor = activeColor;
    } else if (PluginState > 0.5) {
        finalColor = hoverColor;
    }
    
    if (useTexture) {
        finalColor = texture(texture1, TexCoord);
    }
    
    // Bypass/Processing-Status
    if (bypass > 0.5) {
        finalColor = mix(finalColor, bypassColor, 0.5);
    }
    if (processing > 0.5) {
        finalColor = mix(finalColor, processingColor, 0.5);
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