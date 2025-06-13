set(COMPATIBILITY_SOURCES
    CompatibilityEngine.cpp
    CompatibilityManager.cpp
    CompatibilityProcessor.cpp
)

add_library(VRMusicStudioCompatibility STATIC ${COMPATIBILITY_SOURCES})

target_include_directories(VRMusicStudioCompatibility
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(VRMusicStudioCompatibility
    PUBLIC
        VRMusicStudio::Core
        VRMusicStudio::Audio
        VRMusicStudio::AI
    PRIVATE
        spdlog::spdlog
)

target_compile_features(VRMusicStudioCompatibility PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(VRMusicStudioCompatibility PRIVATE /W4)
else()
    target_compile_options(VRMusicStudioCompatibility PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS VRMusicStudioCompatibility
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
) 