#!/bin/bash

# Verzeichnis für das Build-Produkt erstellen
mkdir -p build/StompCoin.app/Contents/MacOS
mkdir -p build/StompCoin.app/Contents/Resources

# App bauen
swift build -c release

# Executable kopieren
cp .build/release/StompCoin build/StompCoin.app/Contents/MacOS/

# Info.plist kopieren
cp Sources/StompCoin/Info.plist build/StompCoin.app/Contents/

# Ressourcen kopieren
cp -r Sources/StompCoin/Resources/* build/StompCoin.app/Contents/Resources/

# Berechtigungen setzen
chmod +x build/StompCoin.app/Contents/MacOS/StompCoin

echo "Build abgeschlossen: build/StompCoin.app" 