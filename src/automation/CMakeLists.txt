set(AUTOMATION_SOURCES
    AutomationEngine.cpp
    AutomationCurve.cpp
    AutomationPoint.cpp
)

add_library(VRMusicStudioAutomation STATIC ${AUTOMATION_SOURCES})

target_include_directories(VRMusicStudioAutomation
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(VRMusicStudioAutomation
    PUBLIC
        VRMusicStudio::Core
        VRMusicStudio::Audio
    PRIVATE
        spdlog::spdlog
)

target_compile_features(VRMusicStudioAutomation PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(VRMusicStudioAutomation PRIVATE /W4)
else()
    target_compile_options(VRMusicStudioAutomation PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS VRMusicStudioAutomation
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
) 