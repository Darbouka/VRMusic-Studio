#include "MixingEngine.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>

namespace VRMusicStudio {

struct MixingEngine::Impl {
    // Mixing-Parameter
    struct Channel {
        std::string name;
        float volume;
        float pan;
        bool muted;
        bool solo;
        std::vector<float> fader;
        std::vector<float> panLaw;
    };
    
    std::vector<Channel> channels;
    float masterVolume;
    bool masterMuted;
    
    // Bus-Parameter
    struct Bus {
        std::string name;
        std::vector<int> channelIndices;
        float volume;
        float pan;
        bool muted;
    };
    
    std::vector<Bus> buses;
    
    // Effekt-Parameter
    struct Effect {
        std::string name;
        std::string type;
        std::map<std::string, float> parameters;
        bool enabled;
    };
    
    std::map<int, std::vector<Effect>> channelEffects;
    std::map<int, std::vector<Effect>> busEffects;
    
    // Zustandsvariablen
    float currentLevel;
    std::vector<float> channelLevels;
    std::vector<float> busLevels;
    
    // Callbacks
    std::function<void(float)> masterLevelCallback;
    std::function<void(const std::vector<float>&)> channelLevelCallback;
    std::function<void(const std::vector<float>&)> busLevelCallback;
    
    Impl() : masterVolume(0.0f), masterMuted(false),
             currentLevel(0.0f) {}
};

MixingEngine::MixingEngine() : pImpl(std::make_unique<Impl>()) {}

MixingEngine::~MixingEngine() = default;

void MixingEngine::createChannel(const std::string& name) {
    Impl::Channel channel;
    channel.name = name;
    channel.volume = 0.0f;
    channel.pan = 0.0f;
    channel.muted = false;
    channel.solo = false;
    channel.fader.resize(1024, 0.0f);
    channel.panLaw.resize(1024, 0.0f);
    
    pImpl->channels.push_back(channel);
    pImpl->channelLevels.push_back(0.0f);
}

void MixingEngine::createBus(const std::string& name) {
    Impl::Bus bus;
    bus.name = name;
    bus.volume = 0.0f;
    bus.pan = 0.0f;
    bus.muted = false;
    
    pImpl->buses.push_back(bus);
    pImpl->busLevels.push_back(0.0f);
}

void MixingEngine::setChannelVolume(int channelIndex, float volume) {
    if (channelIndex >= 0 && channelIndex < pImpl->channels.size()) {
        pImpl->channels[channelIndex].volume = volume;
    }
}

void MixingEngine::setChannelPan(int channelIndex, float pan) {
    if (channelIndex >= 0 && channelIndex < pImpl->channels.size()) {
        pImpl->channels[channelIndex].pan = std::clamp(pan, -1.0f, 1.0f);
    }
}

void MixingEngine::setChannelMute(int channelIndex, bool muted) {
    if (channelIndex >= 0 && channelIndex < pImpl->channels.size()) {
        pImpl->channels[channelIndex].muted = muted;
    }
}

void MixingEngine::setChannelSolo(int channelIndex, bool solo) {
    if (channelIndex >= 0 && channelIndex < pImpl->channels.size()) {
        pImpl->channels[channelIndex].solo = solo;
    }
}

void MixingEngine::setBusVolume(int busIndex, float volume) {
    if (busIndex >= 0 && busIndex < pImpl->buses.size()) {
        pImpl->buses[busIndex].volume = volume;
    }
}

void MixingEngine::setBusPan(int busIndex, float pan) {
    if (busIndex >= 0 && busIndex < pImpl->buses.size()) {
        pImpl->buses[busIndex].pan = std::clamp(pan, -1.0f, 1.0f);
    }
}

void MixingEngine::setBusMute(int busIndex, bool muted) {
    if (busIndex >= 0 && busIndex < pImpl->buses.size()) {
        pImpl->buses[busIndex].muted = muted;
    }
}

void MixingEngine::addChannelToBus(int channelIndex, int busIndex) {
    if (channelIndex >= 0 && channelIndex < pImpl->channels.size() &&
        busIndex >= 0 && busIndex < pImpl->buses.size()) {
        pImpl->buses[busIndex].channelIndices.push_back(channelIndex);
    }
}

void MixingEngine::removeChannelFromBus(int channelIndex, int busIndex) {
    if (busIndex >= 0 && busIndex < pImpl->buses.size()) {
        auto& indices = pImpl->buses[busIndex].channelIndices;
        indices.erase(std::remove(indices.begin(), indices.end(), channelIndex),
                     indices.end());
    }
}

void MixingEngine::addEffectToChannel(int channelIndex, const std::string& name,
                                    const std::string& type,
                                    const std::map<std::string, float>& parameters) {
    if (channelIndex >= 0 && channelIndex < pImpl->channels.size()) {
        Impl::Effect effect;
        effect.name = name;
        effect.type = type;
        effect.parameters = parameters;
        effect.enabled = true;
        
        pImpl->channelEffects[channelIndex].push_back(effect);
    }
}

void MixingEngine::addEffectToBus(int busIndex, const std::string& name,
                                const std::string& type,
                                const std::map<std::string, float>& parameters) {
    if (busIndex >= 0 && busIndex < pImpl->buses.size()) {
        Impl::Effect effect;
        effect.name = name;
        effect.type = type;
        effect.parameters = parameters;
        effect.enabled = true;
        
        pImpl->busEffects[busIndex].push_back(effect);
    }
}

void MixingEngine::setEffectParameter(int channelIndex, int effectIndex,
                                    const std::string& parameter, float value) {
    if (channelIndex >= 0 && channelIndex < pImpl->channels.size() &&
        effectIndex >= 0 && effectIndex < pImpl->channelEffects[channelIndex].size()) {
        pImpl->channelEffects[channelIndex][effectIndex].parameters[parameter] = value;
    }
}

void MixingEngine::setEffectEnabled(int channelIndex, int effectIndex, bool enabled) {
    if (channelIndex >= 0 && channelIndex < pImpl->channels.size() &&
        effectIndex >= 0 && effectIndex < pImpl->channelEffects[channelIndex].size()) {
        pImpl->channelEffects[channelIndex][effectIndex].enabled = enabled;
    }
}

void MixingEngine::processAudio(float* buffer, int numSamples) {
    // Verarbeite Kanäle
    for (size_t i = 0; i < pImpl->channels.size(); ++i) {
        auto& channel = pImpl->channels[i];
        
        // Skip wenn gemuted oder andere Kanäle solo sind
        if (channel.muted || (!channel.solo && std::any_of(pImpl->channels.begin(),
                                                          pImpl->channels.end(),
                                                          [](const Impl::Channel& c) {
                                                              return c.solo;
                                                          }))) {
            continue;
        }
        
        // Wende Kanal-Effekte an
        for (const auto& effect : pImpl->channelEffects[i]) {
            if (effect.enabled) {
                // Implementiere hier die Effekt-Verarbeitung
                // Dies ist eine vereinfachte Version
                for (int j = 0; j < numSamples; j += 2) {
                    float left = buffer[j];
                    float right = buffer[j + 1];
                    
                    // Pan
                    float panGain = std::pow(10.0f, channel.pan / 20.0f);
                    left *= (1.0f - panGain);
                    right *= panGain;
                    
                    // Volume
                    float volume = std::pow(10.0f, channel.volume / 20.0f);
                    left *= volume;
                    right *= volume;
                    
                    buffer[j] = left;
                    buffer[j + 1] = right;
                }
            }
        }
        
        // Update Kanal-Level
        float level = 0.0f;
        for (int j = 0; j < numSamples; j += 2) {
            level = std::max(level,
                           std::max(std::abs(buffer[j]),
                                   std::abs(buffer[j + 1])));
        }
        pImpl->channelLevels[i] = level;
    }
    
    // Verarbeite Buses
    for (size_t i = 0; i < pImpl->buses.size(); ++i) {
        auto& bus = pImpl->buses[i];
        
        if (bus.muted) continue;
        
        // Mixe Kanäle in Bus
        for (int channelIndex : bus.channelIndices) {
            if (channelIndex >= 0 && channelIndex < pImpl->channels.size()) {
                // Implementiere hier das Bus-Mixing
                // Dies ist eine vereinfachte Version
                for (int j = 0; j < numSamples; j += 2) {
                    float left = buffer[j];
                    float right = buffer[j + 1];
                    
                    // Pan
                    float panGain = std::pow(10.0f, bus.pan / 20.0f);
                    left *= (1.0f - panGain);
                    right *= panGain;
                    
                    // Volume
                    float volume = std::pow(10.0f, bus.volume / 20.0f);
                    left *= volume;
                    right *= volume;
                    
                    buffer[j] = left;
                    buffer[j + 1] = right;
                }
            }
        }
        
        // Wende Bus-Effekte an
        for (const auto& effect : pImpl->busEffects[i]) {
            if (effect.enabled) {
                // Implementiere hier die Effekt-Verarbeitung
                // Dies ist eine vereinfachte Version
                for (int j = 0; j < numSamples; j += 2) {
                    float left = buffer[j];
                    float right = buffer[j + 1];
                    
                    // Volume
                    float volume = std::pow(10.0f, bus.volume / 20.0f);
                    left *= volume;
                    right *= volume;
                    
                    buffer[j] = left;
                    buffer[j + 1] = right;
                }
            }
        }
        
        // Update Bus-Level
        float level = 0.0f;
        for (int j = 0; j < numSamples; j += 2) {
            level = std::max(level,
                           std::max(std::abs(buffer[j]),
                                   std::abs(buffer[j + 1])));
        }
        pImpl->busLevels[i] = level;
    }
    
    // Master-Verarbeitung
    if (!pImpl->masterMuted) {
        float masterGain = std::pow(10.0f, pImpl->masterVolume / 20.0f);
        for (int i = 0; i < numSamples; i += 2) {
            buffer[i] *= masterGain;
            buffer[i + 1] *= masterGain;
        }
    }
    
    // Update Master-Level
    float masterLevel = 0.0f;
    for (int i = 0; i < numSamples; i += 2) {
        masterLevel = std::max(masterLevel,
                             std::max(std::abs(buffer[i]),
                                     std::abs(buffer[i + 1])));
    }
    pImpl->currentLevel = masterLevel;
    
    // Callbacks
    if (pImpl->masterLevelCallback) {
        pImpl->masterLevelCallback(masterLevel);
    }
    if (pImpl->channelLevelCallback) {
        pImpl->channelLevelCallback(pImpl->channelLevels);
    }
    if (pImpl->busLevelCallback) {
        pImpl->busLevelCallback(pImpl->busLevels);
    }
}

void MixingEngine::setMasterVolume(float volume) {
    pImpl->masterVolume = volume;
}

void MixingEngine::setMasterMute(bool muted) {
    pImpl->masterMuted = muted;
}

void MixingEngine::setMasterLevelCallback(std::function<void(float)> callback) {
    pImpl->masterLevelCallback = callback;
}

void MixingEngine::setChannelLevelCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->channelLevelCallback = callback;
}

void MixingEngine::setBusLevelCallback(std::function<void(const std::vector<float>&)> callback) {
    pImpl->busLevelCallback = callback;
}

} // namespace VRMusicStudio 