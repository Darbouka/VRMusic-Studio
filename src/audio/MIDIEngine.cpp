#include "audio/MIDIEngine.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <chrono>

namespace VRMusicStudio {

class MIDIEngineImpl : public MIDIEngine {
private:
    RtMidiIn* midiIn = nullptr;
    RtMidiOut* midiOut = nullptr;
    bool initialized = false;
    std::vector<MIDIDevice> inputDevices;
    std::vector<MIDIDevice> outputDevices;
    std::vector<MIDIHandler*> handlers;
    std::vector<MIDIMessage> messageQueue;
    std::mutex queueMutex;

    static void midiCallback(double timeStamp, std::vector<unsigned char>* message,
                           void* userData) {
        auto* engine = static_cast<MIDIEngineImpl*>(userData);
        engine->handleMIDIMessage(timeStamp, message);
    }

    void handleMIDIMessage(double timeStamp, std::vector<unsigned char>* message) {
        if (!message || message->empty()) return;

        MIDIMessage midiMsg;
        midiMsg.timestamp = timeStamp;
        midiMsg.data = *message;

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            messageQueue.push_back(midiMsg);
        }

        // Benachrichtige alle Handler
        for (auto* handler : handlers) {
            handler->onMIDIMessage(midiMsg);
        }
    }

    void scanDevices() {
        // Scanne MIDI-Eingabegeräte
        inputDevices.clear();
        unsigned int numInputs = midiIn->getPortCount();
        for (unsigned int i = 0; i < numInputs; i++) {
            MIDIDevice device;
            device.name = midiIn->getPortName(i);
            device.id = i;
            device.type = MIDIDeviceType::Input;
            inputDevices.push_back(device);
        }

        // Scanne MIDI-Ausgabegeräte
        outputDevices.clear();
        unsigned int numOutputs = midiOut->getPortCount();
        for (unsigned int i = 0; i < numOutputs; i++) {
            MIDIDevice device;
            device.name = midiOut->getPortName(i);
            device.id = i;
            device.type = MIDIDeviceType::Output;
            outputDevices.push_back(device);
        }
    }

public:
    MIDIEngineImpl() {
        try {
            midiIn = new RtMidiIn();
            midiOut = new RtMidiOut();
        } catch (RtMidiError& error) {
            spdlog::error("MIDI-Engine Initialisierung fehlgeschlagen: {}", error.getMessage());
        }
    }

    ~MIDIEngineImpl() override {
        shutdown();
        delete midiIn;
        delete midiOut;
    }

    bool initialize() override {
        if (initialized) return true;

        try {
            midiIn->setCallback(midiCallback, this);
            midiIn->ignoreTypes(false, false, false);
            scanDevices();
            initialized = true;
            spdlog::info("MIDI-Engine initialisiert");
            return true;
        } catch (RtMidiError& error) {
            spdlog::error("MIDI-Engine Initialisierung fehlgeschlagen: {}", error.getMessage());
            return false;
        }
    }

    void shutdown() override {
        if (!initialized) return;

        try {
            if (midiIn->isPortOpen()) {
                midiIn->closePort();
            }
            if (midiOut->isPortOpen()) {
                midiOut->closePort();
            }
            initialized = false;
            spdlog::info("MIDI-Engine heruntergefahren");
        } catch (RtMidiError& error) {
            spdlog::error("MIDI-Engine Shutdown fehlgeschlagen: {}", error.getMessage());
        }
    }

    bool openInputPort(unsigned int portNumber) override {
        if (!initialized) return false;

        try {
            if (midiIn->isPortOpen()) {
                midiIn->closePort();
            }
            midiIn->openPort(portNumber);
            spdlog::info("MIDI-Eingabeport {} geöffnet", portNumber);
            return true;
        } catch (RtMidiError& error) {
            spdlog::error("MIDI-Eingabeport konnte nicht geöffnet werden: {}", error.getMessage());
            return false;
        }
    }

    bool openOutputPort(unsigned int portNumber) override {
        if (!initialized) return false;

        try {
            if (midiOut->isPortOpen()) {
                midiOut->closePort();
            }
            midiOut->openPort(portNumber);
            spdlog::info("MIDI-Ausgabeport {} geöffnet", portNumber);
            return true;
        } catch (RtMidiError& error) {
            spdlog::error("MIDI-Ausgabeport konnte nicht geöffnet werden: {}", error.getMessage());
            return false;
        }
    }

    void closeInputPort() override {
        if (!initialized) return;

        try {
            if (midiIn->isPortOpen()) {
                midiIn->closePort();
                spdlog::info("MIDI-Eingabeport geschlossen");
            }
        } catch (RtMidiError& error) {
            spdlog::error("MIDI-Eingabeport konnte nicht geschlossen werden: {}", error.getMessage());
        }
    }

    void closeOutputPort() override {
        if (!initialized) return;

        try {
            if (midiOut->isPortOpen()) {
                midiOut->closePort();
                spdlog::info("MIDI-Ausgabeport geschlossen");
            }
        } catch (RtMidiError& error) {
            spdlog::error("MIDI-Ausgabeport konnte nicht geschlossen werden: {}", error.getMessage());
        }
    }

    void sendMessage(const std::vector<unsigned char>& message) override {
        if (!initialized || !midiOut->isPortOpen()) return;

        try {
            midiOut->sendMessage(&message);
        } catch (RtMidiError& error) {
            spdlog::error("MIDI-Nachricht konnte nicht gesendet werden: {}", error.getMessage());
        }
    }

    void addHandler(MIDIHandler* handler) override {
        if (handler) {
            handlers.push_back(handler);
        }
    }

    void removeHandler(MIDIHandler* handler) override {
        handlers.erase(
            std::remove(handlers.begin(), handlers.end(), handler),
            handlers.end()
        );
    }

    const std::vector<MIDIDevice>& getInputDevices() const override {
        return inputDevices;
    }

    const std::vector<MIDIDevice>& getOutputDevices() const override {
        return outputDevices;
    }

    void update() override {
        std::lock_guard<std::mutex> lock(queueMutex);
        messageQueue.clear();
    }
};

std::unique_ptr<MIDIEngine> MIDIEngine::create() {
    return std::make_unique<MIDIEngineImpl>();
}

} // namespace VRMusicStudio 