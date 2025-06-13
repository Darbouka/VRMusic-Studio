#pragma once

#include <string>
#include <map>
#include <vector>

struct InstrumentModel {
    std::string brand;
    std::string model;
    std::string year;
    std::string description;
    std::vector<float> defaultParameters;
};

class InstrumentModels {
public:
    // Synthesizer Modelle
    static const std::map<std::string, InstrumentModel> SYNTHESIZER_MODELS;
    
    // Klavier Modelle
    static const std::map<std::string, InstrumentModel> PIANO_MODELS;
    
    // Streichinstrumente Modelle
    static const std::map<std::string, InstrumentModel> STRING_MODELS;
    
    // Blasinstrumente Modelle
    static const std::map<std::string, InstrumentModel> WIND_MODELS;
    
    // Holzblasinstrumente Modelle
    static const std::map<std::string, InstrumentModel> WOODWIND_MODELS;
    
    // Schlaginstrumente Modelle
    static const std::map<std::string, InstrumentModel> PERCUSSION_MODELS;
    
    // Weltweite Instrumente Modelle
    static const std::map<std::string, InstrumentModel> WORLD_MODELS;
    
    // Elektrische Instrumente Modelle
    static const std::map<std::string, InstrumentModel> ELECTRIC_MODELS;
    
    // Revolutionäre Instrumente Modelle
    static const std::map<std::string, InstrumentModel> REVOLUTIONARY_MODELS;
    
    // Neue Spezialinstrumente Modelle
    static const std::map<std::string, InstrumentModel> SPECIAL_MODELS;
}; 