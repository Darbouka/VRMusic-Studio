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
        JUCE::juce_audio_processors
        JUCE::juce_dsp
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