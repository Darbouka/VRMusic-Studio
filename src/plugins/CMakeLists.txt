<<<<<<< HEAD
add_library(VR-DAW-Plugins
    PluginInterface.hpp
    PluginLoader.cpp
)

target_include_directories(VR-DAW-Plugins
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(VR-DAW-Plugins
    PUBLIC
        VR-DAW-Core
        VR-DAW-Audio
        VR-DAW-UI
)

target_compile_definitions(VR-DAW-Plugins
    PRIVATE
        VR_DAW_PLUGINS_EXPORTS
=======
# Plugins-Bibliothek
add_library(VRMusicStudio::Plugins STATIC
    PluginManager.cpp
)

target_include_directories(VRMusicStudio::Plugins
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(VRMusicStudio::Plugins
    PUBLIC
        JUCE::juce_audio_plugin_client
        JUCE::juce_audio_processors
    PRIVATE
        spdlog::spdlog
)

# Installationsregeln
install(TARGETS VRMusicStudio::Plugins
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/VRMusicStudio/plugins
    FILES_MATCHING PATTERN "*.hpp"
>>>>>>> 0dff1c4 (init 2)
)

# Plugin-Beispiele
add_subdirectory(examples) 