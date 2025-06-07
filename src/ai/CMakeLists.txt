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
)

# KI-Module
add_subdirectory(voice)
add_subdirectory(chatbot)
add_subdirectory(vocoder) 