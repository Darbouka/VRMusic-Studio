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
    UIComponentBase.cpp
    components/Button.cpp
    components/Slider.cpp
    components/Pad.cpp
    components/StepSequencer.cpp
)

add_library(VRMusicStudioUI STATIC ${UI_SOURCES})

target_include_directories(VRMusicStudioUI
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(VRMusicStudioUI
    PUBLIC
        VRMusicStudio::Core
        VRMusicStudio::VR
        OpenGL::GL
        glm::glm
)

install(TARGETS VRMusicStudioUI
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
