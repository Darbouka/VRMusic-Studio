cmake_minimum_required(VERSION 3.15)
project(VRMusicStudio VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(OpenVR REQUIRED)
find_package(PortAudio REQUIRED)
find_package(OpenAL REQUIRED)
find_package(FFTW3 REQUIRED)
find_package(nlohmann_json REQUIRED)

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OPENGL_INCLUDE_DIR}
    ${OPENVR_INCLUDE_DIR}
    ${PORTAUDIO_INCLUDE_DIR}
    ${OPENAL_INCLUDE_DIR}
    ${FFTW3_INCLUDE_DIR}
    ${NLOHMANN_JSON_INCLUDE_DIR}
)

# Add subdirectories
add_subdirectory(src)

# Create executable
add_executable(VRMusicStudio src/main.cpp)

# Link libraries
target_link_libraries(VRMusicStudio
    PRIVATE
    VRMusicStudio::Audio
    VRMusicStudio::Core
    VRMusicStudio::UI
    VRMusicStudio::VR
    VRMusicStudio::AI
    VRMusicStudio::Utils
    VRMusicStudio::Plugins
    VRMusicStudio::Sequencer
    VRMusicStudio::MIDI
    VRMusicStudio::Network
    VRMusicStudio::Layout
    VRMusicStudio::Integration
    VRMusicStudio::Compatibility
    VRMusicStudio::Design
    VRMusicStudio::KI
    VRMusicStudio::Clock
    VRMusicStudio::Automation
    VRMusicStudio::DAW
    VRMusicStudio::JSON
    ${OPENGL_LIBRARIES}
    ${OPENVR_LIBRARIES}
    ${PORTAUDIO_LIBRARIES}
    ${OPENAL_LIBRARIES}
    ${FFTW3_LIBRARIES}
    ${NLOHMANN_JSON_LIBRARIES}
)

# Install targets
install(TARGETS VRMusicStudio
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install headers
install(DIRECTORY include/
    DESTINATION include/VRMusicStudio
    FILES_MATCHING PATTERN "*.hpp"
)

# Setze vcpkg Toolchain
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

# Finde erforderliche Pakete
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(GLEW REQUIRED)
find_package(GTest REQUIRED)
find_package(JACK REQUIRED)
find_package(Vulkan REQUIRED)
find_package(assimp REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread filesystem)

# Füge Unterverzeichnisse hinzu
add_subdirectory(src/core)
add_subdirectory(src/audio)
add_subdirectory(src/vr)
add_subdirectory(src/ui)
add_subdirectory(src/sequencer)
add_subdirectory(src/midi)
add_subdirectory(src/network)
add_subdirectory(src/plugins)
add_subdirectory(src/automation)
add_subdirectory(src/ai)
add_subdirectory(src/ki)
add_subdirectory(src/integration)
add_subdirectory(src/compatibility)

# Aktiviere Tests
enable_testing()
add_subdirectory(tests)

# WebRTC Konfiguration
set(WEBRTC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/webrtc")
set(WEBRTC_INCLUDE_DIRS
    "${WEBRTC_ROOT}"
    "${WEBRTC_ROOT}/api"
    "${WEBRTC_ROOT}/rtc_base"
    "${WEBRTC_ROOT}/system_wrappers/include"
)
include_directories(${WEBRTC_INCLUDE_DIRS})
set(WEBRTC_LIBRARIES
    "${WEBRTC_ROOT}/out/Debug/libwebrtc.a"
)
target_link_libraries(VRMusicStudio PRIVATE
    ${WEBRTC_LIBRARIES}
)

# CPack Konfiguration
include(CPack)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "TGZ")
set(CPACK_SOURCE_GENERATOR "TGZ")

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