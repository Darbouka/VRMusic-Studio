# Project name
project(VRMusicStudio)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${OPENGL_INCLUDE_DIR}
    ${OPENVR_INCLUDE_DIR}
    ${PORTAUDIO_INCLUDE_DIR}
    ${OPENAL_INCLUDE_DIR}
    ${FFTW3_INCLUDE_DIR}
    ${NLOHMANN_JSON_INCLUDE_DIR}
)

# Add source files
set(SOURCE_FILES
    main.cpp
)

# Add subdirectories
add_subdirectory(audio)
add_subdirectory(core)
add_subdirectory(ui)
add_subdirectory(vr)
add_subdirectory(ai)
add_subdirectory(utils)
add_subdirectory(plugins)
add_subdirectory(sequencer)
add_subdirectory(midi)
add_subdirectory(network)
add_subdirectory(layout)
add_subdirectory(integration)
add_subdirectory(compatibility)
add_subdirectory(design)
add_subdirectory(ki)
add_subdirectory(clock)
add_subdirectory(automation)
add_subdirectory(daw)
add_subdirectory(json)

# Create executable
add_executable(VRMusicStudio ${SOURCE_FILES})

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
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/
    DESTINATION include/VRMusicStudio
    FILES_MATCHING PATTERN "*.hpp"
)
