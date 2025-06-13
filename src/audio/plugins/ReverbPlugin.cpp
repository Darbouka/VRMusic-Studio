#include "ReverbPlugin.hpp"
#include "core/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace VRMusicStudio {
namespace Audio {

struct ReverbPlugin::ReverbImpl {
    static constexpr size_t MAX_DELAY_LENGTH = 44100 * 2; // 2 Sekunden bei 44.1kHz
    std::vector<float> delayBuffer;
    size_t writeIndex;
    std::vector<size_t> delayTimes;
    std::vector<float> feedbackGains;
    std::vector<float> allpassCoeffs;
    std::vector<float> allpassBuffers;
    size_t allpassIndex;

    ReverbImpl() 
        : delayBuffer(MAX_DELAY_LENGTH, 0.0f)
        , writeIndex(0)
        , delayTimes{1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617}
        , feedbackGains(8, 0.5f)
        , allpassCoeffs(4, 0.5f)
        , allpassBuffers(4 * MAX_DELAY_LENGTH, 0.0f)
        , allpassIndex(0) {
    }
};

ReverbPlugin::ReverbPlugin()
    : impl(std::make_unique<ReverbImpl>())
    , roomSize(0.5f)
    , damping(0.5f)
    , wetLevel(0.33f)
    , dryLevel(0.4f)
    , width(1.0f)
    , freeze(false) {
}

ReverbPlugin::~ReverbPlugin() = default;

bool ReverbPlugin::initialize() {
    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Initialisiere Reverb-Plugin...");

        // Initialisiere Parameter
        setRoomSize(roomSize);
        setDamping(damping);
        setWetLevel(wetLevel);
        setDryLevel(dryLevel);
        setWidth(width);
        setFreeze(freeze);

        logger.info("Reverb-Plugin erfolgreich initialisiert");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Reverb-Plugin-Initialisierung: {}", e.what());
        return false;
    }
}

void ReverbPlugin::shutdown() {
    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Beende Reverb-Plugin...");

        // Lösche Puffer
        impl->delayBuffer.clear();
        impl->allpassBuffers.clear();

        logger.info("Reverb-Plugin erfolgreich beendet");
    } catch (const std::exception& e) {
        spdlog::error("Fehler beim Beenden des Reverb-Plugins: {}", e.what());
    }
}

void ReverbPlugin::update() {
    // Aktualisiere Parameter
    if (!freeze) {
        // Aktualisiere Feedback-Gains basierend auf Damping
        for (size_t i = 0; i < impl->feedbackGains.size(); ++i) {
            impl->feedbackGains[i] = std::pow(0.5f, (1.0f - damping) * 2.0f);
        }

        // Aktualisiere Allpass-Koeffizienten basierend auf RoomSize
        for (size_t i = 0; i < impl->allpassCoeffs.size(); ++i) {
            impl->allpassCoeffs[i] = roomSize * 0.5f;
        }
    }
}

void ReverbPlugin::process(float* input, float* output, size_t numFrames) {
    for (size_t i = 0; i < numFrames; ++i) {
        float inputSample = input[i];
        float outputSample = 0.0f;

        // Verzögerungslinien
        for (size_t j = 0; j < impl->delayTimes.size(); ++j) {
            size_t readIndex = (impl->writeIndex + impl->MAX_DELAY_LENGTH - impl->delayTimes[j]) % impl->MAX_DELAY_LENGTH;
            float delayedSample = impl->delayBuffer[readIndex];
            outputSample += delayedSample;
            impl->delayBuffer[impl->writeIndex] = inputSample + delayedSample * impl->feedbackGains[j];
        }

        // Allpass-Filter
        for (size_t j = 0; j < impl->allpassCoeffs.size(); ++j) {
            size_t allpassReadIndex = (impl->allpassIndex + impl->MAX_DELAY_LENGTH - impl->delayTimes[j] / 2) % impl->MAX_DELAY_LENGTH;
            float allpassSample = impl->allpassBuffers[j * impl->MAX_DELAY_LENGTH + allpassReadIndex];
            float allpassOutput = -impl->allpassCoeffs[j] * outputSample + allpassSample;
            impl->allpassBuffers[j * impl->MAX_DELAY_LENGTH + impl->allpassIndex] = outputSample + impl->allpassCoeffs[j] * allpassOutput;
            outputSample = allpassOutput;
        }

        // Mische trockenes und nasses Signal
        output[i] = inputSample * dryLevel + outputSample * wetLevel;

        // Aktualisiere Indizes
        impl->writeIndex = (impl->writeIndex + 1) % impl->MAX_DELAY_LENGTH;
        impl->allpassIndex = (impl->allpassIndex + 1) % impl->MAX_DELAY_LENGTH;
    }
}

void ReverbPlugin::setRoomSize(float size) {
    roomSize = std::clamp(size, 0.0f, 1.0f);
    for (size_t i = 0; i < impl->delayTimes.size(); ++i) {
        impl->delayTimes[i] = static_cast<size_t>(1116 + i * 100) * roomSize;
    }
}

void ReverbPlugin::setDamping(float newDamping) {
    damping = std::clamp(newDamping, 0.0f, 1.0f);
}

void ReverbPlugin::setWetLevel(float level) {
    wetLevel = std::clamp(level, 0.0f, 1.0f);
}

void ReverbPlugin::setDryLevel(float level) {
    dryLevel = std::clamp(level, 0.0f, 1.0f);
}

void ReverbPlugin::setWidth(float newWidth) {
    width = std::clamp(newWidth, 0.0f, 1.0f);
}

void ReverbPlugin::setFreeze(bool newFreeze) {
    freeze = newFreeze;
}

float ReverbPlugin::getRoomSize() const {
    return roomSize;
}

float ReverbPlugin::getDamping() const {
    return damping;
}

float ReverbPlugin::getWetLevel() const {
    return wetLevel;
}

float ReverbPlugin::getDryLevel() const {
    return dryLevel;
}

float ReverbPlugin::getWidth() const {
    return width;
}

bool ReverbPlugin::isFrozen() const {
    return freeze;
}

} // namespace Audio
} // namespace VRMusicStudio 