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
add_subdirectory(processing)

# Audio-Bibliothek
set(AUDIO_SOURCES
    AudioBuffer.cpp
    AudioStream.cpp
    effects/DistortionEffect.cpp
    processing/AudioProcessor.cpp
)

set(AUDIO_HEADERS
    ${CMAKE_SOURCE_DIR}/include/audio/AudioBuffer.hpp
    ${CMAKE_SOURCE_DIR}/include/audio/AudioStream.hpp
    ${CMAKE_SOURCE_DIR}/include/audio/effects/DistortionEffect.hpp
    ${CMAKE_SOURCE_DIR}/include/audio/processing/AudioProcessor.hpp
)

add_library(VRMusicStudioAudio STATIC
    ${AUDIO_SOURCES}
    ${AUDIO_HEADERS}
)

target_include_directories(VRMusicStudioAudio PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(VRMusicStudioAudio PUBLIC
    VRMusicStudioCore
    PortAudio::PortAudio
    spdlog::spdlog
)

target_compile_features(VRMusicStudioAudio PUBLIC cxx_std_17)

if(APPLE)
    target_compile_options(VRMusicStudioAudio PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -std=c++17
        -arch arm64
        -arch x86_64
        -mmacosx-version-min=15.5
    )
endif()

install(TARGETS VRMusicStudioAudio
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
) 