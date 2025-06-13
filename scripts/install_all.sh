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

# Prüfe ob Homebrew installiert ist
if ! command -v brew &> /dev/null; then
    error "Homebrew ist nicht installiert. Bitte installieren Sie Homebrew zuerst."
fi

# Installiere Abhängigkeiten
log "Installiere Abhängigkeiten..."

# Audio
brew install portaudio
brew install rtmidi

# Grafik
brew install sdl2
brew install glm

# Logging
brew install spdlog

# Build-Tools
brew install cmake
brew install ninja

# Installiere ImGui
log "Installiere ImGui..."
./scripts/install_imgui.sh

# Baue das Projekt
log "Baue das Projekt..."
./scripts/build.sh

# Prüfe ob der Build erfolgreich war
if [ $? -eq 0 ]; then
    log "Installation erfolgreich abgeschlossen!"
else
    error "Build fehlgeschlagen!"
fi 