<<<<<<< HEAD
=======
# Core-Bibliothek
add_library(VRMusicStudio::Core STATIC
    BackendService.cpp
)

target_include_directories(VRMusicStudio::Core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(VRMusicStudio::Core
    PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
    PRIVATE
        spdlog::spdlog
)

# Installationsregeln
install(TARGETS VRMusicStudio::Core
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/VRMusicStudio/core
    FILES_MATCHING PATTERN "*.hpp"
)

>>>>>>> 0dff1c4 (init 2)
set(CORE_SOURCES
    Application.cpp
    Logger.cpp
    Config.cpp
    EventSystem.cpp
    PluginManager.cpp
    ResourceManager.cpp
    AudioPlugin.cpp
)

set(CORE_HEADERS
    Application.hpp
    Logger.hpp
    Config.hpp
    EventSystem.hpp
    PluginManager.hpp
    ResourceManager.hpp
    AudioPlugin.hpp
)

add_library(VR-DAW-Core STATIC ${CORE_SOURCES} ${CORE_HEADERS})

target_include_directories(VR-DAW-Core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
<<<<<<< HEAD
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
=======
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/stb
        /Users/fatih/openvr/headers
>>>>>>> 0dff1c4 (init 2)
)

target_link_libraries(VR-DAW-Core
    PUBLIC
<<<<<<< HEAD
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        portaudio
        openvr_api
        GLEW::GLEW
        glm::glm
        freetype
=======
        VR-DAW-Common
        spdlog::spdlog
        GLEW::GLEW
        glfw
        glm::glm
        sndfile
    PRIVATE
        stb
        /Users/fatih/openvr/lib/osx32/libopenvr_api.dylib
>>>>>>> 0dff1c4 (init 2)
)

target_compile_features(VR-DAW-Core PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(VR-DAW-Core PRIVATE /W4)
else()
    target_compile_options(VR-DAW-Core PRIVATE -Wall -Wextra -Wpedantic)
<<<<<<< HEAD
endif() 
=======
endif()

# Füge stb_image als Header-only Bibliothek hinzu
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/stb) 
>>>>>>> 0dff1c4 (init 2)
