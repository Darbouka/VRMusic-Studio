#include "AudioEngine.hpp"

namespace VRMusicStudio {
namespace Core {

AudioEngine& AudioEngine::getInstance() {
    static AudioEngine instance;
    return instance;
}

void AudioEngine::initialize() {}
void AudioEngine::shutdown() {}

} // namespace Core
} // namespace VRMusicStudio 