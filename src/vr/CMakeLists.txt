set(VR_SOURCES
    VRManager.cpp
    VRController.cpp
    VRDisplay.cpp
)

set(VR_HEADERS
    VRManager.hpp
    VRController.hpp
    VRDisplay.hpp
)

add_library(VR-DAW-VR-Lib STATIC ${VR_SOURCES} ${VR_HEADERS})

target_include_directories(VR-DAW-VR-Lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# OpenVR manuell suchen
set(OPENVR_ROOT $ENV{HOME}/openvr)
find_path(OPENVR_INCLUDE_DIR openvr.h PATHS ${OPENVR_ROOT}/headers)
find_library(OPENVR_LIBRARY openvr_api PATHS ${OPENVR_ROOT}/lib/osx32)
if(NOT OPENVR_INCLUDE_DIR OR NOT OPENVR_LIBRARY)
    message(FATAL_ERROR "OpenVR nicht gefunden!")
endif()
include_directories(${OPENVR_INCLUDE_DIR})
set(OPENVR_LIBRARIES ${OPENVR_LIBRARY})

target_link_libraries(VR-DAW-VR-Lib
    PUBLIC
        VR-DAW-Core
        ${OPENVR_LIBRARIES}
        GLEW::GLEW
        glm::glm
)

target_compile_features(VR-DAW-VR-Lib PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(VR-DAW-VR-Lib PRIVATE /W4)
else()
    target_compile_options(VR-DAW-VR-Lib PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_library(VR-DAW-VR
    VRManager.hpp
    VRManager.cpp
)

target_include_directories(VR-DAW-VR
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(VR-DAW-VR
    PUBLIC
        VR-DAW-Core
        ${OPENVR_LIBRARIES}
        glm
)

target_compile_definitions(VR-DAW-VR
    PRIVATE
        VR_DAW_VR_EXPORTS
) 