#!/bin/bash

# Farben für die Ausgabe
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Funktion zum Überprüfen von Fehlern
check_error() {
    if [ $? -ne 0 ]; then
        echo -e "${RED}Fehler: $1${NC}"
        exit 1
    fi
}

echo "Installiere vcpkg..."
if [ ! -d "vcpkg" ]; then
    git clone https://github.com/Microsoft/vcpkg.git
    check_error "vcpkg konnte nicht geklont werden"
    
    cd vcpkg
    ./bootstrap-vcpkg.sh
    check_error "vcpkg konnte nicht gebootstrappt werden"
    cd ..
fi

# Setze VCPKG_ROOT
export VCPKG_ROOT="$(pwd)/vcpkg"

echo "Installiere Abhängigkeiten..."
if [ -f vcpkg.json ]; then
    $VCPKG_ROOT/vcpkg install
    check_error "Abhängigkeiten aus vcpkg.json konnten nicht installiert werden"
else
    $VCPKG_ROOT/vcpkg install spdlog:x64-osx --recurse
    check_error "spdlog konnte nicht installiert werden"
    $VCPKG_ROOT/vcpkg install fmt:x64-osx --recurse
    check_error "fmt konnte nicht installiert werden"
    $VCPKG_ROOT/vcpkg install glew:x64-osx --recurse
    check_error "glew konnte nicht installiert werden"
    $VCPKG_ROOT/vcpkg install glfw3:x64-osx --recurse
    check_error "glfw3 konnte nicht installiert werden"
    $VCPKG_ROOT/vcpkg install glm:x64-osx --recurse
    check_error "glm konnte nicht installiert werden"
fi

echo -e "${GREEN}Alle Abhängigkeiten wurden erfolgreich installiert!${NC}" 