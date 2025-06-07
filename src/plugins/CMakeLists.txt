add_library(VR-DAW-Plugins
    PluginInterface.hpp
    PluginLoader.cpp
)

target_include_directories(VR-DAW-Plugins
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(VR-DAW-Plugins
    PUBLIC
        VR-DAW-Core
        VR-DAW-Audio
        VR-DAW-UI
)

target_compile_definitions(VR-DAW-Plugins
    PRIVATE
        VR_DAW_PLUGINS_EXPORTS
)

# Plugin-Beispiele
add_subdirectory(examples) 