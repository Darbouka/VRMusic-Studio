#include "MIDIEngine.hpp"
#include <RtMidi.h>
#include <stdexcept>
#include <algorithm>

namespace VRMusicStudio {

MIDIEngine::MIDIEngine()
    : m_midiIn(new RtMidiIn())
    , m_midiOut(new RtMidiOut())
    , m_isInitialized(false)
{
    initialize();
}

MIDIEngine::~MIDIEngine()
{
    if (m_isInitialized) {
        stop();
    }
}

void MIDIEngine::initialize()
{
    try {
        m_midiIn->openPort(0);
        m_midiOut->openPort(0);
        m_isInitialized = true;
    } catch (RtMidiError& error) {
        throw std::runtime_error("Failed to initialize MIDI: " + std::string(error.getMessage()));
    }
}

void MIDIEngine::start()
{
    if (!m_isInitialized) {
        throw std::runtime_error("MIDI engine not initialized");
    }

    m_midiIn->setCallback([](double timeStamp, std::vector<unsigned char>* message, void* userData) {
        auto* engine = static_cast<MIDIEngine*>(userData);
        if (engine && message) {
            engine->processMIDIMessage(*message);
        }
    }, this);
}

void MIDIEngine::stop()
{
    if (m_isInitialized) {
        m_midiIn->closePort();
        m_midiOut->closePort();
    }
}

void MIDIEngine::processMIDIMessage(const std::vector<unsigned char>& message)
{
    if (message.empty()) return;

    MIDIMessage midiMessage;
    midiMessage.status = message[0];
    midiMessage.data1 = message.size() > 1 ? message[1] : 0;
    midiMessage.data2 = message.size() > 2 ? message[2] : 0;
    midiMessage.timestamp = std::chrono::high_resolution_clock::now();

    for (const auto& callback : m_callbacks) {
        callback(midiMessage);
    }
}

void MIDIEngine::sendMIDIMessage(const MIDIMessage& message)
{
    if (!m_isInitialized) return;

    std::vector<unsigned char> midiData = {
        static_cast<unsigned char>(message.status),
        static_cast<unsigned char>(message.data1),
        static_cast<unsigned char>(message.data2)
    };

    try {
        m_midiOut->sendMessage(&midiData);
    } catch (RtMidiError& error) {
        throw std::runtime_error("Failed to send MIDI message: " + std::string(error.getMessage()));
    }
}

void MIDIEngine::addCallback(MIDICallback callback)
{
    m_callbacks.push_back(callback);
}

void MIDIEngine::removeCallback(MIDICallback callback)
{
    auto it = std::find(m_callbacks.begin(), m_callbacks.end(), callback);
    if (it != m_callbacks.end()) {
        m_callbacks.erase(it);
    }
}

std::vector<std::string> MIDIEngine::getInputDevices() const
{
    std::vector<std::string> devices;
    unsigned int numPorts = m_midiIn->getPortCount();
    
    for (unsigned int i = 0; i < numPorts; ++i) {
        try {
            devices.push_back(m_midiIn->getPortName(i));
        } catch (RtMidiError&) {
            // Ignore errors for individual ports
        }
    }
    
    return devices;
}

std::vector<std::string> MIDIEngine::getOutputDevices() const
{
    std::vector<std::string> devices;
    unsigned int numPorts = m_midiOut->getPortCount();
    
    for (unsigned int i = 0; i < numPorts; ++i) {
        try {
            devices.push_back(m_midiOut->getPortName(i));
        } catch (RtMidiError&) {
            // Ignore errors for individual ports
        }
    }
    
    return devices;
}

} // namespace VRMusicStudio 