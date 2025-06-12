<<<<<<< HEAD
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
=======
# UI-Bibliothek
add_library(VRMusicStudio::UI STATIC
    UIManager.cpp
)

target_include_directories(VRMusicStudio::UI
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(VRMusicStudio::UI
    PUBLIC
        glfw
        glm::glm
        ImGui::ImGui
    PRIVATE
        spdlog::spdlog
)

# Installationsregeln
install(TARGETS VRMusicStudio::UI
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/VRMusicStudio/ui
    FILES_MATCHING PATTERN "*.hpp"
) 
>>>>>>> 0dff1c4 (init 2)
