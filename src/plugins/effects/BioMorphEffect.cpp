#include "BioMorphEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VR_DAW {

BioMorphEffect::BioMorphEffect()
    : growth(0.5f)
    , mutation(0.3f)
    , evolution(0.4f)
    , adaptation(0.5f)
    , complexity(0.6f)
    , mix(0.5f)
    , quality(1.0f)
    , automatedGrowth(false)
    , automatedMutation(false)
    , automatedEvolution(false)
    , automatedAdaptation(false)
    , automatedComplexity(false)
    , automatedMix(false)
    , automatedQuality(false)
    , bufferSize(44100 * 2) // 2 Sekunden bei 44.1kHz
{
    std::random_device rd;
    rng.seed(rd());
    initializeOrganisms();
}

BioMorphEffect::~BioMorphEffect() {
    shutdown();
}

bool BioMorphEffect::initialize() {
    initializeOrganisms();
    return true;
}

void BioMorphEffect::shutdown() {
    organisms.clear();
}

void BioMorphEffect::initializeOrganisms() {
    organisms.clear();
    
    // Erstelle 4 Organismen für komplexe Klangtransformation
    for (int i = 0; i < 4; ++i) {
        Organism organism;
        
        // Erstelle 16 Zellen pro Organismus
        for (int j = 0; j < 16; ++j) {
            Cell cell;
            std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
            
            // Initialisiere DNA
            for (auto& gene : cell.dna) {
                gene = dist(rng);
            }
            
            cell.energy = 1.0f;
            cell.age = 0.0f;
            cell.active = true;
            
            organism.cells.push_back(cell);
        }
        
        organism.fitness = 0.0f;
        organism.adaptation = 0.0f;
        organism.alive = true;
        
        organisms.push_back(organism);
    }
}

void BioMorphEffect::updateOrganisms() {
    // Aktualisiere Organismen basierend auf Parametern
    if (growth != currentGrowth) {
        for (auto& organism : organisms) {
            for (auto& cell : organism.cells) {
                cell.energy *= (1.0f + growth);
            }
        }
        currentGrowth = growth;
    }
    
    if (mutation != currentMutation) {
        mutateOrganisms();
        currentMutation = mutation;
    }
    
    if (evolution != currentEvolution) {
        evolveOrganisms();
        currentEvolution = evolution;
    }
    
    if (adaptation != currentAdaptation) {
        adaptOrganisms();
        currentAdaptation = adaptation;
    }
}

void BioMorphEffect::evolveOrganisms() {
    // Evolviere Organismen basierend auf Fitness
    for (auto& organism : organisms) {
        organism.fitness = calculateFitness(organism);
        
        if (organism.fitness > 0.5f) {
            // Vermehre erfolgreiche Organismen
            Organism child = organism;
            for (auto& cell : child.cells) {
                cell.energy *= 0.5f;
                cell.age = 0.0f;
            }
            organisms.push_back(child);
        }
    }
}

void BioMorphEffect::mutateOrganisms() {
    // Mutiere Organismen basierend auf Mutation-Parameter
    for (auto& organism : organisms) {
        for (auto& cell : organism.cells) {
            if (std::rand() / RAND_MAX < mutation) {
                // Mutiere DNA
                for (auto& gene : cell.dna) {
                    gene += mutation * (std::rand() / RAND_MAX - 0.5f);
                }
            }
        }
    }
}

void BioMorphEffect::adaptOrganisms() {
    // Passe Organismen an basierend auf Adaptation-Parameter
    for (auto& organism : organisms) {
        organism.adaptation = adaptation;
        
        for (auto& cell : organism.cells) {
            cell.energy *= (1.0f + adaptation * (std::rand() / RAND_MAX - 0.5f));
        }
    }
}

float BioMorphEffect::calculateFitness(const Organism& organism) {
    // Berechne Fitness basierend auf Komplexität und Adaptation
    float fitness = 0.0f;
    
    for (const auto& cell : organism.cells) {
        if (cell.active) {
            fitness += cell.energy * complexity;
        }
    }
    
    return fitness / organism.cells.size();
}

void BioMorphEffect::processOrganism(Organism& organism, float* buffer, unsigned long framesPerBuffer) {
    if (!organism.alive) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float output = 0.0f;
        
        // Verarbeite jede Zelle
        for (auto& cell : organism.cells) {
            if (cell.active) {
                // Berechne Zellen-Output basierend auf DNA
                float cellOutput = 0.0f;
                for (const auto& gene : cell.dna) {
                    cellOutput += gene * buffer[i];
                }
                
                // Wende Energie an
                cellOutput *= cell.energy;
                
                // Aktualisiere Zellen-Status
                cell.age += 1.0f / 44100.0f;
                cell.energy *= (1.0f - 0.0001f); // Langsamer Energieverlust
                
                output += cellOutput;
            }
        }
        
        // Normalisiere Output
        output /= organism.cells.size();
        
        // Schreibe in Buffer
        buffer[i] = output;
    }
}

void BioMorphEffect::applyGrowth(float* buffer, unsigned long framesPerBuffer) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] *= (1.0f + growth * std::sin(2.0f * M_PI * i / framesPerBuffer));
    }
}

void BioMorphEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    updateOrganisms();
    
    // Verarbeite jeden Organismus
    for (auto& organism : organisms) {
        processOrganism(organism, buffer, framesPerBuffer);
    }
    
    // Wende Wachstum an
    if (growth > 0.0f) {
        applyGrowth(buffer, framesPerBuffer);
    }
    
    // Wende Mix an
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = buffer[i] * mix;
    }
}

std::vector<PluginParameter> BioMorphEffect::getParameters() const {
    return {
        {"growth", growth, 0.0f, 1.0f, ""},
        {"mutation", mutation, 0.0f, 1.0f, ""},
        {"evolution", evolution, 0.0f, 1.0f, ""},
        {"adaptation", adaptation, 0.0f, 1.0f, ""},
        {"complexity", complexity, 0.0f, 1.0f, ""},
        {"mix", mix, 0.0f, 1.0f, ""},
        {"quality", quality, 0.0f, 1.0f, ""}
    };
}

void BioMorphEffect::setParameter(const std::string& name, float value) {
    if (name == "growth") growth = value;
    else if (name == "mutation") mutation = value;
    else if (name == "evolution") evolution = value;
    else if (name == "adaptation") adaptation = value;
    else if (name == "complexity") complexity = value;
    else if (name == "mix") mix = value;
    else if (name == "quality") quality = value;
}

float BioMorphEffect::getParameter(const std::string& name) const {
    if (name == "growth") return growth;
    if (name == "mutation") return mutation;
    if (name == "evolution") return evolution;
    if (name == "adaptation") return adaptation;
    if (name == "complexity") return complexity;
    if (name == "mix") return mix;
    if (name == "quality") return quality;
    return 0.0f;
}

void BioMorphEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "growth") automatedGrowth = automated;
    else if (name == "mutation") automatedMutation = automated;
    else if (name == "evolution") automatedEvolution = automated;
    else if (name == "adaptation") automatedAdaptation = automated;
    else if (name == "complexity") automatedComplexity = automated;
    else if (name == "mix") automatedMix = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool BioMorphEffect::isParameterAutomated(const std::string& name) const {
    if (name == "growth") return automatedGrowth;
    if (name == "mutation") return automatedMutation;
    if (name == "evolution") return automatedEvolution;
    if (name == "adaptation") return automatedAdaptation;
    if (name == "complexity") return automatedComplexity;
    if (name == "mix") return automatedMix;
    if (name == "quality") return automatedQuality;
    return false;
}

void BioMorphEffect::loadPreset(const std::string& presetName) {
    if (presetName == "Standard") {
        growth = 0.5f;
        mutation = 0.3f;
        evolution = 0.4f;
        adaptation = 0.5f;
        complexity = 0.6f;
        mix = 0.5f;
        quality = 1.0f;
    }
    else if (presetName == "Heavy") {
        growth = 0.7f;
        mutation = 0.5f;
        evolution = 0.6f;
        adaptation = 0.7f;
        complexity = 0.8f;
        mix = 0.7f;
        quality = 1.0f;
    }
    else if (presetName == "Subtle") {
        growth = 0.3f;
        mutation = 0.2f;
        evolution = 0.3f;
        adaptation = 0.4f;
        complexity = 0.5f;
        mix = 0.3f;
        quality = 1.0f;
    }
    
    updateOrganisms();
}

void BioMorphEffect::savePreset(const std::string& presetName) {
    // TODO: Implementiere Preset-Speicherung
}

std::vector<std::string> BioMorphEffect::getAvailablePresets() const {
    return {"Standard", "Heavy", "Subtle"};
}

} // namespace VR_DAW 