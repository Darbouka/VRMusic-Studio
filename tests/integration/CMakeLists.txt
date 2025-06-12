# Integration Tests
add_executable(integration_tests
    system_tests.cpp
    performance_tests.cpp
    stress_tests.cpp
)

target_include_directories(integration_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(integration_tests
    PRIVATE
        GTest::GTest
        GTest::Main
        VRMusicStudio::Core
        VRMusicStudio::Audio
        VRMusicStudio::VR
        VRMusicStudio::UI
        VRMusicStudio::AI
        VRMusicStudio::Network
        VRMusicStudio::Plugins
        VRMusicStudio::Utils
)

add_test(NAME integration_tests COMMAND integration_tests) 