cmake_minimum_required(VERSION 3.15)

# Find required packages
find_package(spdlog REQUIRED)

# JUCE über die zentrale CMake-Datei einbinden
include(JUCE)

# Plugin-Konfiguration
include(${CMAKE_SOURCE_DIR}/cmake/JUCE.cmake)

# Plugin-Quellen
set(PLUGIN_SOURCES
    PluginInterface.hpp
    PluginLoader.cpp
    PluginManager.cpp
)

# Erstelle Plugin-Bibliothek
add_library(VRMusicStudioPlugins STATIC ${PLUGIN_SOURCES})

# Include-Verzeichnisse
target_include_directories(VRMusicStudioPlugins PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Verlinke Bibliotheken
target_link_libraries(VRMusicStudioPlugins PUBLIC
    VRMusicStudioJUCE
    VRMusicStudioCore
    VRMusicStudioAudio
)

# Kompilierungsoptionen
target_compile_features(VRMusicStudioPlugins PRIVATE cxx_std_17)

if(APPLE)
    target_compile_options(VRMusicStudioPlugins PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -std=c++17
        -arch arm64
        -arch x86_64
        -mmacosx-version-min=15.0
    )
endif()

# Installationsregeln
install(TARGETS VRMusicStudioPlugins
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/vrmusicstudio/plugins
    FILES_MATCHING PATTERN "*.hpp"
)

# Plugin-Beispiele
add_subdirectory(examples) 