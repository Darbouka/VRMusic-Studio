cmake_minimum_required(VERSION 3.15)
project(VR-DAW-Core)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find required packages
find_package(OpenAL REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Define source files
set(CORE_SOURCES
    Logger.cpp
    Config.cpp
    ErrorHandler.cpp
)

# Define header files
set(CORE_HEADERS
    ${CMAKE_SOURCE_DIR}/include/core/Logger.hpp
    ${CMAKE_SOURCE_DIR}/include/core/Config.hpp
    ${CMAKE_SOURCE_DIR}/include/core/ErrorHandler.hpp
)

# Create library
add_library(VRMusicStudioCore STATIC
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)

# Set include directories
target_include_directories(VRMusicStudioCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(VRMusicStudioCore PUBLIC
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

# Add compile definitions
target_compile_definitions(VRMusicStudioCore PRIVATE
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG
)

# Specify compile features
target_compile_features(VRMusicStudioCore PUBLIC cxx_std_17)

if(APPLE)
    target_compile_options(VRMusicStudioCore PRIVATE
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

# Installationsregeln
install(TARGETS VRMusicStudioCore
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/vrmusicstudio/core
    FILES_MATCHING PATTERN "*.hpp"
)

# Füge stb_image als Header-only Bibliothek hinzu
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/stb)
