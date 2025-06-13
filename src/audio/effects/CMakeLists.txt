# Effect sources
set(EFFECT_SOURCES
    RingModulatorEffect.cpp
    WaveShaperEffect.cpp
    ConvolutionEffect.cpp
    LooperEffect.cpp
    VoiceEditorEffect.cpp
    TapeDelayEffect.cpp
    GranularEffect.cpp
    PhaserEffect.cpp
    VibratoEffect.cpp
    BeatGrinderEffect.cpp
    SpecialEffects.cpp
    DistortionEffect.cpp
    ReverseDelayEffect.cpp
    BeatSlicerEffect.cpp
    GateEffect.cpp
    MultiTapDelayEffect.cpp
    GravityFieldEffect.cpp
    PitchShiftEffect.cpp
    QuantumReverbEffect.cpp
    VoiceVocoderEffect.cpp
    QuantumCompressorEffect.cpp
    NeuralResonatorEffect.cpp
    VocoderEffect.cpp
    LimiterEffect.cpp
    ReverbEffect.cpp
    FlangerEffect.cpp
    SaturatorEffect.cpp
    TimeFractalEffect.cpp
    SlapbackDelayEffect.cpp
    DuckingDelayEffect.cpp
    HarmonizerEffect.cpp
    DelayEffect.cpp
    EQEffect.cpp
    PingPongDelayEffect.cpp
    AutoWahEffect.cpp
    QuantumDelayEffect.cpp
    WahWahEffect.cpp
    TremoloEffect.cpp
    ExciterEffect.cpp
    IceverbEffect.cpp
    BioFilterEffect.cpp
    TimeWarpEffect.cpp
    NeuralVocoderEffect.cpp
    BeatBreakerEffect.cpp
    NeuralSaturatorEffect.cpp
    BioMorphEffect.cpp
    ChorusEffect.cpp
)

# Effect library
add_library(VRMusicStudioEffects STATIC ${EFFECT_SOURCES})

target_include_directories(VRMusicStudioEffects
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(VRMusicStudioEffects
    PUBLIC
    VRMusicStudio::Core
    VRMusicStudio::Audio
    ${FFTW3_LIBRARIES}
    ${OPENAL_LIBRARIES}
)

install(TARGETS VRMusicStudioEffects
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
) 