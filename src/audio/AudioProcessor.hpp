#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include "core/Logger.hpp"

namespace VRMusicStudio {
namespace Audio {

class AudioProcessor {
public:
    static AudioProcessor& getInstance();
    
    bool initialize();
    void shutdown();
    
    // Signal-Verarbeitung
    void processAudio(float* buffer, size_t numFrames);
    void setSampleRate(float sampleRate);
    void setBufferSize(size_t bufferSize);
    
    // Effekt-Kette
    void addEffect(const std::string& effectType);
    void removeEffect(const std::string& effectType);
    void clearEffects();
    
    // Parameter-Steuerung
    void setParameter(const std::string& effectType, const std::string& paramName, float value);
    float getParameter(const std::string& effectType, const std::string& paramName) const;
    
    // Audio-Analyse
    void analyzeSpectrum(const float* buffer, size_t numFrames);
    void getFrequencyData(float* frequencies, size_t numBins);
    
    // MIDI-Integration
    void processMidiMessage(const uint8_t* message, size_t length);
    void setMidiCallback(std::function<void(const uint8_t*, size_t)> callback);
    
    // Performance-Monitoring
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    float getCPUUsage() const;

private:
    AudioProcessor();
    ~AudioProcessor();
    AudioProcessor(const AudioProcessor&) = delete;
    AudioProcessor& operator=(const AudioProcessor&) = delete;
    
    bool initializeDSP();
    void shutdownDSP();
    
    Core::Logger& logger_;
    float sampleRate_;
    size_t bufferSize_;
    bool isInitialized_;
    std::vector<std::string> activeEffects_;
    std::function<void(const uint8_t*, size_t)> midiCallback_;
};

} // namespace Audio
} // namespace VRMusicStudio 