# Unit Tests
add_executable(unit_tests
    core_tests.cpp
    audio_tests.cpp
    vr_tests.cpp
    ui_tests.cpp
    ai_tests.cpp
    network_tests.cpp
    plugins_tests.cpp
    utils_tests.cpp
)

target_include_directories(unit_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(unit_tests
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

add_test(NAME unit_tests COMMAND unit_tests) 