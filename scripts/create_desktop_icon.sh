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

# Prüfe ob die Anwendung existiert
if [ ! -f "build/VRMusicStudio" ]; then
    error "Anwendung wurde noch nicht gebaut. Führen Sie zuerst ./scripts/build.sh aus."
fi

# Erstelle .desktop Datei
log "Erstelle Desktop-Icon..."
cat > VRMusicStudio.desktop << EOL
[Desktop Entry]
Version=1.0
Type=Application
Name=VRMusic Studio
Comment=Ein virtuelles Musikstudio für VR
Exec=$(pwd)/scripts/start.sh --desktop
Icon=$(pwd)/assets/icon.png
Terminal=false
Categories=AudioVideo;Audio;Music;
StartupWMClass=VRMusicStudio
EOL

# Mache die .desktop Datei ausführbar
chmod +x VRMusicStudio.desktop

# Kopiere die .desktop Datei in das Applications-Verzeichnis
if [ -d "$HOME/Applications" ]; then
    cp VRMusicStudio.desktop "$HOME/Applications/"
    log "Desktop-Icon wurde in $HOME/Applications installiert"
elif [ -d "/Applications" ]; then
    sudo cp VRMusicStudio.desktop "/Applications/"
    log "Desktop-Icon wurde in /Applications installiert"
else
    warning "Konnte Applications-Verzeichnis nicht finden"
    log "Desktop-Icon wurde im aktuellen Verzeichnis erstellt: $(pwd)/VRMusicStudio.desktop"
fi

log "Desktop-Icon wurde erfolgreich erstellt!" 