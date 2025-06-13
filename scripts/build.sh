#!/bin/bash

# Farben für die Ausgabe
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Funktion zum Überprüfen des Exit-Codes
check_error() {
    if [ $? -ne 0 ]; then
        echo -e "${RED}Fehler: $1${NC}"
        exit 1
    fi
}

# Build-Verzeichnis erstellen
echo -e "${YELLOW}Erstelle Build-Verzeichnis...${NC}"
mkdir -p build
cd build

# CMake konfigurieren
echo -e "${YELLOW}Konfiguriere CMake...${NC}"
cmake .. -DCMAKE_BUILD_TYPE=Debug
check_error "CMake-Konfiguration fehlgeschlagen"

# Projekt bauen
echo -e "${YELLOW}Baue Projekt...${NC}"
cmake --build . --config Debug
check_error "Build fehlgeschlagen"

# Tests ausführen
echo -e "${YELLOW}Führe Tests aus...${NC}"
ctest --output-on-failure
check_error "Tests fehlgeschlagen"

echo -e "${GREEN}Build erfolgreich abgeschlossen!${NC}" 