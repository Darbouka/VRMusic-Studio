#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in float SequencerState;

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
uniform float currentStep;
uniform float totalSteps;
uniform vec4 stepColor;
uniform vec4 activeStepColor;

void main() {
    vec4 finalColor = color;
    
    // Sequencer-Zustand
    if (SequencerState > 1.5) {
        finalColor = activeColor;
    } else if (SequencerState > 0.5) {
        finalColor = hoverColor;
    }
    
    if (useTexture) {
        finalColor = texture(texture1, TexCoord);
    }
    
    // Schritt-Markierung
    float step = TexCoord.x * totalSteps;
    if (fract(step) < 0.1) {
        if (floor(step) == currentStep) {
            finalColor = mix(finalColor, activeStepColor, 0.7);
        } else {
            finalColor = mix(finalColor, stepColor, 0.5);
        }
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