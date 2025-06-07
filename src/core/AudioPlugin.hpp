#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>

namespace VR_DAW {

class AudioPlugin {
public:
    AudioPlugin();
    virtual ~AudioPlugin();

    // Plugin-Informationen
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getVendor() const = 0;
    virtual std::string getCategory() const = 0;

    // Plugin-Lebenszyklus
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void update() = 0;

    // Audio-Verarbeitung
    virtual void processAudio(float* inputBuffer, float* outputBuffer, int numFrames, int numChannels) = 0;
    virtual void setSampleRate(float sampleRate) = 0;
    virtual void setBlockSize(int blockSize) = 0;

    // Parameter-Handling
    virtual int getNumParameters() const = 0;
    virtual std::string getParameterName(int index) const = 0;
    virtual float getParameterValue(int index) const = 0;
    virtual void setParameterValue(int index, float value) = 0;
    virtual float getParameterDefaultValue(int index) const = 0;
    virtual std::string getParameterLabel(int index) const = 0;
    virtual std::string getParameterDisplay(int index) const = 0;

    // Preset-Handling
    virtual int getNumPresets() const = 0;
    virtual std::string getPresetName(int index) const = 0;
    virtual void setPreset(int index) = 0;
    virtual void savePreset(const std::string& name) = 0;
    virtual void deletePreset(int index) = 0;

protected:
    float sampleRate;
    int blockSize;
    std::vector<float> parameters;
    std::vector<std::string> parameterNames;
    std::vector<float> parameterDefaults;
    std::vector<std::string> parameterLabels;
    std::vector<std::string> presets;
    mutable std::mutex mutex;
};

} // namespace VR_DAW 