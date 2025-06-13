set(VOICE_SOURCES
    VoiceEngine.cpp
    VoiceModel.cpp
    VoiceProcessor.cpp
)

add_library(VRMusicStudioVoice STATIC ${VOICE_SOURCES})

target_include_directories(VRMusicStudioVoice
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../..
)

target_link_libraries(VRMusicStudioVoice
    PUBLIC
        VRMusicStudio::Core
        VRMusicStudio::Audio
        VRMusicStudio::AI
    PRIVATE
        spdlog::spdlog
)

target_compile_features(VRMusicStudioVoice PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(VRMusicStudioVoice PRIVATE /W4)
else()
    target_compile_options(VRMusicStudioVoice PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS VRMusicStudioVoice
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
) 