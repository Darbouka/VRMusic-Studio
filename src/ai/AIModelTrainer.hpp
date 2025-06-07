#pragma once

#include <torch/torch.h>
#include <string>
#include <vector>
#include <memory>

namespace VR_DAW {

class AIModelTrainer {
public:
    AIModelTrainer();
    ~AIModelTrainer();

    // Hauptfunktionen
    void initialize();
    void trainMixingModel(const std::string& datasetPath);
    void trainMasteringModel(const std::string& datasetPath);
    void saveModel(const std::string& path);
    void loadModel(const std::string& path);

    // Training-Parameter
    void setLearningRate(float rate);
    void setBatchSize(int size);
    void setEpochs(int epochs);
    void setValidationSplit(float split);
    void setEarlyStopping(int patience);

    // Training-Status
    float getCurrentLoss() const;
    float getValidationLoss() const;
    int getCurrentEpoch() const;
    bool isTraining() const;
    void stopTraining();

private:
    // Netzwerk-Architektur
    struct MixingNetwork : torch::nn::Module {
        MixingNetwork();
        torch::Tensor forward(torch::Tensor x);
        
        torch::nn::Conv1d conv1{nullptr}, conv2{nullptr}, conv3{nullptr};
        torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};
        torch::nn::BatchNorm1d bn1{nullptr}, bn2{nullptr}, bn3{nullptr};
    };

    struct MasteringNetwork : torch::nn::Module {
        MasteringNetwork();
        torch::Tensor forward(torch::Tensor x);
        
        torch::nn::Conv1d conv1{nullptr}, conv2{nullptr}, conv3{nullptr};
        torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};
        torch::nn::BatchNorm1d bn1{nullptr}, bn2{nullptr}, bn3{nullptr};
    };

    // Training-Komponenten
    std::shared_ptr<MixingNetwork> mixingNetwork;
    std::shared_ptr<MasteringNetwork> masteringNetwork;
    torch::optim::Adam mixingOptimizer{nullptr};
    torch::optim::Adam masteringOptimizer{nullptr};

    // Training-Parameter
    float learningRate;
    int batchSize;
    int epochs;
    float validationSplit;
    int earlyStoppingPatience;

    // Training-Zustand
    float currentLoss;
    float validationLoss;
    int currentEpoch;
    bool isTrainingActive;
    int bestEpoch;
    float bestValidationLoss;

    // Hilfsmethoden
    void initializeNetworks();
    void prepareDataset(const std::string& path);
    torch::Tensor preprocessAudio(const std::vector<float>& audio);
    torch::Tensor postprocessOutput(const torch::Tensor& output);
    void trainEpoch();
    void validateEpoch();
    void updateMetrics();
    void checkEarlyStopping();
    void saveCheckpoint();
    void loadCheckpoint();
};

} // namespace VR_DAW 