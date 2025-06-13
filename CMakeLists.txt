cmake_minimum_required(VERSION 3.20)
project(VRMusicStudio VERSION 1.0.0 LANGUAGES CXX)

# Windows-spezifische Einstellungen
if(WIN32)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_WIN32_EXECUTABLE TRUE)
    
    # Windows SDK Version
    set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)
    
    # Compiler-Flags
    add_compile_options(/W4 /WX /MP)
    
    # Windows-spezifische Bibliotheken
    set(WINDOWS_LIBS
        d3d11
        dxgi
        d3dcompiler
        xinput
        winmm
    )
endif()

# vcpkg Integration
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

# vcpkg Pfade
set(CMAKE_PREFIX_PATH 
    ${CMAKE_PREFIX_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/${CMAKE_SYSTEM_NAME}/lib/cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/${CMAKE_SYSTEM_NAME}/share"
)

# PortAudio Konfiguration
if(WIN32)
    set(PORTAUDIO_INCLUDE_DIR "$ENV{VCPKG_ROOT}/installed/x64-windows/include")
    set(PORTAUDIO_LIBRARY "$ENV{VCPKG_ROOT}/installed/x64-windows/lib/portaudio.lib")
else()
    set(PORTAUDIO_INCLUDE_DIR "/usr/local/include")
    set(PORTAUDIO_LIBRARY "/usr/local/lib/libportaudio.dylib")
endif()

if(NOT EXISTS ${PORTAUDIO_LIBRARY})
    message(FATAL_ERROR "PortAudio library not found at ${PORTAUDIO_LIBRARY}")
endif()

if(NOT EXISTS ${PORTAUDIO_INCLUDE_DIR}/portaudio.h)
    message(FATAL_ERROR "PortAudio headers not found at ${PORTAUDIO_INCLUDE_DIR}")
endif()

# Finde Pakete
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(PortAudio REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(Vulkan REQUIRED)

# JUCE Integration
add_subdirectory(external/JUCE)
juce_add_modules(${CMAKE_CURRENT_SOURCE_DIR}/external/JUCE/modules
    juce_core
    juce_audio_basics
    juce_audio_devices
    juce_audio_formats
    juce_audio_processors
    juce_audio_utils
    juce_gui_basics
    juce_gui_extra
)

# Quell-Dateien
set(SOURCES
    src/main_win.cpp
    src/audio/AudioEngine.cpp
    src/audio/AudioBuffer.cpp
    src/audio/AudioStream.cpp
    src/midi/MIDIEngine.cpp
    src/vr/VRManager.cpp
    src/vr/VRInteraction.cpp
    src/ui/UIComponents.cpp
    src/ui/VRUI.cpp
    src/sequencer/Sequencer.cpp
    src/plugins/PluginManager.cpp
)

# Header-Dateien
set(HEADERS
    src/ui/UIComponents.hpp
)

# Executable erstellen
add_executable(VRMusicStudio WIN32 ${SOURCES} ${HEADERS})

# Include-Verzeichnisse
target_include_directories(VRMusicStudio PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${PORTAUDIO_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${SPDLOG_INCLUDE_DIRS}
    ${NLOHMANN_JSON_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIRS}
)

# Bibliotheken verlinken
target_link_libraries(VRMusicStudio PRIVATE
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    OpenGL::GL
    glfw
    GLEW::GLEW
    glm::glm
    PortAudio::PortAudio
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    Boost::system
    Boost::filesystem
    Vulkan::Vulkan
    ${PORTAUDIO_LIBRARY}
)

if(WIN32)
    target_link_libraries(VRMusicStudio PRIVATE ${WINDOWS_LIBS})
endif()

# Ressourcen-Datei hinzufügen
if(WIN32)
    target_sources(VRMusicStudio PRIVATE resources/app.rc)
endif()

# Installation
install(TARGETS VRMusicStudio
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Tests
enable_testing()
add_subdirectory(tests)

# Dokumentation
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