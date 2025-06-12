<<<<<<< HEAD
find_package(GTest REQUIRED)

set(TEST_SOURCES
    core/ApplicationTest.cpp
    core/LoggerTest.cpp
    core/ConfigTest.cpp
    core/EventSystemTest.cpp
    core/PluginManagerTest.cpp
    core/ResourceManagerTest.cpp
    core/AudioPluginTest.cpp
    audio/AudioEngineTest.cpp
    audio/AudioBufferTest.cpp
    audio/AudioProcessorTest.cpp
    audio/AudioStreamTest.cpp
    audio/AudioDeviceTest.cpp
    vr/VRSceneTest.cpp
    vr/VRControllerTest.cpp
    vr/VRDeviceTest.cpp
    vr/VRRendererTest.cpp
    vr/VRInputTest.cpp
    ui/UIElementTest.cpp
    ui/UIRendererTest.cpp
    ui/UIManagerTest.cpp
    ui/TextRendererTest.cpp
    ui/FontManagerTest.cpp
    utils/MathUtilsTest.cpp
    utils/StringUtilsTest.cpp
    utils/FileUtilsTest.cpp
    utils/TimeUtilsTest.cpp
    utils/DebugUtilsTest.cpp
)

add_executable(VR-DAW-Tests ${TEST_SOURCES})

target_include_directories(VR-DAW-Tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(VR-DAW-Tests
    PRIVATE
        VR-DAW-Core
        VR-DAW-Audio
        VR-DAW-VR
        VR-DAW-UI
        VR-DAW-Utils
        GTest::gtest
        GTest::gtest_main
)

target_compile_features(VR-DAW-Tests PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(VR-DAW-Tests PRIVATE /W4)
else()
    target_compile_options(VR-DAW-Tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_test(NAME VR-DAW-Tests COMMAND VR-DAW-Tests) 
=======
# Test-Konfiguration
enable_testing()

# Google Test
find_package(GTest REQUIRED)

# Unit Tests
add_subdirectory(unit)

# Integration Tests
add_subdirectory(integration)

# Test-Ausführung
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS unit_tests integration_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
) 
>>>>>>> 0dff1c4 (init 2)
