# Cloud-Deployment-Konfiguration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/terraform/main.tf.in
    ${CMAKE_BINARY_DIR}/terraform/main.tf
    @ONLY
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/terraform/variables.tf.in
    ${CMAKE_BINARY_DIR}/terraform/variables.tf
    @ONLY
)

# Terraform-Init-Target
add_custom_target(terraform_init
    COMMAND terraform init
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/terraform
)

# Terraform-Apply-Target
add_custom_target(terraform_apply
    COMMAND terraform apply -auto-approve
    DEPENDS terraform_init
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/terraform
) 