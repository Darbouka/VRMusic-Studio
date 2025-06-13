#include "DelayPlugin.hpp"
#include "core/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace VRMusicStudio {
namespace Audio {

struct DelayPlugin::DelayImpl {
    static constexpr size_t MAX_DELAY_LENGTH = 44100 * 2; // 2 Sekunden bei 44.1kHz
    std::vector<float> delayBuffer;
    size_t writeIndex;
    float lowPassCoeff;
    float highPassCoeff;
    float lastLowPass;
    float lastHighPass;

    DelayImpl() 
        : delayBuffer(MAX_DELAY_LENGTH, 0.0f)
        , writeIndex(0)
        , lowPassCoeff(0.0f)
        , highPassCoeff(0.0f)
        , lastLowPass(0.0f)
        , lastHighPass(0.0f) {
    }
};

DelayPlugin::DelayPlugin()
    : impl(std::make_unique<DelayImpl>())
    , delayTime(0.5f)
    , feedback(0.3f)
    , mix(0.5f)
    , lowPassCutoff(20000.0f)
    , highPassCutoff(20.0f)
    , pingPong(false) {
}

DelayPlugin::~DelayPlugin() = default;

bool DelayPlugin::initialize() {
    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Initialisiere Delay-Plugin...");

        // Initialisiere Parameter
        setDelayTime(delayTime);
        setFeedback(feedback);
        setMix(mix);
        setLowPassCutoff(lowPassCutoff);
        setHighPassCutoff(highPassCutoff);
        setPingPong(pingPong);

        logger.info("Delay-Plugin erfolgreich initialisiert");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Delay-Plugin-Initialisierung: {}", e.what());
        return false;
    }
}

void DelayPlugin::shutdown() {
    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Beende Delay-Plugin...");

        // Lösche Puffer
        impl->delayBuffer.clear();

        logger.info("Delay-Plugin erfolgreich beendet");
    } catch (const std::exception& e) {
        spdlog::error("Fehler beim Beenden des Delay-Plugins: {}", e.what());
    }
}

void DelayPlugin::update() {
    // Aktualisiere Filter-Koeffizienten
    float dt = 1.0f / 44100.0f; // Beispiel-Samplerate
    float RC = 1.0f / (2.0f * M_PI * lowPassCutoff);
    impl->lowPassCoeff = dt / (dt + RC);
    RC = 1.0f / (2.0f * M_PI * highPassCutoff);
    impl->highPassCoeff = RC / (dt + RC);
}

void DelayPlugin::process(float* input, float* output, size_t numFrames) {
    size_t delaySamples = static_cast<size_t>(delayTime * 44100.0f); // Beispiel-Samplerate

    for (size_t i = 0; i < numFrames; ++i) {
        float inputSample = input[i];
        float delayedSample = 0.0f;

        // Lese verzögertes Sample
        size_t readIndex = (impl->writeIndex + impl->MAX_DELAY_LENGTH - delaySamples) % impl->MAX_DELAY_LENGTH;
        delayedSample = impl->delayBuffer[readIndex];

        // Wende Filter an
        float lowPassed = impl->lowPassCoeff * delayedSample + (1.0f - impl->lowPassCoeff) * impl->lastLowPass;
        impl->lastLowPass = lowPassed;

        float highPassed = impl->highPassCoeff * (lowPassed - impl->lastHighPass);
        impl->lastHighPass = lowPassed;

        // Ping-Pong-Effekt
        if (pingPong) {
            float pan = std::sin(2.0f * M_PI * static_cast<float>(i) / numFrames);
            output[i] = inputSample + highPassed * mix * (1.0f + pan);
            output[i + 1] = inputSample + highPassed * mix * (1.0f - pan);
            i++; // Überspringe einen Frame für Stereo
        } else {
            output[i] = inputSample + highPassed * mix;
        }

        // Schreibe in Delay-Puffer
        impl->delayBuffer[impl->writeIndex] = inputSample + highPassed * feedback;

        // Aktualisiere Schreibindex
        impl->writeIndex = (impl->writeIndex + 1) % impl->MAX_DELAY_LENGTH;
    }
}

void DelayPlugin::setDelayTime(float timeInSeconds) {
    delayTime = std::clamp(timeInSeconds, 0.0f, 2.0f);
}

void DelayPlugin::setFeedback(float newFeedback) {
    feedback = std::clamp(newFeedback, 0.0f, 0.99f);
}

void DelayPlugin::setMix(float newMix) {
    mix = std::clamp(newMix, 0.0f, 1.0f);
}

void DelayPlugin::setLowPassCutoff(float cutoff) {
    lowPassCutoff = std::clamp(cutoff, 20.0f, 20000.0f);
}

void DelayPlugin::setHighPassCutoff(float cutoff) {
    highPassCutoff = std::clamp(cutoff, 20.0f, 20000.0f);
}

void DelayPlugin::setPingPong(bool enabled) {
    pingPong = enabled;
}

float DelayPlugin::getDelayTime() const {
    return delayTime;
}

float DelayPlugin::getFeedback() const {
    return feedback;
}

float DelayPlugin::getMix() const {
    return mix;
}

float DelayPlugin::getLowPassCutoff() const {
    return lowPassCutoff;
}

float DelayPlugin::getHighPassCutoff() const {
    return highPassCutoff;
}

bool DelayPlugin::isPingPongEnabled() const {
    return pingPong;
}

} // namespace Audio
} // namespace VRMusicStudio 