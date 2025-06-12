# Kubernetes-Konfiguration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/deployment.yaml.in
    ${CMAKE_BINARY_DIR}/deployment.yaml
    @ONLY
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/service.yaml.in
    ${CMAKE_BINARY_DIR}/service.yaml
    @ONLY
)

# Kubernetes-Deploy-Target
add_custom_target(k8s_deploy
    COMMAND kubectl apply -f ${CMAKE_BINARY_DIR}/deployment.yaml
    COMMAND kubectl apply -f ${CMAKE_BINARY_DIR}/service.yaml
    DEPENDS docker_push
) 