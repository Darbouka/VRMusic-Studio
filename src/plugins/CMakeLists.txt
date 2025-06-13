cmake_minimum_required(VERSION 3.15)

# Find required packages
find_package(spdlog REQUIRED)
find_package(JUCE REQUIRED)

# Plugins-Bibliothek
add_library(plugins STATIC
    PluginInterface.hpp
    PluginLoader.cpp
    PluginManager.cpp
)

target_include_directories(plugins
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(plugins
    PUBLIC
        core
        audio
        ui
        JUCE::juce_audio_plugin_client
        JUCE::juce_audio_processors
    PRIVATE
        spdlog::spdlog
)

target_compile_definitions(plugins PRIVATE
    VR_DAW_PLUGINS_EXPORTS
)

target_compile_features(plugins PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(plugins PRIVATE /W4)
else()
    target_compile_options(plugins PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Installationsregeln
install(TARGETS plugins
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/vrmusicstudio/plugins
    FILES_MATCHING PATTERN "*.hpp"
)

# Plugin-Beispiele
add_subdirectory(examples) 