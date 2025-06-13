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

# Setze Desktop-Modus Umgebungsvariablen
export VRMUSIC_MODE="desktop"
export VRMUSIC_DISABLE_VR="1"
export VRMUSIC_UI_SCALE="1.0"

# Setze Bibliothekspfade
if [[ "$OSTYPE" == "darwin"* ]]; then
    export DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH:build/lib"
else
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:build/lib"
fi

# Starte die Anwendung im Desktop-Modus
log "Starte VRMusic Studio im Desktop-Modus..."

# Prüfe auf vorhandene MIDI-Geräte
if [[ "$OSTYPE" == "darwin"* ]]; then
    MIDI_DEVICES=$(system_profiler SPUSBDataType | grep -i "midi")
    if [ -n "$MIDI_DEVICES" ]; then
        log "MIDI-Geräte gefunden:"
        echo "$MIDI_DEVICES"
    else
        warning "Keine MIDI-Geräte gefunden"
    fi
fi

# Starte die Anwendung
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS-spezifische Einstellungen
    export DISPLAY=:0
    ./build/bin/VRMusicStudio --mode desktop
else
    # Linux/Windows
    ./build/bin/VRMusicStudio --mode desktop
fi

# Prüfe Exit-Status
if [ $? -eq 0 ]; then
    log "VRMusic Studio wurde erfolgreich beendet."
else
    error "VRMusic Studio wurde mit Fehlern beendet."
    exit 1
fi 