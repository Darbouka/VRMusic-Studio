#!/bin/bash

# Farben für die Ausgabe
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Funktion für Logging
log() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Prüfe Git
if ! command -v git &> /dev/null; then
    error "Git ist nicht installiert"
    exit 1
fi

# Erstelle externes Verzeichnis
mkdir -p external

# Klone ImGui
log "Klone ImGui..."
cd external
if [ ! -d "imgui" ]; then
    git clone https://github.com/ocornut/imgui.git
    if [ $? -ne 0 ]; then
        error "ImGui konnte nicht geklont werden"
        exit 1
    fi
fi

# Erstelle ImGui CMake-Konfiguration
log "Erstelle ImGui CMake-Konfiguration..."
cat > imgui/CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.15)
project(imgui)

set(IMGUI_SOURCES
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    backends/imgui_impl_sdl2.cpp
    backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/backends
)

target_link_libraries(imgui
    PUBLIC
        SDL2::SDL2
        OpenGL::GL
)

install(TARGETS imgui
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
EOF

log "ImGui erfolgreich installiert"
cd .. 