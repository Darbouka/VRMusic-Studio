# StompCoin für macOS

StompCoin ist eine innovative Fitness-App, die Tanz und Bewegung miteinander verbindet. Verdiene Coins durch deine Tanzbewegungen und verbessere deine Fitness auf spielerische Weise.

## Systemanforderungen

- macOS 12.0 oder höher
- Xcode 13.0 oder höher (für die Entwicklung)
- Swift 5.5 oder höher

## Installation

1. Klone das Repository:
```bash
git clone https://github.com/yourusername/StompCoin.git
cd StompCoin/macOS
```

2. Baue die App:
```bash
./build_app.sh
```

3. Die fertige App findest du unter `build/StompCoin.app`

## Entwicklung

1. Öffne das Projekt in Xcode:
```bash
open Package.swift
```

2. Installiere die Abhängigkeiten:
```bash
swift package resolve
```

3. Baue das Projekt:
```bash
swift build
```

## Funktionen

- 🎵 Verschiedene Musikgenres und Tanzfloors
- 💰 Verdiene Coins durch Tanzbewegungen
- 📊 Verfolge deine Fitness-Fortschritte
- 🏆 Ranglisten und Wettbewerbe
- 🔔 Benachrichtigungen und Erinnerungen
- 🌙 Dark Mode Unterstützung

## Technologien

- SwiftUI für die Benutzeroberfläche
- Alamofire für Netzwerkanfragen
- SDWebImage für Bildverarbeitung

## Lizenz

Copyright © 2024 StompCoin. Alle Rechte vorbehalten. 