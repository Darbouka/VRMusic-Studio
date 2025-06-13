#!/bin/bash

# Farben für die Ausgabe
RED='\033[0;31m'
GREEN='\033[0;32m'
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

# Überprüfe Systemanforderungen
log "Überprüfe Systemanforderungen..."

# Überprüfe macOS-Version
MACOS_VERSION=$(sw_vers -productVersion)
if [[ $(echo "$MACOS_VERSION 15.0" | awk '{print ($1 < $2)}') -eq 1 ]]; then
    error "macOS 15.0 oder höher wird benötigt. Aktuelle Version: $MACOS_VERSION"
    exit 1
fi

# Überprüfe verfügbaren Speicher
TOTAL_MEM=$(sysctl hw.memsize | awk '{print $2 / 1024 / 1024 / 1024}')
if (( $(echo "$TOTAL_MEM < 8" | bc -l) )); then
    warning "Weniger als 8GB RAM verfügbar. Empfohlen: 16GB oder mehr."
fi

# Überprüfe verfügbaren Speicherplatz
DISK_SPACE=$(df -h / | awk 'NR==2 {print $4}' | sed 's/G//')
if (( $(echo "$DISK_SPACE < 10" | bc -l) )); then
    warning "Weniger als 10GB freier Speicherplatz. Empfohlen: 20GB oder mehr."
fi

# Konfiguriere Audio-Einstellungen
log "Konfiguriere Audio-Einstellungen..."

# Überprüfe JACK-Status
if pgrep -x "jackd" > /dev/null; then
    log "JACK-Server läuft bereits."
else
    log "Starte JACK-Server..."
    jackd -d coreaudio &
fi

# Konfiguriere MIDI-Einstellungen
log "Konfiguriere MIDI-Einstellungen..."
if [ -d "/Library/Audio/MIDI Drivers" ]; then
    log "MIDI-Treiber-Verzeichnis gefunden."
else
    warning "MIDI-Treiber-Verzeichnis nicht gefunden. Erstelle Verzeichnis..."
    sudo mkdir -p "/Library/Audio/MIDI Drivers"
fi

# Konfiguriere VR-Einstellungen
log "Konfiguriere VR-Einstellungen..."
if [ -d "/Library/Application Support/OpenVR" ]; then
    log "OpenVR-Konfigurationsverzeichnis gefunden."
else
    warning "OpenVR-Konfigurationsverzeichnis nicht gefunden. Erstelle Verzeichnis..."
    sudo mkdir -p "/Library/Application Support/OpenVR"
fi

# Erstelle Benutzerkonfigurationsverzeichnis
log "Erstelle Benutzerkonfigurationsverzeichnis..."
mkdir -p ~/.config/VRMusicStudio

# Erstelle Standardkonfigurationsdatei
cat > ~/.config/VRMusicStudio/config.json << EOF
{
    "audio": {
        "sampleRate": 44100,
        "blockSize": 512,
        "channels": 2
    },
    "midi": {
        "enabled": true,
        "devices": []
    },
    "vr": {
        "enabled": true,
        "headset": "auto"
    },
    "ui": {
        "theme": "dark",
        "scale": 1.0
    }
}
EOF

log "Konfiguration abgeschlossen!"
log "Sie können die Einstellungen in ~/.config/VRMusicStudio/config.json anpassen." 