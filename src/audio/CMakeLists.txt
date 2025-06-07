set(AUDIO_SOURCES
    AudioEngine.cpp
    OpenALIntegration.cpp
)

set(AUDIO_HEADERS
    AudioEngine.hpp
    OpenALIntegration.hpp
)

add_library(VR-DAW-Audio STATIC ${AUDIO_SOURCES} ${AUDIO_HEADERS})

# OpenAL-Soft spezifische Pfade
set(OPENAL_INCLUDE_DIR "/usr/local/opt/openal-soft/include")
set(OPENAL_LIBRARY_DIR "/usr/local/opt/openal-soft/lib")

target_include_directories(VR-DAW-Audio
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${OPENAL_INCLUDE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# OpenAL und SndFile finden
find_package(OpenAL REQUIRED)
find_package(SndFile REQUIRED)

target_link_libraries(VR-DAW-Audio
    PUBLIC
        VR-DAW-Core
        OpenAL::OpenAL
        SndFile::sndfile
        portaudio
        glm::glm
)

# Linker-Flags für OpenAL
target_link_directories(VR-DAW-Audio
    PRIVATE
        ${OPENAL_LIBRARY_DIR}
)

target_compile_features(VR-DAW-Audio PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(VR-DAW-Audio PRIVATE /W4)
else()
    target_compile_options(VR-DAW-Audio PRIVATE -Wall -Wextra -Wpedantic)
endif() 