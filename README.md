# StompCoin

StompCoin ist eine innovative Tanz-App, die Tanzschritte in digitale Währung umwandelt. Durch Gehen, Laufen und ganz besonders beim Tanzen und natürlich auch andere Aktivitäten verdienen Nutzer STOMP-Coins, die für verschiedene Zwecke verwendet werden können.

## Features

- 🏃‍♂️ **Fitness-Tracking**: Automatische Schrittzählung, Kalorienverbrauch und Distanzmessung
- 💰 **STOMP-Coins**: Verdienen und Verwenden von digitaler Währung durch Bewegung
- 🎵 **Musik-Integration**: Verschiedene Musikgenres mit unterschiedlichen Belohnungen
- 🏆 **Leaderboard**: Wettbewerb mit anderen Nutzern
- 🔒 **Sicherheit**: Zwei-Faktor-Authentifizierung und sichere Wallet-Verwaltung

## Installation

### Voraussetzungen

- macOS 12.0 oder höher
- Xcode 14.0 oder höher
- Swift 5.7 oder höher

### Installation

1. Repository klonen:
```bash
git clone https://github.com/Darbouka/StompCoin.git
cd StompCoin
```

2. Dependencies installieren:
```bash
swift package resolve
```

3. Projekt in Xcode öffnen:
```bash
open Package.swift
```

4. App bauen und ausführen:
- Wähle das Scheme "StompCoin"
- Drücke Cmd + R oder klicke auf den Play-Button

## Projektstruktur

```
StompCoin/
├── Sources/
│   └── StompCoin/
│       ├── Models/
│       │   ├── AppState.swift
│       │   └── MusicModels.swift
│       ├── Views/
│       │   ├── ContentView.swift
│       │   ├── HomeView.swift
│       │   ├── LeaderboardView.swift
│       │   ├── MusicGenreListView.swift
│       │   └── SettingsView.swift
│       └── Resources/
└── Tests/
    └── StompCoinTests/
```

## Entwicklung

### Coding Style

- Swift Style Guide von Apple folgen
- Dokumentation für öffentliche APIs
- Unit Tests für neue Features

### Git Workflow

1. Feature Branch erstellen:
```bash
git checkout -b feature/neues-feature
```

2. Änderungen committen:
```bash
git add .
git commit -m "Beschreibung der Änderungen"
```

3. Branch pushen:
```bash
git push origin feature/neues-feature
```

4. Pull Request erstellen

## Lizenz

Dieses Projekt ist unter der MIT-Lizenz lizenziert. Siehe [LICENSE](LICENSE) für Details.

## Support

Bei Fragen oder Problemen:
- Erstelle ein Issue auf GitHub
- Kontaktiere das Entwicklungsteam
- Besuche unsere [Dokumentation](Documentation/UserGuide.md)

## Mitwirken

Wir freuen uns über Beiträge! Bitte:
1. Fork das Repository
2. Erstelle einen Feature Branch
3. Committe deine Änderungen
4. Pushe zum Branch
5. Erstelle einen Pull Request 
