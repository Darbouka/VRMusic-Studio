# MIDI-Modul Konfiguration
file(GLOB_RECURSE MIDI_SOURCES "*.cpp" "*.c")

add_library(midi STATIC ${MIDI_SOURCES})

target_include_directories(midi
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/headers
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(midi
    PUBLIC
        audio
        core
        utils
        JUCE::juce_audio_basics
        JUCE::juce_audio_devices
)

# Installation
install(TARGETS midi
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/headers/
    DESTINATION include/midi
    FILES_MATCHING PATTERN "*.hpp"
) 