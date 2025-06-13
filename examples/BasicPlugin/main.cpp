#include "audio/plugins/AudioPlugin.hpp"
#include "core/Logger.hpp"
#include <iostream>
#include <vector>
#include <cmath>

namespace VRMusicStudio {
namespace Examples {

class SimpleGainPlugin : public Audio::AudioPlugin {
public:
    SimpleGainPlugin() : gain(1.0f) {}

    bool initialize() override {
        auto& logger = Core::Logger::getInstance();
        logger.info("Initialisiere SimpleGainPlugin...");
        return true;
    }

    void shutdown() override {
        auto& logger = Core::Logger::getInstance();
        logger.info("Beende SimpleGainPlugin...");
    }

    void update() override {
        // Parameter aktualisieren
        gain = std::clamp(gain, 0.0f, 2.0f);
    }

    void process(float* input, float* output, size_t numFrames) override {
        // Einfache Verstärkung
        for (size_t i = 0; i < numFrames; ++i) {
            output[i] = input[i] * gain;
        }
    }

    void setGain(float newGain) {
        gain = newGain;
    }

    float getGain() const {
        return gain;
    }

private:
    float gain;
};

} // namespace Examples
} // namespace VRMusicStudio

int main() {
    try {
        // Logger initialisieren
        auto& logger = VRMusicStudio::Core::Logger::getInstance();
        if (!logger.initialize()) {
            std::cerr << "Fehler bei der Logger-Initialisierung" << std::endl;
            return 1;
        }

        // Plugin erstellen und initialisieren
        VRMusicStudio::Examples::SimpleGainPlugin plugin;
        if (!plugin.initialize()) {
            std::cerr << "Fehler bei der Plugin-Initialisierung" << std::endl;
            return 1;
        }

        // Test-Audio generieren (Sinus-Welle)
        constexpr size_t numFrames = 44100; // 1 Sekunde bei 44.1kHz
        std::vector<float> input(numFrames);
        std::vector<float> output(numFrames);

        for (size_t i = 0; i < numFrames; ++i) {
            input[i] = std::sin(2.0f * M_PI * 440.0f * i / 44100.0f); // 440 Hz
        }

        // Plugin mit verschiedenen Gain-Werten testen
        float gains[] = {0.5f, 1.0f, 1.5f, 2.0f};
        for (float gain : gains) {
            plugin.setGain(gain);
            plugin.update();
            plugin.process(input.data(), output.data(), numFrames);

            // Ergebnisse ausgeben
            std::cout << "Gain: " << gain << std::endl;
            std::cout << "Input Peak: " << *std::max_element(input.begin(), input.end()) << std::endl;
            std::cout << "Output Peak: " << *std::max_element(output.begin(), output.end()) << std::endl;
            std::cout << std::endl;
        }

        // Aufräumen
        plugin.shutdown();
        logger.shutdown();

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fehler: " << e.what() << std::endl;
        return 1;
    }
} 