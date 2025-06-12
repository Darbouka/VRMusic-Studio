# Docker-Konfiguration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Dockerfile.in
    ${CMAKE_BINARY_DIR}/Dockerfile
    @ONLY
)

# Docker-Build-Target
add_custom_target(docker_build
    COMMAND docker build -t vrmusicstudio:${PROJECT_VERSION} ${CMAKE_BINARY_DIR}
    DEPENDS vrmusicstudio
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Docker-Push-Target
add_custom_target(docker_push
    COMMAND docker push vrmusicstudio:${PROJECT_VERSION}
    DEPENDS docker_build
) 