# Verarbeitungs-Quellen
set(PROCESSING_SOURCES
    EffectEngine.cpp
    AudioProcessor.cpp
    BufferProcessor.cpp
    StreamProcessor.cpp
    DeviceProcessor.cpp
)

# Verarbeitungs-Bibliothek
add_library(VRMusicStudioProcessing STATIC ${PROCESSING_SOURCES})

# Include-Verzeichnisse
target_include_directories(VRMusicStudioProcessing PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Verlinke Bibliotheken
target_link_libraries(VRMusicStudioProcessing PUBLIC
    VRMusicStudioCore
    VRMusicStudioEffects
)

# Kompilierungsoptionen
target_compile_features(VRMusicStudioProcessing PRIVATE cxx_std_17)

if(APPLE)
    target_compile_options(VRMusicStudioProcessing PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -std=c++17
        -arch arm64
        -arch x86_64
        -mmacosx-version-min=15.0
    )
endif()

# Installationsregeln
install(TARGETS VRMusicStudioProcessing
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
) 