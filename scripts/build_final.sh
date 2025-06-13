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

# Überprüfe macOS Version
check_macos_version() {
    print_step "Überprüfe macOS Version..."
    MACOS_VERSION=$(sw_vers -productVersion)
    if [[ "$MACOS_VERSION" != "15.5" ]]; then
        print_error "Dieses Skript ist für macOS 15.5 (Sequoia) optimiert. Gefunden: $MACOS_VERSION"
        exit 1
    fi
}

# Installiere Abhängigkeiten
install_dependencies() {
    print_step "Installiere Abhängigkeiten..."
    
    # Homebrew
    if ! command -v brew &> /dev/null; then
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
    
    # Abhängigkeiten
    brew install cmake ninja vulkan-volk portaudio spdlog glm glfw glew nlohmann-json boost
    
    # Xcode Command Line Tools
    xcode-select --install || true
}

# Konfiguriere vcpkg
setup_vcpkg() {
    print_step "Konfiguriere vcpkg..."
    
    if [ ! -d "vcpkg" ]; then
        git clone https://github.com/Microsoft/vcpkg.git
        cd vcpkg
        ./bootstrap-vcpkg.sh
        cd ..
    fi
    
    # vcpkg aktualisieren
    cd vcpkg
    git pull
    ./bootstrap-vcpkg.sh
    cd ..
}

# Klone JUCE
setup_juce() {
    print_step "Konfiguriere JUCE..."
    
    if [ ! -d "external/JUCE" ]; then
        mkdir -p external
        git clone https://github.com/juce-framework/JUCE.git external/JUCE
        cd external/JUCE
        git checkout 7.0.9
        cd ../..
    fi
}

# Baue das Projekt
build_project() {
    print_step "Baue das Projekt..."
    
    # Build-Verzeichnis erstellen
    rm -rf build
    mkdir -p build
    cd build
    
    # CMake konfigurieren
    cmake .. \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=15.5 \
        -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_MAKE_PROGRAM=ninja
    
    # Projekt bauen
    cmake --build . --config Release
    
    cd ..
}

# Erstelle App Bundle
create_app_bundle() {
    print_step "Erstelle App Bundle..."
    
    # App Bundle Struktur
    rm -rf VRMusicStudio.app
    mkdir -p "VRMusicStudio.app/Contents/MacOS"
    mkdir -p "VRMusicStudio.app/Contents/Resources"
    mkdir -p "VRMusicStudio.app/Contents/Frameworks"
    
    # Binary kopieren
    cp build/VRMusicStudio "VRMusicStudio.app/Contents/MacOS/"
    
    # Info.plist erstellen
    cat > "VRMusicStudio.app/Contents/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>VRMusicStudio</string>
    <key>CFBundleIdentifier</key>
    <string>com.vrmusicstudio.app</string>
    <key>CFBundleName</key>
    <string>VRMusicStudio</string>
    <key>CFBundleDisplayName</key>
    <string>VRMusicStudio</string>
    <key>CFBundleVersion</key>
    <string>1.0.0</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0.0</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>LSMinimumSystemVersion</key>
    <string>15.5</string>
</dict>
</plist>
EOF
    
    # Ressourcen kopieren
    cp -r resources/* "VRMusicStudio.app/Contents/Resources/"
    
    # Frameworks kopieren
    cp -r /usr/local/lib/*.dylib "VRMusicStudio.app/Contents/Frameworks/"
    
    # Code signieren
    codesign --force --deep --sign - "VRMusicStudio.app"
}

# Hauptfunktion
main() {
    print_step "Starte finalen Build-Prozess für macOS 15.5..."
    
    check_macos_version
    install_dependencies
    setup_vcpkg
    setup_juce
    build_project
    create_app_bundle
    
    print_success "Build erfolgreich abgeschlossen!"
    print_success "VRMusicStudio.app wurde erstellt."
}

# Starte Hauptfunktion
main 