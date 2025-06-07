add_library(VR-DAW-Shaders
    UIShader.cpp
)

target_include_directories(VR-DAW-Shaders
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(VR-DAW-Shaders
    PUBLIC
        VR-DAW-Core
    PRIVATE
        glad
        glm
)

target_compile_definitions(VR-DAW-Shaders
    PRIVATE
        VR_DAW_SHADERS_EXPORTS
)

# Shader-Dateien installieren
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/shaders
        FILES_MATCHING PATTERN "*.vert" PATTERN "*.frag") 