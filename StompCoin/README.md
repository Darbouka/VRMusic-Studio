# StompCoin

StompCoin ist eine innovative Fitness-App, die Tanz und Bewegung miteinander verbindet. Verdiene Coins durch deine Tanzbewegungen und verbessere deine Fitness auf spielerische Weise.

## Features

- 🎵 Verschiedene Musikgenres und Tanzflächen
- 💰 Verdiene Coins durch Tanzbewegungen
- 🏆 Ranglisten und Wettbewerbe
- 📊 Fitness-Tracking und Statistiken
- 👥 Soziale Funktionen und Freunde
- 🎮 Gamification-Elemente

## Installation

### Voraussetzungen

- macOS 12.0 oder höher
- Xcode 14.0 oder höher
- Swift 5.7 oder höher

### Installation

1. Klone das Repository:
```bash
git clone https://github.com/yourusername/StompCoin.git
cd StompCoin
```

2. Öffne das Projekt in Xcode:
```bash
open StompCoin.xcodeproj
```

3. Installiere die Abhängigkeiten:
```bash
swift package resolve
```

4. Baue und starte die App:
```bash
xcodebuild -scheme StompCoin -configuration Debug
```

## Entwicklung

### Projektstruktur

```
StompCoin/
├── macOS/                 # macOS-spezifischer Code
│   ├── Models/           # Datenmodelle
│   ├── Views/            # SwiftUI-Ansichten
│   ├── Resources/        # Ressourcen (Bilder, Fonts, etc.)
│   └── Utils/            # Hilfsfunktionen
├── Shared/               # Gemeinsam genutzter Code
│   ├── Models/          # Gemeinsame Datenmodelle
│   ├── Utils/           # Gemeinsame Hilfsfunktionen
│   └── Resources/       # Gemeinsame Ressourcen
└── Server/              # Backend-Server
    ├── src/             # Quellcode
    └── config/          # Konfigurationsdateien
```

### Entwicklungsumgebung einrichten

1. Installiere die erforderlichen Tools:
```bash
brew install swiftlint
brew install swiftformat
```

2. Konfiguriere Git Hooks:
```bash
cp .git-hooks/* .git/hooks/
chmod +x .git/hooks/*
```

3. Installiere die Entwicklungsumgebung:
```bash
./scripts/setup_dev.sh
```

### Tests ausführen

```bash
swift test
```

### Code-Formatierung

```bash
swiftformat .
swiftlint
```

## Dokumentation

Die vollständige Dokumentation findest du im [Documentation](./Documentation) Verzeichnis:

- [Benutzerhandbuch](./Documentation/UserGuide.md)
- [Entwicklerhandbuch](./Documentation/DeveloperGuide.md)
- [API-Dokumentation](./Documentation/API.md)

## Lizenz

Dieses Projekt ist unter der MIT-Lizenz lizenziert. Siehe die [LICENSE](LICENSE) Datei für Details.

## Beitragen

1. Fork das Repository
2. Erstelle einen Feature-Branch (`git checkout -b feature/AmazingFeature`)
3. Committe deine Änderungen (`git commit -m 'Add some AmazingFeature'`)
4. Push zum Branch (`git push origin feature/AmazingFeature`)
5. Öffne einen Pull Request

## Kontakt

- Website: [https://stompcoin.app](https://stompcoin.app)
- E-Mail: [contact@stompcoin.app](mailto:contact@stompcoin.app)
- Twitter: [@StompCoin](https://twitter.com/StompCoin) 