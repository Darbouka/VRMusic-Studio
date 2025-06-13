cmake_minimum_required(VERSION 3.15)

# Find required packages
find_package(spdlog REQUIRED)
find_package(Threads REQUIRED)

set(AI_SOURCES
    AIEngine.cpp
    AIModel.cpp
    AITraining.cpp
)

add_library(VRMusicStudioAI STATIC ${AI_SOURCES})

target_include_directories(VRMusicStudioAI
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(VRMusicStudioAI
    PUBLIC
        VRMusicStudio::Core
        VRMusicStudio::Audio
    PRIVATE
        spdlog::spdlog
)

target_compile_definitions(VRMusicStudioAI PRIVATE
    VR_DAW_AI_EXPORTS
)

target_compile_features(VRMusicStudioAI PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(VRMusicStudioAI PRIVATE /W4)
else()
    target_compile_options(VRMusicStudioAI PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Installationsregeln
install(TARGETS VRMusicStudioAI
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/vrmusicstudio/ai
    FILES_MATCHING PATTERN "*.hpp"
)

# KI-Module
add_subdirectory(voice)
add_subdirectory(chatbot)
add_subdirectory(vocoder) 