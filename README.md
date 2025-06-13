# VRMusic Studio

Ein virtuelles Musikstudio für VR-Anwendungen, das sowohl unter macOS als auch Windows läuft.

## Features

- Cross-Platform Unterstützung (macOS & Windows)
- DirectX 11 Integration für Windows
- OpenGL/Vulkan Integration für macOS
- JUCE Audio Engine
- PortAudio Integration
- Moderne UI mit DirectX/OpenGL Rendering
- VR-Controller Unterstützung

## Voraussetzungen

### Allgemein
- CMake 3.20 oder höher
- C++17-kompatibler Compiler
- Git
- vcpkg

### macOS
- Xcode Command Line Tools
- Homebrew
- Vulkan SDK

### Windows
- Visual Studio 2022 oder neuer
- Windows SDK 10.0
- DirectX SDK

## Installation

### macOS

1. Repository klonen:
```bash
git clone https://github.com/Darbouka/VRMusic-Studio.git
cd VRMusic-Studio
```

2. Abhängigkeiten installieren:
```bash
./scripts/install_dependencies.sh
```

3. Projekt bauen:
```bash
./scripts/build.sh
```

### Windows

1. Repository klonen:
```bash
git clone https://github.com/Darbouka/VRMusic-Studio.git
cd VRMusic-Studio
```

2. Abhängigkeiten installieren:
```bash
scripts\install_dependencies.bat
```

3. Projekt bauen:
```bash
scripts\build_windows.bat
```

## Projektstruktur

```
VRMusic-Studio/
├── src/
│   ├── core/      # Kernfunktionalität
│   ├── audio/     # Audiokomponenten
│   ├── vr/        # VR-spezifische Komponenten
│   └── ui/        # Benutzeroberfläche
├── include/       # Öffentliche Header
├── resources/     # Ressourcen (Icons, etc.)
├── scripts/       # Build- und Installationsskripte
├── tests/         # Unit-Tests
└── external/      # Externe Abhängigkeiten
```

## Entwicklung

### Tests ausführen

```bash
cd build
ctest --output-on-failure
```

### Code-Formatierung

Das Projekt verwendet clang-format für die Code-Formatierung. Führen Sie vor dem Committen aus:

```bash
./scripts/format.sh
```

## Lizenz

Dieses Projekt ist unter der MIT-Lizenz lizenziert. Siehe [LICENSE](LICENSE) für Details.

## Beitragen

Beiträge sind willkommen! Bitte lesen Sie [CONTRIBUTING.md](CONTRIBUTING.md) für Details.

## Kontakt

- GitHub Issues: [Issues](https://github.com/Darbouka/VRMusic-Studio/issues)
- E-Mail: [Ihre E-Mail]
