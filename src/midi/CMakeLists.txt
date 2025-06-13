# MIDI-Modul Konfiguration
set(MIDI_SOURCES
    MIDIManager.cpp
    MIDIDevice.cpp
    MIDIController.cpp
)

set(MIDI_HEADERS
    ${CMAKE_SOURCE_DIR}/include/midi/MIDIManager.hpp
    ${CMAKE_SOURCE_DIR}/include/midi/MIDIDevice.hpp
    ${CMAKE_SOURCE_DIR}/include/midi/MIDIController.hpp
)

add_library(VRMusicStudioMIDI STATIC
    ${MIDI_SOURCES}
    ${MIDI_HEADERS}
)

target_include_directories(VRMusicStudioMIDI PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(VRMusicStudioMIDI PUBLIC
    VRMusicStudioCore
    JUCE::juce_audio_basics
    JUCE::juce_audio_devices
    JUCE::juce_audio_formats
    JUCE::juce_audio_processors
    JUCE::juce_audio_utils
    JUCE::juce_core
    JUCE::juce_data_structures
    JUCE::juce_events
    JUCE::juce_graphics
    JUCE::juce_gui_basics
    JUCE::juce_gui_extra
)

target_compile_features(VRMusicStudioMIDI PUBLIC cxx_std_17)

if(APPLE)
    target_compile_options(VRMusicStudioMIDI PRIVATE
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

install(TARGETS VRMusicStudioMIDI
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
) 