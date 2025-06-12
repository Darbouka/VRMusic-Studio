<<<<<<< HEAD
set(UTILS_SOURCES
    MathUtils.cpp
    StringUtils.cpp
    FileUtils.cpp
    TimeUtils.cpp
    DebugUtils.cpp
)

set(UTILS_HEADERS
    MathUtils.hpp
    StringUtils.hpp
    FileUtils.hpp
    TimeUtils.hpp
    DebugUtils.hpp
)

add_library(VR-DAW-Utils STATIC ${UTILS_SOURCES} ${UTILS_HEADERS})

target_include_directories(VR-DAW-Utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(VR-DAW-Utils
    PUBLIC
        VR-DAW-Core
        glm::glm
)

target_compile_features(VR-DAW-Utils PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(VR-DAW-Utils PRIVATE /W4)
else()
    target_compile_options(VR-DAW-Utils PRIVATE -Wall -Wextra -Wpedantic)
endif() 
=======
# Utils-Bibliothek
add_library(VRMusicStudio::Utils STATIC
    Utils.cpp
)

target_include_directories(VRMusicStudio::Utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(VRMusicStudio::Utils
    PUBLIC
        spdlog::spdlog
        nlohmann_json::nlohmann_json
    PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
)

# Installationsregeln
install(TARGETS VRMusicStudio::Utils
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/VRMusicStudio/utils
    FILES_MATCHING PATTERN "*.hpp"
) 
>>>>>>> 0dff1c4 (init 2)
