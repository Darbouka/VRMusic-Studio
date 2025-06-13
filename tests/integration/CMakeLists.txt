# Integration Tests
# add_executable(integration_tests system_tests.cpp)
# target_link_libraries(integration_tests PRIVATE GTest::gtest_main VRMusicStudio::Core)
# target_include_directories(integration_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

# ... ggf. weitere Zeilen auskommentieren ...

add_test(NAME integration_tests COMMAND integration_tests) 