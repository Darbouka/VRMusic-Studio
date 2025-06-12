<<<<<<< HEAD
set(AUDIO_SOURCES
    AudioEngine.cpp
    OpenALIntegration.cpp
)

set(AUDIO_HEADERS
    AudioEngine.hpp
    OpenALIntegration.hpp
)

add_library(VR-DAW-Audio STATIC ${AUDIO_SOURCES} ${AUDIO_HEADERS})
=======
# Audio-Bibliothek
add_library(VRMusicStudio::Audio STATIC
    AudioEngine.cpp
)

target_include_directories(VRMusicStudio::Audio
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(VRMusicStudio::Audio
    PUBLIC
        PortAudio::PortAudio
        OpenAL::OpenAL
        JUCE::juce_audio_basics
        JUCE::juce_audio_devices
        JUCE::juce_audio_formats
        JUCE::juce_audio_processors
        JUCE::juce_audio_utils
    PRIVATE
        spdlog::spdlog
)

# Installationsregeln
install(TARGETS VRMusicStudio::Audio
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/VRMusicStudio/audio
    FILES_MATCHING PATTERN "*.hpp"
)
>>>>>>> 0dff1c4 (init 2)

# OpenAL-Soft spezifische Pfade
set(OPENAL_INCLUDE_DIR "/usr/local/opt/openal-soft/include")
set(OPENAL_LIBRARY_DIR "/usr/local/opt/openal-soft/lib")

<<<<<<< HEAD
target_include_directories(VR-DAW-Audio
=======
target_include_directories(VRMusicStudio::Audio
>>>>>>> 0dff1c4 (init 2)
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${OPENAL_INCLUDE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# OpenAL und SndFile finden
find_package(OpenAL REQUIRED)
find_package(SndFile REQUIRED)

<<<<<<< HEAD
target_link_libraries(VR-DAW-Audio
    PUBLIC
        VR-DAW-Core
        OpenAL::OpenAL
        SndFile::sndfile
        portaudio
=======
target_link_libraries(VRMusicStudio::Audio
    PUBLIC
        VR-DAW-Core
>>>>>>> 0dff1c4 (init 2)
        glm::glm
)

# Linker-Flags für OpenAL
<<<<<<< HEAD
target_link_directories(VR-DAW-Audio
=======
target_link_directories(VRMusicStudio::Audio
>>>>>>> 0dff1c4 (init 2)
    PRIVATE
        ${OPENAL_LIBRARY_DIR}
)

<<<<<<< HEAD
target_compile_features(VR-DAW-Audio PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(VR-DAW-Audio PRIVATE /W4)
else()
    target_compile_options(VR-DAW-Audio PRIVATE -Wall -Wextra -Wpedantic)
=======
target_compile_features(VRMusicStudio::Audio PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(VRMusicStudio::Audio PRIVATE /W4)
else()
    target_compile_options(VRMusicStudio::Audio PRIVATE -Wall -Wextra -Wpedantic)
>>>>>>> 0dff1c4 (init 2)
endif() 