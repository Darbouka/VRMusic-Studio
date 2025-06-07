add_library(VR-DAW-UI
    VRUI.hpp
    VRUI.cpp
)

target_include_directories(VR-DAW-UI
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(VR-DAW-UI
    PUBLIC
        VR-DAW-Core
        VR-DAW-VR
        OpenGL::GL
        GLEW::GLEW
        glm
)

target_compile_definitions(VR-DAW-UI
    PRIVATE
        VR_DAW_UI_EXPORTS
)

target_compile_features(VR-DAW-UI PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(VR-DAW-UI PRIVATE /W4)
else()
    target_compile_options(VR-DAW-UI PRIVATE -Wall -Wextra -Wpedantic)
endif() 