#pragma once

#include <vector>
#include <memory>
#include <string>
#include <torch/torch.h>

namespace VR_DAW {

class AIMixingEngine {
public:
    AIMixingEngine();
    ~AIMixingEngine();

    // Hauptfunktionen
    void processMixing(const std::vector<float>& input, std::vector<float>& output);
    void processPreMastering(const std::vector<float>& input, std::vector<float>& output);
    void processMastering(const std::vector<float>& input, std::vector<float>& output);
    void processFinalMastering(const std::vector<float>& input, std::vector<float>& output);

    // Parameter und Einstellungen
    void setStyle(const std::string& style);
    void setReference(const std::vector<float>& reference);
    void setTargetLoudness(float loudness);
    void setTargetDynamicRange(float range);

    // Analyse und Feedback
    float getCurrentLoudness() const;
    float getCurrentDynamicRange() const;
    std::vector<float> getFrequencyResponse() const;
    std::vector<float> getStereoImage() const;

private:
    // KI-Modelle
    struct MixingModel {
        torch::jit::script::Module model;
        std::vector<float> parameters;
    };

    struct MasteringModel {
        torch::jit::script::Module model;
        std::vector<float> parameters;
    };

    // Modelle
    MixingModel mixingModel;
    MasteringModel preMasteringModel;
    MasteringModel masteringModel;
    MasteringModel finalMasteringModel;

    // Analyse-Tools
    struct Analyzer {
        std::vector<float> frequencyBands;
        std::vector<float> stereoWidth;
        float loudness;
        float dynamicRange;
    };

    // Parameter
    struct Parameters {
        std::string style;
        float targetLoudness;
        float targetDynamicRange;
        std::vector<float> reference;
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
};

} // namespace VR_DAW 