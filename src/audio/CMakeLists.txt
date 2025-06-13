# Effects-Modul Konfiguration
file(GLOB_RECURSE EFFECT_SOURCES "*.cpp" "*.c")

add_library(effects STATIC ${EFFECT_SOURCES})

target_include_directories(effects
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/headers
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(effects
    PUBLIC
        audio
        core
        utils
)

# Installation
install(TARGETS effects
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/headers/
    DESTINATION include/effects
    FILES_MATCHING PATTERN "*.hpp"
)

# Audio-Bibliothek
add_library(audio
    AudioEngine.cpp
    AudioProcessor.cpp
    AudioPlugin.cpp
    AudioTrack.cpp
    OpenALIntegration.cpp
    ProfessionalMixers.cpp
    Synthesizers.cpp
    Samplers.cpp
    InstrumentModels.cpp
    DrumMachine.cpp
    VocoderEngine.cpp
    VirtualRooms.cpp
    InstrumentEngine.cpp
    MixerEngine.cpp
    EffectEngine.cpp
    MidiTrack.cpp
)

target_include_directories(audio
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/audio
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/audio
)

target_link_libraries(audio
    PUBLIC
        core
        spdlog::spdlog
        fmt::fmt
)

# Audio components
add_subdirectory(effects)
add_subdirectory(instruments)
add_subdirectory(processing)

# Audio library
add_library(VRMusicStudioAudio
    AudioEngine.cpp
    AudioProcessor.cpp
    AudioTrack.cpp
    MidiTrack.cpp
    OpenALIntegration.cpp
    VocoderEngine.cpp
    PresetManager.cpp
    Samplers.cpp
    Synthesizers.cpp
    InstrumentEngine.cpp
)

target_include_directories(VRMusicStudioAudio
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(VRMusicStudioAudio
    PUBLIC
    VRMusicStudio::Core
    VRMusicStudio::Effects
    VRMusicStudio::Instruments
    VRMusicStudio::Processing
    ${PORTAUDIO_LIBRARIES}
    ${OPENAL_LIBRARIES}
    ${FFTW3_LIBRARIES}
)

install(TARGETS VRMusicStudioAudio
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
) 