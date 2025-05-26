# StompCoin

StompCoin ist eine innovative iOS-App, die Fitness-Tracking mit Kryptowährung verbindet. Nutzer können durch körperliche Aktivität StompCoins verdienen, diese handeln und mit anderen Nutzern teilen.

## Features

- **Fitness-Tracking**: Schritte, Distanz und Kalorienverbrauch
- **Kryptowährung**: Mining durch Bewegung
- **Wallet**: Sichere Verwaltung von StompCoins
- **Social**: Freunde hinzufügen, Gruppen erstellen
- **Trading**: StompCoins kaufen und verkaufen
- **Offline-Modus**: Funktionen auch ohne Internet
- **Mehrsprachig**: Deutsch, Englisch, Französisch, Spanisch

## Technische Details

- **iOS Version**: 15.0+
- **Swift Version**: 5.5
- **Architektur**: MVVM
- **Dependencies**: SwiftUI, Combine, CoreData
- **APIs**: Blockchain, Fitness, Social, Analytics

## Installation

1. Repository klonen:
```bash
git clone https://github.com/Darbouka/StompCoin.git
```

2. Dependencies installieren:
```bash
cd StompCoin
pod install
```

3. Projekt öffnen:
```bash
open StompCoin.xcworkspace
```

4. API-Keys konfigurieren:
- Öffne `Config.swift`
- Trage deine API-Keys ein

## Entwicklung

- **Branching**: Git Flow
- **CI/CD**: GitHub Actions
- **Tests**: Unit Tests, UI Tests
- **Linting**: SwiftLint
- **Documentation**: SwiftDoc

## Deployment

1. Version in `Config.swift` aktualisieren
2. Tests ausführen:
```bash
xcodebuild test -workspace StompCoin.xcworkspace -scheme StompCoin
```
3. Archive erstellen:
```bash
xcodebuild archive -workspace StompCoin.xcworkspace -scheme StompCoin
```
4. App Store Connect hochladen

## Sicherheit

- Verschlüsselte Datenspeicherung
- Sichere API-Kommunikation
- KYC-Verifizierung
- Regelmäßige Sicherheitsaudits

## Support

- Email: support@stompcoin.com
- Website: https://support.stompcoin.com
- Twitter: @StompCoin

## Lizenz

Copyright © 2024 StompCoin. Alle Rechte vorbehalten. 