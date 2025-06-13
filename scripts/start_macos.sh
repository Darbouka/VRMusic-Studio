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

# Überprüfe Installation
check_installation() {
    print_step "Überprüfe Installation..."
    
    if [ ! -d "/Applications/VRMusicStudio.app" ]; then
        print_error "VRMusicStudio ist nicht installiert."
        print_error "Bitte führen Sie zuerst das Installationsskript aus."
        exit 1
    fi
}

# Überprüfe VR-Headset
check_vr_headset() {
    print_step "Überprüfe VR-Headset..."
    
    # SteamVR
    if [ -d "/Applications/SteamVR.app" ]; then
        open -a SteamVR
    fi
    
    # Oculus
    if [ -d "/Applications/Oculus.app" ]; then
        open -a Oculus
    fi
    
    # Warte auf VR-Headset
    print_step "Warte auf VR-Headset..."
    sleep 5
}

# Starte VRMusicStudio
start_vrmusicstudio() {
    print_step "Starte VRMusicStudio..."
    
    # App starten
    open -a VRMusicStudio
    
    # Warte auf Start
    sleep 2
    
    # Überprüfe Prozess
    if pgrep -x "VRMusicStudio" > /dev/null; then
        print_success "VRMusicStudio wurde erfolgreich gestartet."
    else
        print_error "VRMusicStudio konnte nicht gestartet werden."
        exit 1
    fi
}

# Hauptfunktion
main() {
    print_step "Starte VRMusicStudio..."
    
    check_installation
    check_vr_headset
    start_vrmusicstudio
}

# Starte Hauptfunktion
main 