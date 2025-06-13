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

# Überprüfe Berechtigungen
check_permissions() {
    print_step "Überprüfe Berechtigungen..."
    
    if [ "$EUID" -ne 0 ]; then
        print_error "Dieses Skript muss mit sudo ausgeführt werden."
        exit 1
    fi
}

# Installiere VRMusicStudio
install_vrmusicstudio() {
    print_step "Installiere VRMusicStudio..."
    
    # App Bundle kopieren
    cp -r VRMusicStudio.app /Applications/
    
    # Berechtigungen setzen
    chown -R root:admin /Applications/VRMusicStudio.app
    chmod -R 755 /Applications/VRMusicStudio.app
    
    # Symlinks erstellen
    ln -sf /Applications/VRMusicStudio.app/Contents/MacOS/VRMusicStudio /usr/local/bin/vrmusicstudio
}

# Installiere Launch Agent
install_launch_agent() {
    print_step "Installiere Launch Agent..."
    
    # Launch Agent Verzeichnis erstellen
    mkdir -p ~/Library/LaunchAgents
    
    # Launch Agent plist erstellen
    cat > ~/Library/LaunchAgents/com.vrmusicstudio.plist << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>com.vrmusicstudio</string>
    <key>ProgramArguments</key>
    <array>
        <string>/Applications/VRMusicStudio.app/Contents/MacOS/VRMusicStudio</string>
    </array>
    <key>RunAtLoad</key>
    <true/>
    <key>KeepAlive</key>
    <true/>
</dict>
</plist>
EOF
    
    # Berechtigungen setzen
    chmod 644 ~/Library/LaunchAgents/com.vrmusicstudio.plist
}

# Erstelle Verzeichnisse
create_directories() {
    print_step "Erstelle Verzeichnisse..."
    
    # Benutzerverzeichnisse
    mkdir -p ~/Documents/VRMusicStudio/Projects
    mkdir -p ~/Documents/VRMusicStudio/Plugins
    mkdir -p ~/Documents/VRMusicStudio/Presets
    mkdir -p ~/Documents/VRMusicStudio/Exports
    
    # Cache-Verzeichnis
    mkdir -p ~/Library/Caches/VRMusicStudio
}

# Konfiguriere Umgebungsvariablen
setup_environment() {
    print_step "Konfiguriere Umgebungsvariablen..."
    
    # .zshrc aktualisieren
    if ! grep -q "VRMusicStudio" ~/.zshrc; then
        echo 'export VRMUSICSTUDIO_HOME="$HOME/Documents/VRMusicStudio"' >> ~/.zshrc
        echo 'export PATH="$PATH:/Applications/VRMusicStudio.app/Contents/MacOS"' >> ~/.zshrc
    fi
}

# Hauptfunktion
main() {
    print_step "Starte Installationsprozess..."
    
    check_permissions
    install_vrmusicstudio
    install_launch_agent
    create_directories
    setup_environment
    
    print_success "Installation erfolgreich abgeschlossen!"
    print_success "VRMusicStudio wurde installiert und kann über das Launchpad oder Terminal gestartet werden."
}

# Starte Hauptfunktion
main 