#include "MIDIController.hpp"
#include <RtMidi.h>
#include <algorithm>
#include <stdexcept>

namespace VRMusicStudio {

MIDIController::MIDIController() {
    initialize();
}

MIDIController::~MIDIController() {
    shutdown();
}

bool MIDIController::initialize() {
    try {
        // MIDI-In initialisieren
        midiIn = std::make_unique<RtMidiIn>();
        midiOut = std::make_unique<RtMidiOut>();

        // Verfügbare MIDI-Ports auflisten
        unsigned int numPorts = midiIn->getPortCount();
        for (unsigned int i = 0; i < numPorts; i++) {
            availableInputPorts.push_back(midiIn->getPortName(i));
        }

        numPorts = midiOut->getPortCount();
        for (unsigned int i = 0; i < numPorts; i++) {
            availableOutputPorts.push_back(midiOut->getPortName(i));
        }

        // Standard-Ports öffnen
        if (!availableInputPorts.empty()) {
            midiIn->openPort(0);
        }
        if (!availableOutputPorts.empty()) {
            midiOut->openPort(0);
        }

        // Callback für MIDI-Eingang setzen
        midiIn->setCallback(&MIDIController::midiCallback, this);

        return true;
    } catch (const RtMidiError& error) {
        throw std::runtime_error("Failed to initialize MIDI: " + error.getMessage());
    }
}

void MIDIController::shutdown() {
    if (midiIn) {
        midiIn->closePort();
    }
    if (midiOut) {
        midiOut->closePort();
    }
}

void MIDIController::midiCallback(double timeStamp, std::vector<unsigned char>* message, void* userData) {
    auto* controller = static_cast<MIDIController*>(userData);
    if (!controller) return;

    if (message->size() < 2) return;

    unsigned char status = message->at(0) & 0xF0;
    unsigned char channel = message->at(0) & 0x0F;

    switch (status) {
        case 0x90: // Note On
            if (message->size() >= 3) {
                unsigned char note = message->at(1);
                unsigned char velocity = message->at(2);
                if (velocity > 0) {
                    controller->handleNoteOn(channel, note, velocity);
                } else {
                    controller->handleNoteOff(channel, note, 0);
                }
            }
            break;

        case 0x80: // Note Off
            if (message->size() >= 3) {
                unsigned char note = message->at(1);
                unsigned char velocity = message->at(2);
                controller->handleNoteOff(channel, note, velocity);
            }
            break;

        case 0xE0: // Pitch Bend
            if (message->size() >= 3) {
                unsigned char lsb = message->at(1);
                unsigned char msb = message->at(2);
                int value = ((msb << 7) | lsb) - 8192;
                float normalizedValue = value / 8192.0f;
                controller->handlePitchBend(channel, normalizedValue);
            }
            break;

        case 0xB0: // Control Change
            if (message->size() >= 3) {
                unsigned char control = message->at(1);
                unsigned char value = message->at(2);
                controller->handleControlChange(channel, control, value);
            }
            break;

        case 0xD0: // Aftertouch
            if (message->size() >= 2) {
                unsigned char value = message->at(1);
                controller->handleAftertouch(channel, value);
            }
            break;
    }
}

void MIDIController::handleNoteOn(unsigned char channel, unsigned char note, unsigned char velocity) {
    if (noteCallback) {
        noteCallback(channel, note, velocity, true);
    }
}

void MIDIController::handleNoteOff(unsigned char channel, unsigned char note, unsigned char velocity) {
    if (noteCallback) {
        noteCallback(channel, note, velocity, false);
    }
}

void MIDIController::handlePitchBend(unsigned char channel, float value) {
    if (pitchBendCallback) {
        pitchBendCallback(channel, value);
    }
}

void MIDIController::handleControlChange(unsigned char channel, unsigned char control, unsigned char value) {
    if (controlChangeCallback) {
        controlChangeCallback(channel, control, value);
    }
}

void MIDIController::handleAftertouch(unsigned char channel, unsigned char value) {
    if (aftertouchCallback) {
        aftertouchCallback(channel, value);
    }
}

void MIDIController::sendNoteOn(unsigned char channel, unsigned char note, unsigned char velocity) {
    std::vector<unsigned char> message = {static_cast<unsigned char>(0x90 | channel), note, velocity};
    midiOut->sendMessage(&message);
}

void MIDIController::sendNoteOff(unsigned char channel, unsigned char note, unsigned char velocity) {
    std::vector<unsigned char> message = {static_cast<unsigned char>(0x80 | channel), note, velocity};
    midiOut->sendMessage(&message);
}

void MIDIController::sendPitchBend(unsigned char channel, float value) {
    int intValue = static_cast<int>(value * 8192.0f) + 8192;
    unsigned char lsb = intValue & 0x7F;
    unsigned char msb = (intValue >> 7) & 0x7F;
    std::vector<unsigned char> message = {static_cast<unsigned char>(0xE0 | channel), lsb, msb};
    midiOut->sendMessage(&message);
}

void MIDIController::sendControlChange(unsigned char channel, unsigned char control, unsigned char value) {
    std::vector<unsigned char> message = {static_cast<unsigned char>(0xB0 | channel), control, value};
    midiOut->sendMessage(&message);
}

void MIDIController::sendAftertouch(unsigned char channel, unsigned char value) {
    std::vector<unsigned char> message = {static_cast<unsigned char>(0xD0 | channel), value};
    midiOut->sendMessage(&message);
}

void MIDIController::setNoteCallback(std::function<void(unsigned char, unsigned char, unsigned char, bool)> callback) {
    noteCallback = callback;
}

void MIDIController::setPitchBendCallback(std::function<void(unsigned char, float)> callback) {
    pitchBendCallback = callback;
}

void MIDIController::setControlChangeCallback(std::function<void(unsigned char, unsigned char, unsigned char)> callback) {
    controlChangeCallback = callback;
}

void MIDIController::setAftertouchCallback(std::function<void(unsigned char, unsigned char)> callback) {
    aftertouchCallback = callback;
}

const std::vector<std::string>& MIDIController::getAvailableInputPorts() const {
    return availableInputPorts;
}

const std::vector<std::string>& MIDIController::getAvailableOutputPorts() const {
    return availableOutputPorts;
}

} // namespace VRMusicStudio 