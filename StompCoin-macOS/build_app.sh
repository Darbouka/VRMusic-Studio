#!/bin/bash

# Verzeichnisse erstellen
mkdir -p StompCoin.app/Contents/MacOS
mkdir -p StompCoin.app/Contents/Resources

# App bauen
swift build -c release

# Binary kopieren
cp .build/release/StompCoin StompCoin.app/Contents/MacOS/

# Info.plist kopieren
cp Sources/StompCoin/Info.plist StompCoin.app/Contents/

# Berechtigungen setzen
chmod +x StompCoin.app/Contents/MacOS/StompCoin

# App in Applications kopieren
cp -R StompCoin.app /Applications/

echo "StompCoin wurde erfolgreich installiert!" 