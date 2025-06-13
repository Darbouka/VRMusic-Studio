# Effekt-Quellen
set(EFFECT_SOURCES
    DistortionEffect.cpp
    ReverseDelayEffect.cpp
    ReverbEffect.cpp
    DelayEffect.cpp
    ChorusEffect.cpp
    FlangerEffect.cpp
    PhaserEffect.cpp
    CompressorEffect.cpp
    LimiterEffect.cpp
    EQEffect.cpp
)

# Effekt-Bibliothek
add_library(VRMusicStudioEffects STATIC ${EFFECT_SOURCES})

# Include-Verzeichnisse
target_include_directories(VRMusicStudioEffects PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Verlinke Bibliotheken
target_link_libraries(VRMusicStudioEffects PUBLIC
    VRMusicStudioCore
)

# Kompilierungsoptionen
target_compile_features(VRMusicStudioEffects PRIVATE cxx_std_17)

if(APPLE)
    target_compile_options(VRMusicStudioEffects PRIVATE
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
install(TARGETS VRMusicStudioEffects
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
) 