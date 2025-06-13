#!/bin/bash

# Farben für die Ausgabe
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Logging-Funktionen
log() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
    exit 1
}

warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Prüfe ob das Projekt gebaut wurde
if [ ! -f "build/VRMusicStudio" ]; then
    error "Projekt wurde noch nicht gebaut. Führen Sie zuerst ./scripts/build.sh aus."
fi

# Führe Unit-Tests aus
log "Führe Unit-Tests aus..."
cd build && ctest --output-on-failure

if [ $? -ne 0 ]; then
    error "Unit-Tests fehlgeschlagen!"
fi

# Teste Audio-Engine
log "Teste Audio-Engine..."
./VRMusicStudio --test-audio

if [ $? -ne 0 ]; then
    error "Audio-Engine-Test fehlgeschlagen!"
fi

# Teste MIDI-Engine
log "Teste MIDI-Engine..."
./VRMusicStudio --test-midi

if [ $? -ne 0 ]; then
    error "MIDI-Engine-Test fehlgeschlagen!"
fi

# Teste VR-Integration
log "Teste VR-Integration..."
./VRMusicStudio --test-vr

if [ $? -ne 0 ]; then
    warning "VR-Integration-Test fehlgeschlagen! (Dies ist normal, wenn keine VR-Hardware angeschlossen ist)"
fi

# Teste UI
log "Teste UI..."
./VRMusicStudio --test-ui

if [ $? -ne 0 ]; then
    error "UI-Test fehlgeschlagen!"
fi

log "Alle Tests erfolgreich abgeschlossen!" 