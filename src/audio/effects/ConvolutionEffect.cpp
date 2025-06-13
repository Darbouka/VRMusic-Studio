#include "ConvolutionEffect.hpp"
#include <cmath>
#include <algorithm>

namespace VRMusicStudio {

ConvolutionEffect::ConvolutionEffect() :
    mix(0.5f),
    predelay(0.0f),
    decay(0.5f),
    damping(0.5f),
    width(0.5f),
    quality(1.0f),
    automatedMix(false),
    automatedPredelay(false),
    automatedDecay(false),
    automatedDamping(false),
    automatedWidth(false),
    automatedQuality(false),
    fftSize(2048),
    blockSize(1024),
    overlap(512),
    writePos(0),
    readPos(0),
    irLoaded(false),
    currentMix(0.5f),
    currentPredelay(0.0f),
    currentDecay(0.5f),
    currentDamping(0.5f),
    currentWidth(0.5f),
    currentQuality(1.0f)
{
}

ConvolutionEffect::~ConvolutionEffect() {
    shutdown();
}

bool ConvolutionEffect::initialize() {
    initializeFFT();
    initializeBuffers();
    return true;
}

void ConvolutionEffect::shutdown() {
    impulseResponse.clear();
    inputBuffer.clear();
    outputBuffer.clear();
    fftBuffer.clear();
    irFftBuffer.clear();
}

std::vector<PluginParameter> ConvolutionEffect::getParameters() const {
    return {
        {"mix", 0.5f, 0.0f, 1.0f},
        {"predelay", 0.0f, 0.0f, 100.0f},
        {"decay", 0.5f, 0.0f, 1.0f},
        {"damping", 0.5f, 0.0f, 1.0f},
        {"width", 0.5f, 0.0f, 1.0f},
        {"quality", 1.0f, 0.0f, 1.0f}
    };
}

void ConvolutionEffect::setParameter(const std::string& name, float value) {
    if (name == "mix") mix = value;
    else if (name == "predelay") predelay = value;
    else if (name == "decay") decay = value;
    else if (name == "damping") damping = value;
    else if (name == "width") width = value;
    else if (name == "quality") quality = value;
}

float ConvolutionEffect::getParameter(const std::string& name) const {
    if (name == "mix") return mix;
    else if (name == "predelay") return predelay;
    else if (name == "decay") return decay;
    else if (name == "damping") return damping;
    else if (name == "width") return width;
    else if (name == "quality") return quality;
    return 0.0f;
}

void ConvolutionEffect::setParameterAutomated(const std::string& name, bool automated) {
    if (name == "mix") automatedMix = automated;
    else if (name == "predelay") automatedPredelay = automated;
    else if (name == "decay") automatedDecay = automated;
    else if (name == "damping") automatedDamping = automated;
    else if (name == "width") automatedWidth = automated;
    else if (name == "quality") automatedQuality = automated;
}

bool ConvolutionEffect::isParameterAutomated(const std::string& name) const {
    if (name == "mix") return automatedMix;
    else if (name == "predelay") return automatedPredelay;
    else if (name == "decay") return automatedDecay;
    else if (name == "damping") return automatedDamping;
    else if (name == "width") return automatedWidth;
    else if (name == "quality") return automatedQuality;
    return false;
}

void ConvolutionEffect::processAudio(float* buffer, unsigned long framesPerBuffer) {
    if (!irLoaded) return;
    
    updateParameters();
    processConvolution(buffer, framesPerBuffer);
    applyPredelay(buffer, framesPerBuffer);
    applyDecay(buffer, framesPerBuffer);
    applyDamping(buffer, framesPerBuffer);
    applyWidth(buffer, framesPerBuffer);
    applyMix(buffer, framesPerBuffer);
    applyQuality(buffer, framesPerBuffer);
}

void ConvolutionEffect::loadPreset(const std::string& presetName) {
    // TODO: Implement preset loading
}

void ConvolutionEffect::savePreset(const std::string& presetName) {
    // TODO: Implement preset saving
}

std::vector<std::string> ConvolutionEffect::getAvailablePresets() const {
    return {}; // TODO: Implement preset list
}

bool ConvolutionEffect::loadImpulseResponse(const std::string& filePath) {
    // TODO: Implement impulse response loading
    return false;
}

void ConvolutionEffect::clearImpulseResponse() {
    impulseResponse.clear();
    irLoaded = false;
}

bool ConvolutionEffect::isImpulseResponseLoaded() const {
    return irLoaded;
}

void ConvolutionEffect::updateParameters() {
    currentMix = mix;
    currentPredelay = predelay;
    currentDecay = decay;
    currentDamping = damping;
    currentWidth = width;
    currentQuality = quality;
}

void ConvolutionEffect::processConvolution(float* buffer, unsigned long framesPerBuffer) {
    // Copy input to buffer
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        inputBuffer[writePos] = buffer[i];
        writePos = (writePos + 1) % blockSize;
    }
    
    // Process blocks
    for (unsigned long i = 0; i < framesPerBuffer; i += blockSize - overlap) {
        // Copy block to FFT buffer
        std::fill(fftBuffer.begin(), fftBuffer.end(), std::complex<float>(0.0f, 0.0f));
        for (unsigned long j = 0; j < blockSize; ++j) {
            fftBuffer[j] = std::complex<float>(inputBuffer[(writePos - blockSize + j) % blockSize], 0.0f);
        }
        
        // Apply window
        applyWindow(fftBuffer);
        
        // Perform FFT
        performFFT(fftBuffer);
        
        // Multiply with IR FFT
        for (unsigned long j = 0; j < fftSize; ++j) {
            fftBuffer[j] *= irFftBuffer[j];
        }
        
        // Perform IFFT
        performIFFT(fftBuffer);
        
        // Overlap-add to output
        overlapAdd(outputBuffer, fftBuffer);
    }
    
    // Copy output to buffer
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] = outputBuffer[readPos];
        readPos = (readPos + 1) % blockSize;
    }
}

void ConvolutionEffect::applyPredelay(float* buffer, unsigned long framesPerBuffer) {
    if (currentPredelay <= 0.0f) return;
    
    const float sampleRate = 44100.0f;
    unsigned long delaySamples = static_cast<unsigned long>(currentPredelay * sampleRate / 1000.0f);
    
    std::vector<float> tempBuffer(buffer, buffer + framesPerBuffer);
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        if (i >= delaySamples) {
            buffer[i] = tempBuffer[i - delaySamples];
        } else {
            buffer[i] = 0.0f;
        }
    }
}

void ConvolutionEffect::applyDecay(float* buffer, unsigned long framesPerBuffer) {
    if (currentDecay == 0.5f) return;
    
    const float sampleRate = 44100.0f;
    float decayFactor = calculateDecayFactor(currentDecay, sampleRate);
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] *= std::pow(decayFactor, i);
    }
}

void ConvolutionEffect::applyDamping(float* buffer, unsigned long framesPerBuffer) {
    if (currentDamping == 0.5f) return;
    
    const float sampleRate = 44100.0f;
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float frequency = static_cast<float>(i) * sampleRate / framesPerBuffer;
        float dampingFactor = calculateDampingFactor(currentDamping, frequency);
        buffer[i] *= dampingFactor;
    }
}

void ConvolutionEffect::applyWidth(float* buffer, unsigned long framesPerBuffer) {
    if (currentWidth == 0.5f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float pan = (static_cast<float>(i) / framesPerBuffer) * 2.0f - 1.0f;
        float widthFactor = 1.0f + (currentWidth - 0.5f) * 2.0f;
        buffer[i] *= (1.0f + pan * widthFactor);
    }
}

void ConvolutionEffect::applyMix(float* buffer, unsigned long framesPerBuffer) {
    if (currentMix == 0.5f) return;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        buffer[i] *= currentMix;
    }
}

void ConvolutionEffect::applyQuality(float* buffer, unsigned long framesPerBuffer) {
    if (currentQuality >= 1.0f) return;
    
    // Simple low-pass filter based on quality
    float alpha = currentQuality;
    float prev = buffer[0];
    
    for (unsigned long i = 1; i < framesPerBuffer; ++i) {
        float current = buffer[i];
        buffer[i] = prev + alpha * (current - prev);
        prev = buffer[i];
    }
}

void ConvolutionEffect::initializeFFT() {
    fftBuffer.resize(fftSize);
    irFftBuffer.resize(fftSize);
}

void ConvolutionEffect::initializeBuffers() {
    inputBuffer.resize(blockSize);
    outputBuffer.resize(blockSize);
    writePos = 0;
    readPos = 0;
}

void ConvolutionEffect::resizeBuffers(unsigned long size) {
    inputBuffer.resize(size);
    outputBuffer.resize(size);
    writePos = 0;
    readPos = 0;
}

void ConvolutionEffect::performFFT(std::vector<std::complex<float>>& buffer) {
    // TODO: Implement FFT
}

void ConvolutionEffect::performIFFT(std::vector<std::complex<float>>& buffer) {
    // TODO: Implement IFFT
}

void ConvolutionEffect::applyWindow(std::vector<float>& buffer) {
    // Hanning window
    for (unsigned long i = 0; i < buffer.size(); ++i) {
        float multiplier = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (buffer.size() - 1)));
        buffer[i] *= multiplier;
    }
}

void ConvolutionEffect::overlapAdd(std::vector<float>& output, const std::vector<float>& input) {
    for (unsigned long i = 0; i < input.size(); ++i) {
        output[i] += input[i];
    }
}

float ConvolutionEffect::calculateDecayFactor(float decay, float sampleRate) {
    return std::pow(0.001f, 1.0f / (decay * sampleRate));
}

float ConvolutionEffect::calculateDampingFactor(float damping, float frequency) {
    float cutoff = 1000.0f + damping * 19000.0f;
    return 1.0f / (1.0f + std::exp((frequency - cutoff) / 1000.0f));
}

} // namespace VRMusicStudio 