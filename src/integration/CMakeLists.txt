set(INTEGRATION_SOURCES
    IntegrationEngine.cpp
    IntegrationManager.cpp
    IntegrationProcessor.cpp
)

add_library(VRMusicStudioIntegration STATIC ${INTEGRATION_SOURCES})

target_include_directories(VRMusicStudioIntegration
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(VRMusicStudioIntegration
    PUBLIC
        VRMusicStudio::Core
        VRMusicStudio::Audio
        VRMusicStudio::AI
    PRIVATE
        spdlog::spdlog
)

target_compile_features(VRMusicStudioIntegration PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(VRMusicStudioIntegration PRIVATE /W4)
else()
    target_compile_options(VRMusicStudioIntegration PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS VRMusicStudioIntegration
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
) 