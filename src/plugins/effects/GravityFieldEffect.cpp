#include "GravityFieldEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

GravityFieldEffect::GravityFieldEffect()
    : strength(0.5f)
    , radius(0.3f)
    , decay(0.5f)
    , chaos(0.5f)
    , dimension(0.5f)
    , mix(0.5f)
    , quality(1.0f)
    , automatedStrength(false)
    , automatedRadius(false)
    , automatedDecay(false)
    , automatedChaos(false)
    , automatedDimension(false)
    , automatedMix(false)
    , automatedQuality(false)
    , bufferSize(44100 * 2) // 2 Sekunden bei 44.1kHz
{
    std::random_device rd;
    rng.seed(rd());
    initializeFields();
}

GravityFieldEffect::~GravityFieldEffect() {
    shutdown();
}

bool GravityFieldEffect::initialize() {
    initializeFields();
    return true;
}

void GravityFieldEffect::shutdown() {
    fields.clear();
}

void GravityFieldEffect::initializeFields() {
    fields.clear();
    
    // Erstelle 4 Felder
    for (int i = 0; i < 4; ++i) {
        Field field;
        
        // Erstelle 16 Partikel pro Feld
        for (int j = 0; j < 16; ++j) {
            Particle particle;
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            
            particle.position = dist(rng);
            particle.velocity = dist(rng) * 2.0f - 1.0f;
            particle.mass = dist(rng);
            particle.charge = dist(rng) * 2.0f - 1.0f;
            
            field.particles.push_back(particle);
        }
        
        field.dimension = dimension;
        fields.push_back(field);
    }
}

void GravityFieldEffect::updateFields() {
    // Aktualisiere Felder
    for (auto& field : fields) {
        field.dimension = dimension;
        
        // Aktualisiere Partikel
        for (auto& particle : field.particles) {
            // Aktualisiere Position basierend auf Geschwindigkeit
            particle.position += particle.velocity * 0.001f;
            particle.position = std::fmod(particle.position + 1.0f, 1.0f);
            
            // Aktualisiere Geschwindigkeit basierend auf Masse und Ladung
            particle.velocity += particle.mass * particle.charge * strength * 0.01f;
            particle.velocity = std::max(-1.0f, std::min(1.0f, particle.velocity));
            
            // Aktualisiere Masse basierend auf Zerfall
            particle.mass *= (1.0f - decay * 0.001f);
            particle.mass = std::max(0.0f, std::min(1.0f, particle.mass));
        }
    }
}

void GravityFieldEffect::processField(Field& field, float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float t = static_cast<float>(i) / framesPerBuffer;
        float output = buffer[i];
        
        // Verarbeite jedes Partikel
        for (auto& particle : field.particles) {
            // Berechne Partikel-Wert
            float value = output * particle.mass;
            value *= std::sin(2.0f * M_PI * particle.position * t);
            
            // Modifiziere Output
            output += value * field.dimension;
        }
        
        // Normalisiere Output
        output /= (field.particles.size() + 1);
        
        buffer[i] = output;
    }
}

void GravityFieldEffect::applyStrength(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] *= (1.0f + strength);
    }
}

void GravityFieldEffect::applyRadius(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float t = static_cast<float>(i) / framesPerBuffer;
        buffer[i] *= (1.0f + radius * std::sin(2.0f * M_PI * t));
    }
}

void GravityFieldEffect::applyDecay(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] *= (1.0f - decay * static_cast<float>(i) / framesPerBuffer);
    }
}

void GravityFieldEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateFields();
    
    // Verarbeite jedes Feld
    for (auto& field : fields) {
        processField(field, buffer, framesPerBuffer);
    }
    
    // Wende Effekte an
    applyStrength(buffer, framesPerBuffer);
    applyRadius(buffer, framesPerBuffer);
    applyDecay(buffer, framesPerBuffer);
    
    // Wende Mix an
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = buffer[i] * mix;
    }
}

std::vector<PluginParameter> GravityFieldEffect::getParameters() const {
    return {
        {"strength", strength, 0.0f, 1.0f, ""},
        {"radius", radius, 0.0f, 1.0f, ""},
        {"decay", decay, 0.0f, 1.0f, ""},
        {"chaos", chaos, 0.0f, 1.0f, ""},
        {"dimension", dimension, 0.0f, 1.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void GravityFieldEffect::setParameter(const std::string& name, float value) {
    if (name == "strength") strength = value;
    else if (name == "radius") radius = value;
    else if (name == "decay") decay = value;
    else if (name == "chaos") chaos = value;
    else if (name == "dimension") dimension = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
}

float GravityFieldEffect::getParameter(const std::string& name) const {
    if (name == "strength") return strength;
    if (name == "radius") return radius;
    if (name == "decay") return decay;
    if (name == "chaos") return chaos;
    if (name == "dimension") return dimension;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
    return 0.0f;
}

void GravityFieldEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "strength") automatedStrength = automated;
    else if (name == "radius") automatedRadius = automated;
    else if (name == "decay") automatedDecay = automated;
    else if (name == "chaos") automatedChaos = automated;
    else if (name == "dimension") automatedDimension = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool GravityFieldEffect::isParameterAutomated(const std::string& name) const {
    if (name == "strength") return automatedStrength;
    if (name == "radius") return automatedRadius;
    if (name == "decay") return automatedDecay;
    if (name == "chaos") return automatedChaos;
    if (name == "dimension") return automatedDimension;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    return false;
}

void GravityFieldEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        strength = 0.5f;
        radius = 0.3f;
        decay = 0.5f;
        chaos = 0.5f;
        dimension = 0.5f;
        mix = 0.5f;
        quality = 1.0f;
    }
    else if (presetName == "Intense") {
        strength = 0.8f;
        radius = 0.6f;
        decay = 0.7f;
        chaos = 0.8f;
        dimension = 0.7f;
        mix = 0.7f;
        quality = 1.0f;
    }
    else if (presetName == "Subtle") {
        strength = 0.3f;
        radius = 0.2f;
        decay = 0.3f;
        chaos = 0.3f;
        dimension = 0.3f;
        mix = 0.3f;
        quality = 1.0f;
    }
    
    updateFields();
}

void GravityFieldEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> GravityFieldEffect::getAvailablePresets() const {
    return {"Standard", "Intense", "Subtle"};
}

} // namespace VR_DAW 