cmake_minimum_required(VERSION 3.15)
<<<<<<< HEAD
cmake_policy(VERSION 3.15)

project(VR-DAW VERSION 1.0.0 LANGUAGES CXX)

# vcpkg Integration
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler-Flags
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Find required packages
find_package(nlohmann_json 3.12.0 REQUIRED)
find_package(OpenAL REQUIRED)
find_package(SndFile REQUIRED)

# PortAudio manuell suchen
find_path(PORTAUDIO_INCLUDE_DIR portaudio.h)
find_library(PORTAUDIO_LIBRARY portaudio)
if(NOT PORTAUDIO_INCLUDE_DIR OR NOT PORTAUDIO_LIBRARY)
    message(FATAL_ERROR "PortAudio nicht gefunden!")
endif()
include_directories(${PORTAUDIO_INCLUDE_DIR})
set(PORTAUDIO_LIBRARIES ${PORTAUDIO_LIBRARY})

# glad als statische Library einbinden
add_library(glad STATIC src/thirdparty/glad/glad.c)
target_include_directories(glad PUBLIC src/thirdparty/glad)

find_package(glm REQUIRED)
find_package(Freetype REQUIRED)

# spdlog als header-only verwenden
include_directories(src/thirdparty/spdlog/include)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)

# Unterverzeichnisse hinzufügen
add_subdirectory(src/core)
add_subdirectory(src/audio)
add_subdirectory(src/ui)
add_subdirectory(src/vr)
add_subdirectory(src/plugins)

# Hauptanwendung
add_executable(VR-DAW src/main.cpp)

target_link_libraries(VR-DAW
    PRIVATE
        VR-DAW-Core
        VR-DAW-Audio
        VR-DAW-UI
        VR-DAW-VR-Lib
        VR-DAW-Plugins
        nlohmann_json::nlohmann_json
        OpenAL
        sndfile
        ${PORTAUDIO_LIBRARIES}
        glad
        glm
        freetype
        spdlog
)

# Installation konfigurieren
install(TARGETS VR-DAW-Core
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# Doxygen-Dokumentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generiere API-Dokumentation mit Doxygen"
        VERBATIM
    )
endif()

# WebRTC Konfiguration
set(WEBRTC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/webrtc")
set(WEBRTC_INCLUDE_DIRS
    "${WEBRTC_ROOT}"
    "${WEBRTC_ROOT}/api"
    "${WEBRTC_ROOT}/rtc_base"
    "${WEBRTC_ROOT}/system_wrappers/include"
)

include_directories(${WEBRTC_INCLUDE_DIRS})

# WebRTC Bibliotheken
set(WEBRTC_LIBRARIES
    "${WEBRTC_ROOT}/out/Debug/libwebrtc.a"
)

target_link_libraries(VR-DAW PRIVATE
    ${WEBRTC_LIBRARIES}
=======
project(VRMusicStudio)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE Konfiguration
add_subdirectory(external/JUCE)
juce_add_module(external/JUCE/modules/juce_audio_processors)

# macOS 15 spezifische Einstellungen
if(APPLE)
    add_compile_definitions(JUCE_DISABLE_COREGRAPHICS_FONT_SMOOTHING=1)
    add_compile_definitions(JUCE_DISABLE_COREGRAPHICS_FONT_SMOOTHING=1)
    add_compile_definitions(JUCE_DISABLE_COREGRAPHICS_FONT_SMOOTHING=1)
    add_compile_definitions(JUCE_DISABLE_COREGRAPHICS_FONT_SMOOTHING=1)
    add_compile_definitions(JUCE_DISABLE_COREGRAPHICS_FONT_SMOOTHING=1)
endif()

# OpenVR Konfiguration
set(OPENVR_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/external/openvr)
set(OPENVR_INCLUDE_DIR ${OPENVR_ROOT}/headers)
set(OPENVR_LIBRARY ${OPENVR_ROOT}/lib/osx64/libopenvr_api.dylib)

# Abhängigkeiten
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# GLEW explizit setzen
set(GLEW_INCLUDE_DIR /usr/local/include)
set(GLEW_LIBRARY /usr/local/lib/libGLEW.dylib)

# Quell- und Header-Dateien
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp")

# Ausführbare Datei
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include-Verzeichnisse
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${OPENVR_INCLUDE_DIR}
    ${GLM_INCLUDE_DIRS}
    ${SPDLOG_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${GLFW3_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIR}
    ${JUCE_MODULE_DIR}
)

# Bibliotheken verlinken
target_link_libraries(${PROJECT_NAME}
    ${OPENVR_LIBRARY}
    ${OPENGL_gl_LIBRARY}
    glfw
    ${GLEW_LIBRARY}
    juce::juce_audio_processors
)

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources/
    DESTINATION share/${PROJECT_NAME}/resources
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/models/
    DESTINATION share/${PROJECT_NAME}/models
>>>>>>> 0dff1c4 (init 2)
) 