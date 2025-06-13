cmake_minimum_required(VERSION 3.15)

# Find required packages
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)

# VR sources
set(VR_SOURCES
    VRManager.cpp
    VRDevice.cpp
    VRController.cpp
    VRHeadset.cpp
)

set(VR_HEADERS
    ${CMAKE_SOURCE_DIR}/include/vr/VRManager.hpp
    ${CMAKE_SOURCE_DIR}/include/vr/VRDevice.hpp
    ${CMAKE_SOURCE_DIR}/include/vr/VRController.hpp
    ${CMAKE_SOURCE_DIR}/include/vr/VRHeadset.hpp
)

# VR library
add_library(VRMusicStudioVR STATIC
    ${VR_SOURCES}
    ${VR_HEADERS}
)

target_include_directories(VRMusicStudioVR PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(VRMusicStudioVR PUBLIC
    VRMusicStudioCore
    OpenGL::GL
    GLEW::GLEW
    Vulkan::Vulkan
)

target_compile_features(VRMusicStudioVR PUBLIC cxx_std_17)

if(APPLE)
    target_compile_options(VRMusicStudioVR PRIVATE
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

install(TARGETS VRMusicStudioVR
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# VR-Bibliothek
add_library(vr
    VRDisplay.cpp
    VRRenderer.cpp
    VREngine.cpp
    VRManager.cpp
    VRController.cpp
    VRPhysics.cpp
    VRAudio.cpp
    VRUI.cpp
)

target_include_directories(vr
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/vr
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/vr
)

target_link_libraries(vr
    PUBLIC
        core
        spdlog::spdlog
        fmt::fmt
        GLEW::GLEW
        OpenGL::GL
)

target_compile_definitions(vr PRIVATE
    VR_DAW_VR_EXPORTS
)

target_compile_features(vr PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(vr PRIVATE /W4)
else()
    target_compile_options(vr PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Installationsregeln
install(TARGETS vr
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/vrmusicstudio/vr
    FILES_MATCHING PATTERN "*.hpp"
) 