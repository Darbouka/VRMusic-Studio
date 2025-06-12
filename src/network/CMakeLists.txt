# Network-Bibliothek
add_library(VRMusicStudio::Network STATIC
    NetworkEngine.cpp
)

target_include_directories(VRMusicStudio::Network
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(VRMusicStudio::Network
    PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
        websocketpp::websocketpp
    PRIVATE
        spdlog::spdlog
)

# Installationsregeln
install(TARGETS VRMusicStudio::Network
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/VRMusicStudio/network
    FILES_MATCHING PATTERN "*.hpp"
) 