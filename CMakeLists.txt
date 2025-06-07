cmake_minimum_required(VERSION 3.15)
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
) 