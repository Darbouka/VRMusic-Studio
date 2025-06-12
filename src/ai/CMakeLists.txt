<<<<<<< HEAD
add_library(VR-DAW-AI
    AIIntegration.cpp
)

target_include_directories(VR-DAW-AI
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(VR-DAW-AI
    PUBLIC
        VR-DAW-Core
        VR-DAW-Audio
    PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
)

target_compile_definitions(VR-DAW-AI
    PRIVATE
        VR_DAW_AI_EXPORTS
=======
# AI-Bibliothek
add_library(VRMusicStudio::AI STATIC
    AIEngine.cpp
)

target_include_directories(VRMusicStudio::AI
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(VRMusicStudio::AI
    PUBLIC
        TensorFlow::TensorFlow
        PyTorch::PyTorch
    PRIVATE
        spdlog::spdlog
)

# Installationsregeln
install(TARGETS VRMusicStudio::AI
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/VRMusicStudio/ai
    FILES_MATCHING PATTERN "*.hpp"
>>>>>>> 0dff1c4 (init 2)
)

# KI-Module
add_subdirectory(voice)
add_subdirectory(chatbot)
add_subdirectory(vocoder) 