# Processing sources
set(PROCESSING_SOURCES
    AudioProcessor.cpp
    EffectEngine.cpp
    MasteringEngine.cpp
    MixingEngine.cpp
)

# Processing library
add_library(VRMusicStudioProcessing STATIC ${PROCESSING_SOURCES})

target_include_directories(VRMusicStudioProcessing
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(VRMusicStudioProcessing
    PUBLIC
    VRMusicStudio::Core
    VRMusicStudio::Audio
    VRMusicStudio::Effects
    ${FFTW3_LIBRARIES}
    ${OPENAL_LIBRARIES}
)

install(TARGETS VRMusicStudioProcessing
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
) 