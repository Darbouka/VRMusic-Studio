set(VOCODER_SOURCES
    VocoderEngine.cpp
    VocoderModel.cpp
    VocoderProcessor.cpp
)

add_library(VRMusicStudioVocoder STATIC ${VOCODER_SOURCES})

target_include_directories(VRMusicStudioVocoder
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../..
)

target_link_libraries(VRMusicStudioVocoder
    PUBLIC
        VRMusicStudio::Core
        VRMusicStudio::Audio
        VRMusicStudio::AI
    PRIVATE
        spdlog::spdlog
)

target_compile_features(VRMusicStudioVocoder PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(VRMusicStudioVocoder PRIVATE /W4)
else()
    target_compile_options(VRMusicStudioVocoder PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS VRMusicStudioVocoder
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
) 