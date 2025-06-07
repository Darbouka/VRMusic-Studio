#include "BioFilterEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

BioFilterEffect::BioFilterEffect()
    : frequency(1000.0f)
    , resonance(0.7f)
    , mutation(0.5f)
    , evolution(0.5f)
    , adaptation(0.5f)
    , mix(0.5f)
    , quality(1.0f)
    , automatedFrequency(false)
    , automatedResonance(false)
    , automatedMutation(false)
    , automatedEvolution(false)
    , automatedAdaptation(false)
    , automatedMix(false)
    , automatedQuality(false)
    , bufferSize(44100 * 2) // 2 Sekunden bei 44.1kHz
{
    std::random_device rd;
    rng.seed(rd());
    initializeOrganisms();
}

BioFilterEffect::~BioFilterEffect() {
    shutdown();
}

bool BioFilterEffect::initialize() {
    initializeOrganisms();
    return true;
}

void BioFilterEffect::shutdown() {
    organisms.clear();
}

void BioFilterEffect::initializeOrganisms() {
    organisms.clear();
    
    // Erstelle 4 Organismen
    for (int i = 0; i < 4; ++i) {
        Organism organism;
        
        // Erstelle 16 Zellen pro Organismus
        for (int j = 0; j < 16; ++j) {
            Cell cell;
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            
            cell.frequency = frequency * (1.0f + dist(rng) * 0.5f);
            cell.resonance = resonance * (1.0f + dist(rng) * 0.2f);
            cell.energy = dist(rng);
            cell.age = 0.0f;
            
            organism.cells.push_back(cell);
        }
        
        organism.fitness = calculateFitness(organism);
        organisms.push_back(organism);
    }
}

void BioFilterEffect::updateOrganisms() {
    // Aktualisiere Organismen
    for (auto& organism : organisms) {
        // Aktualisiere Zellen
        for (auto& cell : organism.cells) {
            // Aktualisiere Alter
            cell.age += 0.001f;
            
            // Aktualisiere Energie basierend auf Alter und Anpassung
            cell.energy *= (1.0f - cell.age * adaptation);
            cell.energy = std::max(0.0f, std::min(1.0f, cell.energy));
            
            // Aktualisiere Frequenz und Resonanz basierend auf Energie
            cell.frequency *= (1.0f + cell.energy * 0.1f);
            cell.resonance *= (1.0f + cell.energy * 0.05f);
        }
        
        // Aktualisiere Fitness
        organism.fitness = calculateFitness(organism);
    }
}

void BioFilterEffect::processOrganism(Organism& organism, float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float t = static_cast<float>(i) / framesPerBuffer;
        float output = buffer[i];
        
        // Verarbeite jede Zelle
        for (auto& cell : organism.cells) {
            // Berechne Filter-Wert
            float value = output * std::sin(2.0f * M_PI * cell.frequency * t);
            value *= cell.resonance * cell.energy;
            
            // Modifiziere Output
            output += value;
        }
        
        // Normalisiere Output
        output /= (organism.cells.size() + 1);
        
        buffer[i] = output;
    }
}

void BioFilterEffect::evolveOrganisms() {
    // Sortiere Organismen nach Fitness
    std::sort(organisms.begin(), organisms.end(),
              [](const Organism& a, const Organism& b) {
                  return a.fitness > b.fitness;
              });
    
    // Erstelle neue Organismen aus den besten
    std::vector<Organism> newOrganisms;
    for (size_t i = 0; i < organisms.size() / 2; ++i) {
        // Kopiere besten Organismus
        newOrganisms.push_back(organisms[i]);
        
        // Erstelle Kind aus zwei Eltern
        Organism child;
        for (size_t j = 0; j < organisms[i].cells.size(); ++j) {
            Cell cell;
            cell.frequency = (organisms[i].cells[j].frequency + organisms[i+1].cells[j].frequency) * 0.5f;
            cell.resonance = (organisms[i].cells[j].resonance + organisms[i+1].cells[j].resonance) * 0.5f;
            cell.energy = 1.0f;
            cell.age = 0.0f;
            child.cells.push_back(cell);
        }
        child.fitness = calculateFitness(child);
        newOrganisms.push_back(child);
    }
    
    organisms = newOrganisms;
}

void BioFilterEffect::mutateOrganisms() {
    for (auto& organism : organisms) {
        for (auto& cell : organism.cells) {
            if (std::uniform_real_distribution<float>(0.0f, 1.0f)(rng) < mutation) {
                // Mutiere Frequenz
                cell.frequency *= (1.0f + std::uniform_real_distribution<float>(-0.2f, 0.2f)(rng));
                
                // Mutiere Resonanz
                cell.resonance *= (1.0f + std::uniform_real_distribution<float>(-0.1f, 0.1f)(rng));
                cell.resonance = std::max(0.0f, std::min(1.0f, cell.resonance));
            }
        }
    }
}

void BioFilterEffect::adaptOrganisms() {
    for (auto& organism : organisms) {
        for (auto& cell : organism.cells) {
            // Passe Energie basierend auf Anpassung an
            cell.energy *= (1.0f + adaptation * std::uniform_real_distribution<float>(-0.1f, 0.1f)(rng));
            cell.energy = std::max(0.0f, std::min(1.0f, cell.energy));
        }
    }
}

float BioFilterEffect::calculateFitness(const Organism& organism) const {
    float fitness = 0.0f;
    
    // Berechne Fitness basierend auf Zellen
    for (const auto& cell : organism.cells) {
        fitness += cell.energy * cell.resonance;
    }
    
    return fitness / organism.cells.size();
}

void BioFilterEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateOrganisms();
    
    // Verarbeite jeden Organismus
    for (auto& organism : organisms) {
        processOrganism(organism, buffer, framesPerBuffer);
    }
    
    // Wende Evolution an
    if (std::uniform_real_distribution<float>(0.0f, 1.0f)(rng) < evolution) {
        evolveOrganisms();
    }
    
    // Wende Mutation an
    mutateOrganisms();
    
    // Wende Anpassung an
    adaptOrganisms();
    
    // Wende Mix an
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = buffer[i] * mix;
    }
}

std::vector<PluginParameter> BioFilterEffect::getParameters() const {
    return {
        {"frequency", frequency, 20.0f, 20000.0f, "Hz"},
        {"resonance", resonance, 0.0f, 1.0f, ""},
        {"mutation", mutation, 0.0f, 1.0f, ""},
        {"evolution", evolution, 0.0f, 1.0f, ""},
        {"adaptation", adaptation, 0.0f, 1.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void BioFilterEffect::setParameter(const std::string& name, float value) {
    if (name == "frequency") frequency = value;
    else if (name == "resonance") resonance = value;
    else if (name == "mutation") mutation = value;
    else if (name == "evolution") evolution = value;
    else if (name == "adaptation") adaptation = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
}

float BioFilterEffect::getParameter(const std::string& name) const {
    if (name == "frequency") return frequency;
    if (name == "resonance") return resonance;
    if (name == "mutation") return mutation;
    if (name == "evolution") return evolution;
    if (name == "adaptation") return adaptation;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
    return 0.0f;
}

void BioFilterEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "frequency") automatedFrequency = automated;
    else if (name == "resonance") automatedResonance = automated;
    else if (name == "mutation") automatedMutation = automated;
    else if (name == "evolution") automatedEvolution = automated;
    else if (name == "adaptation") automatedAdaptation = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool BioFilterEffect::isParameterAutomated(const std::string& name) const {
    if (name == "frequency") return automatedFrequency;
    if (name == "resonance") return automatedResonance;
    if (name == "mutation") return automatedMutation;
    if (name == "evolution") return automatedEvolution;
    if (name == "adaptation") return automatedAdaptation;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    return false;
}

void BioFilterEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        frequency = 1000.0f;
        resonance = 0.7f;
        mutation = 0.5f;
        evolution = 0.5f;
        adaptation = 0.5f;
        mix = 0.5f;
        quality = 1.0f;
    }
    else if (presetName == "Evolving") {
        frequency = 2000.0f;
        resonance = 0.8f;
        mutation = 0.7f;
        evolution = 0.8f;
        adaptation = 0.6f;
        mix = 0.7f;
        quality = 1.0f;
    }
    else if (presetName == "Stable") {
        frequency = 500.0f;
        resonance = 0.5f;
        mutation = 0.3f;
        evolution = 0.3f;
        adaptation = 0.4f;
        mix = 0.4f;
        quality = 1.0f;
    }
    
    updateOrganisms();
}

void BioFilterEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> BioFilterEffect::getAvailablePresets() const {
    return {"Standard", "Evolving", "Stable"};
}

} // namespace VR_DAW 