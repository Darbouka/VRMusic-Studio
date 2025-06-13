#pragma once

#include <vector>
#include <memory>
#include <string>
#include <torch/torch.h>

namespace VR_DAW {

class AIMasteringEngine {
public:
    AIMasteringEngine();
    ~AIMasteringEngine();

    // Hauptfunktionen
    void processMastering(const std::vector<float>& input, std::vector<float>& output);
    void processFinalMastering(const std::vector<float>& input, std::vector<float>& output);
    void analyzeTrack(const std::vector<float>& input);

    // Parameter und Einstellungen
    void setGenre(const std::string& genre);
    void setReference(const std::vector<float>& reference);
    void setTargetLoudness(float loudness);
    void setTargetDynamicRange(float range);
    void setStereoWidth(float width);
    void setEQProfile(const std::vector<float>& profile);

    // Analyse und Feedback
    float getCurrentLoudness() const;
    float getCurrentDynamicRange() const;
    float getCurrentStereoWidth() const;
    std::vector<float> getFrequencyResponse() const;
    std::vector<float> getPhaseResponse() const;
    std::vector<float> getTransientResponse() const;

private:
    // KI-Modelle
    struct MasteringModel {
        torch::jit::script::Module model;
        std::vector<float> parameters;
    };

    // Modelle
    MasteringModel masteringModel;
    MasteringModel finalMasteringModel;

    // Analyse-Tools
    struct Analyzer {
        std::vector<float> frequencyResponse;
        std::vector<float> phaseResponse;
        std::vector<float> transientResponse;
        float loudness;
        float dynamicRange;
        float stereoWidth;
    };

    // Parameter
    struct Parameters {
        std::string genre;
        float targetLoudness;
        float targetDynamicRange;
        float targetStereoWidth;
        std::vector<float> reference;
        std::vector<float> eqProfile;
    };

    // Zustandsvariablen
    Analyzer analyzer;
    Parameters parameters;

    // Hilfsmethoden
    void initializeModels();
    void loadModel(const std::string& path, torch::jit::script::Module& model);
    void updateAnalyzer(const std::vector<float>& audio);
    void optimizeParameters();
    void applyProcessing(const std::vector<float>& input, std::vector<float>& output, 
                        torch::jit::script::Module& model);
    void applyEQ(const std::vector<float>& input, std::vector<float>& output);
    void applyCompression(const std::vector<float>& input, std::vector<float>& output);
    void applyStereoProcessing(const std::vector<float>& input, std::vector<float>& output);
    void applyLimiting(const std::vector<float>& input, std::vector<float>& output);
    void applyHarmonicEnhancement(const std::vector<float>& input, std::vector<float>& output);
    void applyTransientEnhancement(const std::vector<float>& input, std::vector<float>& output);
};

} // namespace VR_DAW 