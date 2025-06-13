cmake_minimum_required(VERSION 3.15)

# Find required packages
find_package(spdlog REQUIRED)
find_package(Threads REQUIRED)

# AI-Bibliothek
add_library(ai STATIC
    AIEngine.cpp
    AIIntegration.cpp
)

target_include_directories(ai
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(ai
    PUBLIC
        core
        audio
    PRIVATE
        spdlog::spdlog
        ${CMAKE_THREAD_LIBS_INIT}
)

target_compile_definitions(ai PRIVATE
    VR_DAW_AI_EXPORTS
)

target_compile_features(ai PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(ai PRIVATE /W4)
else()
    target_compile_options(ai PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Installationsregeln
install(TARGETS ai
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