# Mastering-Modul Konfiguration
file(GLOB_RECURSE MASTERING_SOURCES "*.cpp" "*.c")

add_library(mastering STATIC ${MASTERING_SOURCES})

target_include_directories(mastering
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/headers
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(mastering
    PUBLIC
        audio
        core
        utils
        ai
        JUCE::juce_audio_processors
        JUCE::juce_dsp
)

# Installation
install(TARGETS mastering
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/headers/
    DESTINATION include/mastering
    FILES_MATCHING PATTERN "*.hpp"
) 