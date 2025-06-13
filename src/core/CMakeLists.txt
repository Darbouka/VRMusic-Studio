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
set(SOURCES
    Application.cpp
    EventSystem.cpp
    Logger.cpp
    ResourceManager.cpp
    PluginManager.cpp
    ProjectManager.cpp
    UIManager.cpp
    VRController.cpp
    VRManager.cpp
    ConfigManager.cpp
    PerformanceMonitor.cpp
    ErrorHandler.cpp
    AudioEngine.cpp
    Version.cpp
    BackendService.cpp
    SystemEngine.cpp
    LoggingEngine.cpp
    EventEngine.cpp
    Config.cpp
    VRMusicStudioApp.cpp
)

# Define header files
set(HEADERS
    Logger.hpp
    EventSystem.hpp
    Application.hpp
    ResourceManager.hpp
    PluginManager.hpp
    ProjectManager.hpp
    UIManager.hpp
    VRController.hpp
    VRManager.hpp
    ConfigManager.hpp
    PerformanceMonitor.hpp
    ErrorHandler.hpp
    AudioEngine.hpp
    Version.hpp
)

# Create library
add_library(core STATIC ${SOURCES} ${HEADERS})

# Set include directories
target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENAL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${spdlog_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/core
)

# Link libraries
target_link_libraries(core PUBLIC
    OpenAL::OpenAL
    GLEW::GLEW
    OpenGL::GL
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    fmt::fmt
)

# Add compile definitions
target_compile_definitions(core PRIVATE
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG
)

# Specify compile features
target_compile_features(core PRIVATE cxx_std_17)

# Installationsregeln
install(TARGETS core
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
