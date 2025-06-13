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
