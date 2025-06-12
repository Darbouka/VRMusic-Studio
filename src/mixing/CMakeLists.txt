# Mixing-Modul Konfiguration
file(GLOB_RECURSE MIXING_SOURCES "*.cpp" "*.c")

add_library(mixing STATIC ${MIXING_SOURCES})

target_include_directories(mixing
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/headers
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(mixing
    PUBLIC
        audio
        core
        utils
        ai
        JUCE::juce_audio_processors
        JUCE::juce_dsp
)

# Installation
install(TARGETS mixing
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/headers/
    DESTINATION include/mixing
    FILES_MATCHING PATTERN "*.hpp"
) 