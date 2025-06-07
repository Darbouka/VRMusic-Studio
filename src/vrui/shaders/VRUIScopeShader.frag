#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in float ScopeState;

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
uniform float phase[1024];
uniform vec4 spectrumColor;
uniform vec4 waveformColor;
uniform vec4 phaseColor;

// Revolutionäre VR-Visualisierungen
uniform float time;
uniform float frequency;
uniform float amplitude;
uniform float modulation;
uniform float spatialization;
uniform float holographicDepth;
uniform float volumetricDensity;
uniform float particleSize;
uniform float glowIntensity;
uniform float distortionAmount;

// Erweiterte Shader-Funktionen
vec4 applyHolographicEffect(vec4 color, float depth) {
    float hologram = sin(time * 2.0 + depth * 10.0) * 0.5 + 0.5;
    return mix(color, vec4(0.0, 1.0, 1.0, 1.0), hologram * 0.3);
}

vec4 applyVolumetricEffect(vec4 color, float density) {
    float noise = fract(sin(dot(TexCoord, vec2(12.9898, 78.233))) * 43758.5453);
    return mix(color, vec4(1.0), noise * density);
}

vec4 applyParticleEffect(vec4 color, float size) {
    vec2 particle = fract(TexCoord * size + time);
    float particleMask = smoothstep(0.0, 0.1, length(particle - 0.5));
    return mix(color, vec4(1.0), particleMask);
}

vec4 applyGlowEffect(vec4 color, float intensity) {
    float glow = pow(1.0 - length(TexCoord - 0.5) * 2.0, 4.0) * intensity;
    return color + vec4(glow);
}

vec4 applyDistortionEffect(vec4 color, float amount) {
    vec2 distorted = TexCoord + sin(TexCoord * 10.0 + time) * amount;
    return texture(texture1, distorted);
}

void main() {
    vec4 finalColor = color;
    
    // Scope-Zustand
    if (ScopeState > 1.5) {
        finalColor = activeColor;
    } else if (ScopeState > 0.5) {
        finalColor = hoverColor;
    }
    
    if (useTexture) {
        finalColor = texture(texture1, TexCoord);
    }
    
    // Revolutionäre Spektrum-Visualisierung
    float freq = TexCoord.x * 1024.0;
    float level = spectrum[int(freq)];
    if (TexCoord.y < level) {
        vec4 spectrumEffect = mix(finalColor, spectrumColor, 0.7);
        spectrumEffect = applyHolographicEffect(spectrumEffect, holographicDepth);
        spectrumEffect = applyVolumetricEffect(spectrumEffect, volumetricDensity);
        spectrumEffect = applyParticleEffect(spectrumEffect, particleSize);
        spectrumEffect = applyGlowEffect(spectrumEffect, glowIntensity);
        finalColor = spectrumEffect;
    }
    
    // Revolutionäre Wellenform-Visualisierung
    float time = TexCoord.x * 1024.0;
    float wave = waveform[int(time)];
    if (abs(TexCoord.y - wave) < 0.05) {
        vec4 waveformEffect = mix(finalColor, waveformColor, 0.7);
        waveformEffect = applyHolographicEffect(waveformEffect, holographicDepth);
        waveformEffect = applyVolumetricEffect(waveformEffect, volumetricDensity);
        waveformEffect = applyParticleEffect(waveformEffect, particleSize);
        waveformEffect = applyGlowEffect(waveformEffect, glowIntensity);
        finalColor = waveformEffect;
    }
    
    // Revolutionäre Phasen-Visualisierung
    float phase = phase[int(time)];
    if (abs(TexCoord.y - phase) < 0.05) {
        vec4 phaseEffect = mix(finalColor, phaseColor, 0.7);
        phaseEffect = applyHolographicEffect(phaseEffect, holographicDepth);
        phaseEffect = applyVolumetricEffect(phaseEffect, volumetricDensity);
        phaseEffect = applyParticleEffect(phaseEffect, particleSize);
        phaseEffect = applyGlowEffect(phaseEffect, glowIntensity);
        finalColor = phaseEffect;
    }
    
    // Revolutionäre Beleuchtung
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
        
        // Combine mit revolutionären Effekten
        vec3 result = (ambient + diffuse + specular) * finalColor.rgb;
        finalColor = vec4(result, finalColor.a);
        finalColor = applyHolographicEffect(finalColor, holographicDepth);
        finalColor = applyVolumetricEffect(finalColor, volumetricDensity);
        finalColor = applyParticleEffect(finalColor, particleSize);
        finalColor = applyGlowEffect(finalColor, glowIntensity);
    }
    
    // Finale Verzerrung
    finalColor = applyDistortionEffect(finalColor, distortionAmount);
    
    FragColor = finalColor;
} 