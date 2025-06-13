cmake_minimum_required(VERSION 3.15)

# Find required packages
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(websocketpp REQUIRED)
find_package(spdlog REQUIRED)

# Network-Bibliothek
add_library(network STATIC
    NetworkEngine.cpp
)

target_include_directories(network
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(network
    PUBLIC
        core
        OpenSSL::SSL
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
        websocketpp::websocketpp
    PRIVATE
        spdlog::spdlog
)

target_compile_features(network PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(network PRIVATE /W4)
else()
    target_compile_options(network PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Installationsregeln
install(TARGETS network
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/vrmusicstudio/network
    FILES_MATCHING PATTERN "*.hpp"
) 