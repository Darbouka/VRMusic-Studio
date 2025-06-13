cmake_minimum_required(VERSION 3.15)

# Find required packages
find_package(spdlog REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)

# UI-Bibliothek
add_library(ui
    UIComponents.cpp
    UIManager.cpp
    StudioLayout.cpp
    RoomManager.cpp
    DesktopSpace.cpp
    VisualizationEngine.cpp
    GUIEngine.cpp
    AudioControlPanel.cpp
    UIEngine.cpp
)

target_include_directories(ui
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/ui
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/ui
)

target_link_libraries(ui
    PUBLIC
        core
        spdlog::spdlog
        fmt::fmt
        GLEW::GLEW
        OpenGL::GL
)

target_compile_definitions(ui PRIVATE
    VR_DAW_UI_EXPORTS
)

target_compile_features(ui PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(ui PRIVATE /W4)
else()
    target_compile_options(ui PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Installationsregeln
install(TARGETS ui
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/vrmusicstudio/ui
    FILES_MATCHING PATTERN "*.hpp"
)

set(UI_SOURCES
    UIComponents.cpp
    MainWindow.cpp
    AudioWindow.cpp
    MIDIWindow.cpp
    VRWindow.cpp
)

set(UI_HEADERS
    ${CMAKE_SOURCE_DIR}/include/ui/UIComponents.hpp
    ${CMAKE_SOURCE_DIR}/include/ui/MainWindow.hpp
    ${CMAKE_SOURCE_DIR}/include/ui/AudioWindow.hpp
    ${CMAKE_SOURCE_DIR}/include/ui/MIDIWindow.hpp
    ${CMAKE_SOURCE_DIR}/include/ui/VRWindow.hpp
)

add_library(VRMusicStudioUI STATIC
    ${UI_SOURCES}
    ${UI_HEADERS}
)

target_include_directories(VRMusicStudioUI PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(VRMusicStudioUI PUBLIC
    VRMusicStudioCore
    VRMusicStudioAudio
    VRMusicStudioMIDI
    VRMusicStudioVR
    SDL2::SDL2
    OpenGL::GL
    GLEW::GLEW
)

target_compile_features(VRMusicStudioUI PUBLIC cxx_std_17)

if(APPLE)
    target_compile_options(VRMusicStudioUI PRIVATE
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

install(TARGETS VRMusicStudioUI
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
