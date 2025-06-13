cmake_minimum_required(VERSION 3.15)

# Find required packages
find_package(spdlog REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(glm REQUIRED)

# Utils-Bibliothek
add_library(utils STATIC
    MathUtils.cpp
    StringUtils.cpp
    FileUtils.cpp
    TimeUtils.cpp
    DebugUtils.cpp
    Utils.cpp
)

target_include_directories(utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(utils
    PUBLIC
        core
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        glm::glm
    PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
)

target_compile_features(utils PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(utils PRIVATE /W4)
else()
    target_compile_options(utils PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Installationsregeln
install(TARGETS utils
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/vrmusicstudio/utils
    FILES_MATCHING PATTERN "*.hpp"
)
