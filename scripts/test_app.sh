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

# Prüfe Build-Verzeichnis
if [ ! -d "build" ]; then
    error "Build-Verzeichnis nicht gefunden. Bitte zuerst bauen: ./scripts/build.sh"
    exit 1
fi

# Prüfe SteamVR-Installation
if [[ "$OSTYPE" == "darwin"* ]]; then
    STEAMVR_PATHS=(
        "/Applications/SteamVR.app/Contents/MacOS"
        "$HOME/Library/Application Support/Steam/steamapps/common/SteamVR"
    )
else
    STEAMVR_PATHS=(
        "/usr/local/SteamVR"
        "/opt/SteamVR"
    )
fi

STEAMVR_FOUND=false
for path in "${STEAMVR_PATHS[@]}"; do
    if [ -d "$path" ]; then
        STEAMVR_FOUND=true
        log "SteamVR gefunden in: $path"
        break
    fi
done

if [ "$STEAMVR_FOUND" = false ]; then
    warning "SteamVR nicht gefunden. VR-Funktionalität wird möglicherweise nicht verfügbar sein."
fi

# Starte die Anwendung
log "Starte VRMusic Studio..."

# Setze Umgebungsvariablen
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:build/lib"
export DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH:build/lib"

# Starte die Anwendung
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS-spezifische Einstellungen
    export DISPLAY=:0
    ./build/bin/VRMusicStudio
else
    # Linux/Windows
    ./build/bin/VRMusicStudio
fi

# Prüfe Exit-Status
if [ $? -eq 0 ]; then
    log "VRMusic Studio wurde erfolgreich beendet."
else
    error "VRMusic Studio wurde mit Fehlern beendet."
    exit 1
fi 