#include "AudioPlugin.hpp"
#include "Logger.hpp"

namespace VR_DAW {

AudioPlugin::AudioPlugin()
    : sampleRate(44100.0f)
    , blockSize(1024) {
}

AudioPlugin::~AudioPlugin() = default;

} // namespace VR_DAW 