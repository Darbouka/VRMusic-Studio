#pragma once

#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <memory>

namespace VRMusicStudio {

struct MIDIMessage {
    unsigned char status;
    unsigned char data1;
    unsigned char data2;
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
};

using MIDICallback = std::function<void(const MIDIMessage&)>;

class MIDIEngine {
public:
    MIDIEngine();
    ~MIDIEngine();
    
    void initialize();
    void start();
    void stop();
    
    void sendMIDIMessage(const MIDIMessage& message);
    void addCallback(MIDICallback callback);
    void removeCallback(MIDICallback callback);
    
    std::vector<std::string> getInputDevices() const;
    std::vector<std::string> getOutputDevices() const;

private:
    void processMIDIMessage(const std::vector<unsigned char>& message);
    
    class RtMidiIn;
    class RtMidiOut;
    
    std::unique_ptr<RtMidiIn> m_midiIn;
    std::unique_ptr<RtMidiOut> m_midiOut;
    bool m_isInitialized;
    std::vector<MIDICallback> m_callbacks;
};

} // namespace VRMusicStudio 