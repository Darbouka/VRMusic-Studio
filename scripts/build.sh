#!/bin/bash

# Fehlerbehandlung
set -e
set -o pipefail

# Farben für die Ausgabe
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Hilfsfunktionen
print_step() {
    echo -e "${YELLOW}==>${NC} $1"
}

print_success() {
    echo -e "${GREEN}==>${NC} $1"
}

print_error() {
    echo -e "${RED}==>${NC} $1"
}

# Überprüfe Voraussetzungen
check_requirements() {
    print_step "Überprüfe Voraussetzungen..."
    
    # CMake
    if ! command -v cmake &> /dev/null; then
        print_error "CMake nicht gefunden. Bitte installieren Sie CMake."
        exit 1
    fi
    
    # Ninja
    if ! command -v ninja &> /dev/null; then
        print_error "Ninja nicht gefunden. Bitte installieren Sie Ninja."
        exit 1
    fi
    
    # vcpkg
    if [ ! -d "vcpkg" ]; then
        print_error "vcpkg nicht gefunden. Bitte klonen Sie das vcpkg Repository."
        exit 1
    fi
}

# Konfiguriere CMake
configure_cmake() {
    print_step "Konfiguriere CMake..."
    
    cmake -B build \
        -S . \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=15.5 \
        -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
}

# Baue das Projekt
build_project() {
    print_step "Baue das Projekt..."
    
    cmake --build build --config Debug
}

# Führe Tests aus
run_tests() {
    print_step "Führe Tests aus..."
    
    cd build && ctest --output-on-failure
}

# Hauptfunktion
main() {
    print_step "Starte Build-Prozess..."
    
    check_requirements
    configure_cmake
    build_project
    run_tests
    
    print_success "Build erfolgreich abgeschlossen!"
}

# Starte Hauptfunktion
main 