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