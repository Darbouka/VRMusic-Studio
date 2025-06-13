set(KI_SOURCES
    KIEngine.cpp
    KIModel.cpp
    KIProcessor.cpp
)

add_library(VRMusicStudioKI STATIC ${KI_SOURCES})

target_include_directories(VRMusicStudioKI
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(VRMusicStudioKI
    PUBLIC
        VRMusicStudio::Core
        VRMusicStudio::Audio
        VRMusicStudio::AI
    PRIVATE
        spdlog::spdlog
)

target_compile_features(VRMusicStudioKI PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(VRMusicStudioKI PRIVATE /W4)
else()
    target_compile_options(VRMusicStudioKI PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS VRMusicStudioKI
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
) 