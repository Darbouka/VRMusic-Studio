# Deployment-Konfiguration
add_subdirectory(docker)
add_subdirectory(kubernetes)
add_subdirectory(cloud)

# Deployment-Target
add_custom_target(deploy
    COMMAND ${CMAKE_COMMAND} -E echo "Deploying VR Music Studio..."
    DEPENDS vrmusicstudio
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
) 