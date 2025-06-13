#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace VRMusicStudio {

class VocoderEngine {
public:
    // Vocoder-Modi
    enum class VocoderMode {
        // Standard-Modi
        Standard,
        Robot,
        Choir,
        Whisper,
        
        // Psychedelische Modis
        Psychedelic,
        Acid,
        Space,
        Dream,
        
        // Elektronische Modis
        Techno,
        Trance,
        House,
        Dubstep,
        DrumAndBass,
        Industrial,
        
        // Weltweite Modis
        Indian,
        Arabic,
        African,
        Asian,
        Latin,
        Celtic,
        
        // Experimentelle Modis
        Glitch,
        Granular,
        Spectral,
        Experimental
    };

    // Vocoder-Parameter
    struct VocoderParameters {
        std::string name;
        VocoderMode mode;
        std::map<std::string, float> parameters;
        bool enabled;
        std::vector<float> buffer;
        
        // Genre-spezifische Parameter
        struct {
            // Psychedelische Parameter
            float lsd;
            float trippy;
            float space;
            float dream;
            
            // Elektronische Parameter
            float techno;
            float trance;
            float house;
            float dubstep;
            float dnb;
            float industrial;
            
            // Weltweite Parameter
            float indian;
            float arabic;
            float african;
            float asian;
            float latin;
            float celtic;
            
            // Experimentelle Parameter
            float glitch;
            float granular;
            float spectral;
            float experimental;
        } genreParams;
    };

    // Voice-Editor-Parameter
    struct VoiceEditorParameters {
        std::string name;
        std::string type;
        std::map<std::string, float> parameters;
        bool enabled;
        std::vector<float> buffer;
        
        // Voice-Editor-spezifische Parameter
        float pitch;
        float formant;
        float breath;
        float vibrato;
        float tremolo;
        float chorus;
        float phaser;
        float flanger;
    };

    VocoderEngine();
    ~VocoderEngine();

    // Vocoder-Funktionen
    void createVocoder(const std::string& name, VocoderMode mode);
    void setVocoderParameter(const std::string& name, const std::string& parameter, float value);
    void setVocoderEnabled(const std::string& name, bool enabled);
    void processVocoder(float* buffer, int numSamples);

    // Voice-Editor-Funktionen
    void createVoiceEditor(const std::string& name);
    void setVoiceEditorParameter(const std::string& name, const std::string& parameter, float value);
    void setVoiceEditorEnabled(const std::string& name, bool enabled);
    void processVoiceEditor(float* buffer, int numSamples);

    // Genre-spezifische Funktionen
    void setPsychedelicParameters(const std::string& name, float lsd, float trippy, float space, float dream);
    void setElectronicParameters(const std::string& name, float techno, float trance, float house, float dubstep, float dnb, float industrial);
    void setWorldParameters(const std::string& name, float indian, float arabic, float african, float asian, float latin, float celtic);
    void setExperimentalParameters(const std::string& name, float glitch, float granular, float spectral, float experimental);

    // Callback-Setter
    void setVocoderCallback(std::function<void(const std::vector<float>&)> callback);
    void setVoiceEditorCallback(std::function<void(const std::vector<float>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; 