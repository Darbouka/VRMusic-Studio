#include "InstrumentModels.hpp"

// Synthesizer Modelle
const std::map<std::string, InstrumentModel> InstrumentModels::SYNTHESIZER_MODELS = {
    {"Moog Minimoog", {"Moog", "Minimoog", "1970", "Legendärer monophoner Synthesizer", {0.8f, 0.5f, 0.3f, 0.7f, 0.2f, 0.4f}}},
    {"Roland Jupiter-8", {"Roland", "Jupiter-8", "1981", "Polyphoner Analog-Synthesizer", {0.9f, 0.6f, 0.4f, 0.8f, 0.3f, 0.5f}}},
    {"Sequential Prophet-5", {"Sequential", "Prophet-5", "1978", "Polyphoner Synthesizer", {0.85f, 0.55f, 0.35f, 0.75f, 0.25f, 0.45f}}},
    {"Yamaha DX7", {"Yamaha", "DX7", "1983", "FM-Synthesizer", {0.7f, 0.4f, 0.2f, 0.6f, 0.1f, 0.3f}}},
    {"Korg MS-20", {"Korg", "MS-20", "1978", "Monophoner Synthesizer", {0.75f, 0.45f, 0.25f, 0.65f, 0.15f, 0.35f}}}
};

// Klavier Modelle
const std::map<std::string, InstrumentModel> InstrumentModels::PIANO_MODELS = {
    {"Steinway Model D", {"Steinway & Sons", "Model D", "1884", "Konzertflügel", {0.9f, 0.6f, 0.4f, 0.8f, 0.3f, 0.5f}}},
    {"Yamaha CFX", {"Yamaha", "CFX", "2010", "Konzertflügel", {0.85f, 0.55f, 0.35f, 0.75f, 0.25f, 0.45f}}},
    {"Bösendorfer Imperial", {"Bösendorfer", "Imperial", "1909", "Konzertflügel", {0.95f, 0.65f, 0.45f, 0.85f, 0.35f, 0.55f}}},
    {"Fazioli F308", {"Fazioli", "F308", "1981", "Konzertflügel", {0.88f, 0.58f, 0.38f, 0.78f, 0.28f, 0.48f}}},
    {"Kawai EX", {"Kawai", "EX", "1996", "Konzertflügel", {0.82f, 0.52f, 0.32f, 0.72f, 0.22f, 0.42f}}}
};

// Streichinstrumente Modelle
const std::map<std::string, InstrumentModel> InstrumentModels::STRING_MODELS = {
    {"Stradivarius Violin", {"Antonio Stradivari", "Violin", "1700", "Geige", {0.9f, 0.6f, 0.4f, 0.8f, 0.3f, 0.5f}}},
    {"Guarneri Cello", {"Guarneri", "Cello", "1730", "Cello", {0.85f, 0.55f, 0.35f, 0.75f, 0.25f, 0.45f}}},
    {"Amati Viola", {"Amati", "Viola", "1650", "Bratsche", {0.88f, 0.58f, 0.38f, 0.78f, 0.28f, 0.48f}}},
    {"Montagnana Double Bass", {"Montagnana", "Double Bass", "1740", "Kontrabass", {0.92f, 0.62f, 0.42f, 0.82f, 0.32f, 0.52f}}},
    {"Gofriller Violin", {"Gofriller", "Violin", "1720", "Geige", {0.87f, 0.57f, 0.37f, 0.77f, 0.27f, 0.47f}}}
};

// Blasinstrumente Modelle
const std::map<std::string, InstrumentModel> InstrumentModels::WIND_MODELS = {
    {"Selmer Mark VI", {"Selmer", "Mark VI", "1954", "Saxophon", {0.85f, 0.55f, 0.35f, 0.75f, 0.25f, 0.45f}}},
    {"Yamaha YTR-8335", {"Yamaha", "YTR-8335", "2000", "Trompete", {0.8f, 0.5f, 0.3f, 0.7f, 0.2f, 0.4f}}},
    {"Bach Stradivarius", {"Bach", "Stradivarius", "1920", "Trompete", {0.82f, 0.52f, 0.32f, 0.72f, 0.22f, 0.42f}}},
    {"Conn 8D", {"Conn", "8D", "1930", "Horn", {0.88f, 0.58f, 0.38f, 0.78f, 0.28f, 0.48f}}},
    {"Yamaha YSL-882O", {"Yamaha", "YSL-882O", "1990", "Posaune", {0.83f, 0.53f, 0.33f, 0.73f, 0.23f, 0.43f}}}
};

// Holzblasinstrumente Modelle
const std::map<std::string, InstrumentModel> InstrumentModels::WOODWIND_MODELS = {
    {"Buffet R13", {"Buffet", "R13", "1950", "Klarinette", {0.85f, 0.55f, 0.35f, 0.75f, 0.25f, 0.45f}}},
    {"Yamaha YFL-677H", {"Yamaha", "YFL-677H", "2000", "Querflöte", {0.8f, 0.5f, 0.3f, 0.7f, 0.2f, 0.4f}}},
    {"Loree Royal", {"Loree", "Royal", "1980", "Oboe", {0.82f, 0.52f, 0.32f, 0.72f, 0.22f, 0.42f}}},
    {"Fox 601", {"Fox", "601", "1970", "Fagott", {0.88f, 0.58f, 0.38f, 0.78f, 0.28f, 0.48f}}},
    {"Yamaha YCL-650", {"Yamaha", "YCL-650", "1990", "Bassklarinette", {0.83f, 0.53f, 0.33f, 0.73f, 0.23f, 0.43f}}}
};

// Schlaginstrumente Modelle
const std::map<std::string, InstrumentModel> InstrumentModels::PERCUSSION_MODELS = {
    {"Ludwig Classic Maple", {"Ludwig", "Classic Maple", "1960", "Schlagzeug", {0.9f, 0.6f, 0.4f, 0.8f, 0.3f, 0.5f}}},
    {"Yamaha Recording Custom", {"Yamaha", "Recording Custom", "1980", "Schlagzeug", {0.85f, 0.55f, 0.35f, 0.75f, 0.25f, 0.45f}}},
    {"Pearl Masterworks", {"Pearl", "Masterworks", "2000", "Schlagzeug", {0.88f, 0.58f, 0.38f, 0.78f, 0.28f, 0.48f}}},
    {"Gretsch USA Custom", {"Gretsch", "USA Custom", "1990", "Schlagzeug", {0.82f, 0.52f, 0.32f, 0.72f, 0.22f, 0.42f}}},
    {"Tama Starclassic", {"Tama", "Starclassic", "1995", "Schlagzeug", {0.87f, 0.57f, 0.37f, 0.77f, 0.27f, 0.47f}}}
};

// Weltweite Instrumente Modelle
const std::map<std::string, InstrumentModel> InstrumentModels::WORLD_MODELS = {
    {"Yamaha Koto", {"Yamaha", "Koto", "2000", "Japanische Koto", {0.85f, 0.55f, 0.35f, 0.75f, 0.25f, 0.45f}}},
    {"Meinl Djembe", {"Meinl", "Djembe", "2010", "Afrikanische Djembe", {0.9f, 0.6f, 0.4f, 0.8f, 0.3f, 0.5f}}},
    {"Rikhi Ram Sitar", {"Rikhi Ram", "Sitar", "1980", "Indische Sitar", {0.88f, 0.58f, 0.38f, 0.78f, 0.28f, 0.48f}}},
    {"Meinl Oud", {"Meinl", "Oud", "2015", "Arabische Oud", {0.82f, 0.52f, 0.32f, 0.72f, 0.22f, 0.42f}}},
    {"Meinl Cajon", {"Meinl", "Cajon", "2012", "Peruanische Cajon", {0.87f, 0.57f, 0.37f, 0.77f, 0.27f, 0.47f}}}
};

// Elektrische Instrumente Modelle
const std::map<std::string, InstrumentModel> InstrumentModels::ELECTRIC_MODELS = {
    {"Fender Stratocaster", {"Fender", "Stratocaster", "1954", "E-Gitarre", {0.9f, 0.6f, 0.4f, 0.8f, 0.3f, 0.5f}}},
    {"Gibson Les Paul", {"Gibson", "Les Paul", "1952", "E-Gitarre", {0.85f, 0.55f, 0.35f, 0.75f, 0.25f, 0.45f}}},
    {"Fender Precision Bass", {"Fender", "Precision Bass", "1951", "E-Bass", {0.88f, 0.58f, 0.38f, 0.78f, 0.28f, 0.48f}}},
    {"Roland RD-2000", {"Roland", "RD-2000", "2017", "Stage Piano", {0.82f, 0.52f, 0.32f, 0.72f, 0.22f, 0.42f}}},
    {"Nord Stage 3", {"Nord", "Stage 3", "2017", "Stage Piano", {0.87f, 0.57f, 0.37f, 0.77f, 0.27f, 0.47f}}}
};

// Revolutionäre Instrumente Modelle
const std::map<std::string, InstrumentModel> InstrumentModels::REVOLUTIONARY_MODELS = {
    {"Quantum Synth Q1", {"Quantum Audio", "Q1", "2023", "Quanten-Synthesizer", {0.95f, 0.65f, 0.45f, 0.85f, 0.35f, 0.55f}}},
    {"Neural Space NS-1", {"Neural Audio", "NS-1", "2023", "Neural Space Instrument", {0.92f, 0.62f, 0.42f, 0.82f, 0.32f, 0.52f}}},
    {"Bio Feedback BF-1", {"Bio Audio", "BF-1", "2023", "Bio-Feedback Instrument", {0.88f, 0.58f, 0.38f, 0.78f, 0.28f, 0.48f}}},
    {"Holographic H1", {"Holo Audio", "H1", "2023", "Holographisches Instrument", {0.9f, 0.6f, 0.4f, 0.8f, 0.3f, 0.5f}}},
    {"Time Warp TW-1", {"Time Audio", "TW-1", "2023", "Zeit-Instrument", {0.85f, 0.55f, 0.35f, 0.75f, 0.25f, 0.45f}}}
};

// Neue Spezialinstrumente Modelle
const std::map<std::string, InstrumentModel> InstrumentModels::SPECIAL_MODELS = {
    {"Hang Drum H1", {"PANArt", "Hang", "2000", "Hang Drum", {0.9f, 0.6f, 0.4f, 0.8f, 0.3f, 0.5f}}},
    {"Handpan Spirit", {"Spirit", "Handpan", "2010", "Handpan", {0.85f, 0.55f, 0.35f, 0.75f, 0.25f, 0.45f}}},
    {"Rav Vast", {"Rav", "Vast", "2015", "Rav Vast", {0.88f, 0.58f, 0.38f, 0.78f, 0.28f, 0.48f}}},
    {"Halo Handpan", {"Halo", "Handpan", "2012", "Halo Handpan", {0.82f, 0.52f, 0.32f, 0.72f, 0.22f, 0.42f}}},
    {"Aura Handpan", {"Aura", "Handpan", "2018", "Aura Handpan", {0.87f, 0.57f, 0.37f, 0.77f, 0.27f, 0.47f}}}
}; 