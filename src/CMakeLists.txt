<<<<<<< HEAD
=======
# Project name.
project(openvr_api)

set( LIBNAME "openvr_api" )
set(OPENVR_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../headers)

# Set some properies for specific files.
if(APPLE)
  set(CMAKE_MACOSX_RPATH 1)
  if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set_source_files_properties(vrcore/pathtools_public.cpp vrcore/vrpathregistry_public.cpp PROPERTIES COMPILE_FLAGS "-x objective-c++")
  endif()
  if(BUILD_SHARED OR BUILD_FRAMEWORK)
    find_library(FOUNDATION_FRAMEWORK Foundation)
    mark_as_advanced(FOUNDATION_FRAMEWORK)
    set(EXTRA_LIBS ${EXTRA_LIBS} ${FOUNDATION_FRAMEWORK})
  endif(BUILD_SHARED OR BUILD_FRAMEWORK)
elseif(WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_definitions( -DWIN64 )
    set( LIBNAME "openvr_api64" )
  endif()
endif()

# Add include folders.
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../headers ${CMAKE_CURRENT_SOURCE_DIR}/vrcore)

if(USE_CUSTOM_LIBCXX)
	link_directories(
		${LIBCXX_LIB_DIR}
	)
endif()

# Set the source group and files.
set(CORE_FILES
	openvr_api_public.cpp
	jsoncpp.cpp
)
set(VRCORE_FILES
	vrcore/dirtools_public.cpp
	vrcore/envvartools_public.cpp
	vrcore/pathtools_public.cpp
	vrcore/sharedlibtools_public.cpp
	vrcore/hmderrors_public.cpp
	vrcore/vrpathregistry_public.cpp
	vrcore/strtools_public.cpp
)

add_compile_definitions(VRCORE_NO_PLATFORM)

set(SOURCE_FILES
	${CORE_FILES}
	${VRCORE_FILES}
)

set(PUBLIC_HEADER_FILES
	${OPENVR_HEADER_DIR}/openvr_driver.h
	${OPENVR_HEADER_DIR}/openvr_capi.h
	${OPENVR_HEADER_DIR}/openvr.h
)

source_group("Src" FILES
	${CORE_FILES}
)

source_group("VRCore" FILES
	${VRCORE_FILES}
)

# Build the library.
if(BUILD_SHARED)
	add_library(${LIBNAME} SHARED ${SOURCE_FILES})
elseif(BUILD_FRAMEWORK)
	set( LIBNAME "OpenVR" )
	add_library( ${LIBNAME} 
            SHARED ${SOURCE_FILES}
            ${CMAKE_SOURCE_DIR}/headers/openvr.h
            ${CMAKE_SOURCE_DIR}/headers/openvr_api.cs
            ${CMAKE_SOURCE_DIR}/headers/openvr_api.json
            ${CMAKE_SOURCE_DIR}/headers/openvr_capi.h
            ${CMAKE_SOURCE_DIR}/headers/openvr_driver.h
	)
	set_target_properties(OpenVR PROPERTIES
		FRAMEWORK TRUE
		FRAMEWORK_VERSION A
		MACOSX_FRAMEWORK_IDENTIFIER com.valvesoftware.OpenVR.framework
		MACOSX_FRAMEWORK_INFO_PLIST ${CMAKE_SOURCE_DIR}/src/Info.plist
		# "current version" in semantic format in Mach-O binary file
		VERSION 1.0.6
		# "compatibility version" in semantic format in Mach-O binary file
		SOVERSION 1.0.0
		PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/headers/openvr.h;${CMAKE_SOURCE_DIR}/headers/openvr_api.cs;${CMAKE_SOURCE_DIR}/headers/openvr_api.json;${CMAKE_SOURCE_DIR}/headers/openvr_capi.h;${CMAKE_SOURCE_DIR}/headers/openvr_driver.h"
		LINKER_LANGUAGE CXX
	)
else()
	add_library(${LIBNAME} STATIC ${SOURCE_FILES})
endif()

if(USE_CUSTOM_LIBCXX)
	set(EXTRA_LIBS ${EXTRA_LIBS} c++ c++abi)
endif()

target_link_libraries(${LIBNAME} ${EXTRA_LIBS} ${CMAKE_DL_LIBS})
target_include_directories(${LIBNAME} PUBLIC ${OPENVR_HEADER_DIR})

install(TARGETS ${LIBNAME} DESTINATION lib)
install(FILES ${PUBLIC_HEADER_FILES} DESTINATION include/openvr)

# Generate a .pc file for linux environments
if(PLATFORM_NAME MATCHES "linux")
    set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_PREFIX}/share/pkgconfig" CACHE PATH "Installation directory for pkgconfig (.pc) files")
    CONFIGURE_FILE("openvr.pc.in" "openvr.pc" @ONLY)

    set(OPENVR_PC ${CMAKE_CURRENT_BINARY_DIR}/openvr.pc)
    if(NOT SKIP_INSTALL_FILES AND NOT SKIP_INSTALL_ALL )
        install(FILES ${OPENVR_PC} DESTINATION "${INSTALL_PKGCONFIG_DIR}")
    endif()
endif()

# Add subdirectories first
add_subdirectory(audio)
add_subdirectory(core)
add_subdirectory(ui)
add_subdirectory(vr)
add_subdirectory(ai)
add_subdirectory(utils)
add_subdirectory(plugins)
add_subdirectory(effects)
add_subdirectory(midi)
add_subdirectory(mastering)
add_subdirectory(mixing)
add_subdirectory(network)

# Collect source files
file(GLOB_RECURSE SOURCES 
    "*.cpp"
    "*.c"
)

# Create executable
add_executable(VRMusicStudio ${SOURCES})

# Set target properties
set_target_properties(VRMusicStudio PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    DEBUG_POSTFIX "d"
)

# Set include directories
target_include_directories(VRMusicStudio
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/headers
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set compile definitions
target_compile_definitions(VRMusicStudio
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        $<$<PLATFORM_ID:Darwin>:MACOSX>
        $<$<PLATFORM_ID:Linux>:LINUX>
        $<$<PLATFORM_ID:Windows>:WINDOWS>
)

# Link libraries
target_link_libraries(VRMusicStudio
    PRIVATE
        audio
        core
        ui
        vr
        ai
        utils
        plugins
        effects
        midi
        mastering
        mixing
        OpenGL::GL
        glfw
        GLEW::GLEW
        glm::glm
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        JUCE::juce_core
        JUCE::juce_data_structures
        JUCE::juce_events
        JUCE::juce_graphics
        JUCE::juce_gui_basics
        JUCE::juce_gui_extra
        JUCE::juce_opengl
        JUCE::juce_audio_basics
        JUCE::juce_audio_devices
        JUCE::juce_audio_formats
        JUCE::juce_audio_plugin_client
        JUCE::juce_audio_processors
        JUCE::juce_audio_utils
        JUCE::juce_cryptography
        JUCE::juce_dsp
        JUCE::juce_recommended_config_flags
        JUCE::juce_recommended_lto_flags
        JUCE::juce_recommended_warning_flags
        PortAudio::PortAudio
        Freetype::Freetype
        VRMusicStudio::Network
    PUBLIC
        $<$<PLATFORM_ID:Darwin>:-framework Cocoa>
        $<$<PLATFORM_ID:Darwin>:-framework IOKit>
        $<$<PLATFORM_ID:Darwin>:-framework CoreVideo>
)

# Install rules
install(TARGETS VRMusicStudio
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/headers/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# Quellverzeichnisse
>>>>>>> 0dff1c4 (init 2)
add_subdirectory(core)
add_subdirectory(audio)
add_subdirectory(vr)
add_subdirectory(ui)
<<<<<<< HEAD
add_subdirectory(utils) 
=======
add_subdirectory(ai)
add_subdirectory(network)
add_subdirectory(plugins)
add_subdirectory(utils)

# Hauptausführbare Datei
add_executable(vrmusicstudio
    main.cpp
)

# Verlinke mit allen Komponenten
target_link_libraries(vrmusicstudio
    PRIVATE
    VRMusicStudio::Core
    VRMusicStudio::Audio
    VRMusicStudio::VR
    VRMusicStudio::UI
    VRMusicStudio::AI
    VRMusicStudio::Network
    VRMusicStudio::Plugins
    VRMusicStudio::Utils
)

# Installationsregeln
install(TARGETS vrmusicstudio
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
>>>>>>> 0dff1c4 (init 2)
