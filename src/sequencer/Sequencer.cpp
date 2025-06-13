#include "sequencer/Sequencer.hpp"
#include "core/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace VRMusicStudio {
namespace Sequencer {

Sequencer::Sequencer()
    : m_isInitialized(false)
    , m_bpm(120.0)
    , m_isPlaying(false)
    , m_currentPosition(0)
    , m_loopStart(0)
    , m_loopEnd(16)
    , m_quantization(16)
{
}

Sequencer::~Sequencer()
{
    if (m_isInitialized) {
        shutdown();
    }
}

bool Sequencer::initialize()
{
    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Initialisiere Sequencer...");

        // Sequencer-Parameter initialisieren
        m_tracks.clear();
        m_events.clear();
        m_currentPosition = 0;
        m_isPlaying = false;

        m_isInitialized = true;
        logger.info("Sequencer erfolgreich initialisiert");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Fehler bei der Sequencer-Initialisierung: {}", e.what());
        return false;
    }
}

void Sequencer::shutdown()
{
    if (!m_isInitialized) {
        return;
    }

    try {
        auto& logger = Core::Logger::getInstance();
        logger.info("Beende Sequencer...");

        stop();
        m_tracks.clear();
        m_events.clear();

        m_isInitialized = false;
        logger.info("Sequencer erfolgreich beendet");
    } catch (const std::exception& e) {
        spdlog::error("Fehler beim Beenden des Sequencers: {}", e.what());
    }
}

void Sequencer::update(double deltaTime)
{
    if (!m_isInitialized || !m_isPlaying) {
        return;
    }

    // Position aktualisieren
    double stepSize = (60.0 / m_bpm) / m_quantization;
    m_currentPosition += deltaTime / stepSize;

    // Loop-Logik
    if (m_currentPosition >= m_loopEnd) {
        m_currentPosition = m_loopStart;
    }

    // Events abspielen
    int currentStep = static_cast<int>(std::floor(m_currentPosition));
    for (const auto& event : m_events) {
        if (event.step == currentStep) {
            // Event an alle Listener weiterleiten
            for (auto& listener : m_eventListeners) {
                listener->onSequencerEvent(event);
            }
        }
    }
}

void Sequencer::play()
{
    if (!m_isInitialized) {
        return;
    }

    m_isPlaying = true;
    m_currentPosition = m_loopStart;
}

void Sequencer::stop()
{
    m_isPlaying = false;
    m_currentPosition = m_loopStart;
}

void Sequencer::pause()
{
    m_isPlaying = false;
}

void Sequencer::resume()
{
    if (!m_isInitialized) {
        return;
    }

    m_isPlaying = true;
}

void Sequencer::setBPM(double bpm)
{
    m_bpm = std::max(20.0, std::min(300.0, bpm));
}

void Sequencer::setQuantization(int steps)
{
    m_quantization = std::max(1, std::min(64, steps));
}

void Sequencer::setLoopPoints(int start, int end)
{
    m_loopStart = std::max(0, start);
    m_loopEnd = std::max(m_loopStart + 1, end);
}

void Sequencer::addTrack(const std::string& name)
{
    Track track;
    track.name = name;
    track.muted = false;
    track.solo = false;
    m_tracks.push_back(track);
}

void Sequencer::removeTrack(size_t index)
{
    if (index < m_tracks.size()) {
        m_tracks.erase(m_tracks.begin() + index);
    }
}

void Sequencer::addEvent(const SequencerEvent& event)
{
    m_events.push_back(event);
    std::sort(m_events.begin(), m_events.end(),
        [](const SequencerEvent& a, const SequencerEvent& b) {
            return a.step < b.step;
        });
}

void Sequencer::removeEvent(size_t index)
{
    if (index < m_events.size()) {
        m_events.erase(m_events.begin() + index);
    }
}

void Sequencer::addEventListener(std::shared_ptr<SequencerEventListener> listener)
{
    if (listener) {
        m_eventListeners.push_back(listener);
    }
}

void Sequencer::removeEventListener(std::shared_ptr<SequencerEventListener> listener)
{
    auto it = std::find(m_eventListeners.begin(), m_eventListeners.end(), listener);
    if (it != m_eventListeners.end()) {
        m_eventListeners.erase(it);
    }
}

double Sequencer::getBPM() const
{
    return m_bpm;
}

int Sequencer::getQuantization() const
{
    return m_quantization;
}

int Sequencer::getCurrentPosition() const
{
    return static_cast<int>(m_currentPosition);
}

bool Sequencer::isPlaying() const
{
    return m_isPlaying;
}

const std::vector<Track>& Sequencer::getTracks() const
{
    return m_tracks;
}

const std::vector<SequencerEvent>& Sequencer::getEvents() const
{
    return m_events;
}

} // namespace Sequencer
} // namespace VRMusicStudio 