# Beitragsrichtlinien für StompCoin

Vielen Dank für dein Interesse an StompCoin! Hier sind die Richtlinien für Beiträge.

## Code of Conduct

- Respektiere andere Entwickler
- Sei konstruktiv in Diskussionen
- Halte dich an die Coding Standards

## Entwicklungsumgebung

1. **Voraussetzungen**
   - macOS 12.0+
   - Xcode 14.0+
   - Swift 5.7+
   - Git

2. **Setup**
   ```bash
   git clone https://github.com/Darbouka/StompCoin.git
   cd StompCoin
   swift package resolve
   open Package.swift
   ```

## Workflow

1. **Issue erstellen**
   - Beschreibe das Problem/Feature
   - Füge Screenshots hinzu (falls relevant)
   - Wähle das passende Label

2. **Branch erstellen**
   ```bash
   git checkout -b feature/issue-nummer-kurzbeschreibung
   ```

3. **Entwicklung**
   - Folge dem Swift Style Guide
   - Schreibe Tests für neue Features
   - Dokumentiere öffentliche APIs

4. **Commit**
   ```bash
   git add .
   git commit -m "Beschreibung der Änderungen"
   ```

5. **Pull Request**
   - Beschreibe die Änderungen
   - Verlinke das Issue
   - Füge Screenshots hinzu (falls relevant)

## Coding Standards

- **Swift Style Guide**
  - Verwende 4 Spaces für Einrückung
  - Max. 120 Zeichen pro Zeile
  - Dokumentiere öffentliche APIs

- **Architektur**
  - MVVM-Pattern
  - Dependency Injection
  - Unit Tests

- **Namenskonventionen**
  - Klare, beschreibende Namen
  - Englische Namen für Code
  - Deutsch für UI-Texte

## Tests

- Schreibe Unit Tests für neue Features
- Stelle sicher, dass alle Tests durchlaufen
- Teste Edge Cases

## Dokumentation

- Dokumentiere neue Features
- Aktualisiere die README.md
- Füge Kommentare zu komplexem Code hinzu

## Review-Prozess

1. Code Review durch Maintainer
2. Feedback einarbeiten
3. Tests müssen durchlaufen
4. CI/CD Pipeline muss erfolgreich sein

## Lizenz

Mit deinem Beitrag stimmst du zu, dass dein Code unter der MIT-Lizenz veröffentlicht wird. 