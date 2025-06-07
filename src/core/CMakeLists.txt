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
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(VR-DAW-Core
    PUBLIC
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        portaudio
        openvr_api
        GLEW::GLEW
        glm::glm
        freetype
)

target_compile_features(VR-DAW-Core PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(VR-DAW-Core PRIVATE /W4)
else()
    target_compile_options(VR-DAW-Core PRIVATE -Wall -Wextra -Wpedantic)
endif() 