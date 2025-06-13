# VR Music Studio

Ein virtuelles Musikstudio für VR-Anwendungen.

## Voraussetzungen

- CMake 3.15 oder höher
- C++17-kompatibler Compiler
- Git
- vcpkg (wird automatisch installiert)

## Installation

### Automatische Installation

1. Klonen Sie das Repository:
   ```bash
   git clone https://github.com/yourusername/vrmusicstudio.git
   cd vrmusicstudio
   ```

2. Führen Sie das Installationsskript aus:
   ```bash
   chmod +x scripts/install_dependencies.sh
   ./scripts/install_dependencies.sh
   ```

3. Bauen Sie das Projekt:
   ```bash
   chmod +x scripts/build.sh
   ./scripts/build.sh
   ```

### Manuelle Installation

1. Installieren Sie vcpkg:
   ```bash
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   ./bootstrap-vcpkg.sh
   ```

2. Installieren Sie die Abhängigkeiten:
   ```bash
   ./vcpkg install spdlog:x64-osx
   ./vcpkg install fmt:x64-osx
   ./vcpkg install glew:x64-osx
   ./vcpkg install glfw3:x64-osx
   ./vcpkg install glm:x64-osx
   ```

3. Bauen Sie das Projekt:
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
   cmake --build .
   ```

### Docker-Installation

1. Bauen Sie das Docker-Image:
   ```bash
   docker build -t vrmusicstudio .
   ```

2. Führen Sie den Container aus:
   ```bash
   docker run -it vrmusicstudio
   ```

## Entwicklung

### Projektstruktur

```
vrmusicstudio/
├── src/
│   ├── core/      # Kernfunktionalität
│   ├── audio/     # Audiokomponenten
│   ├── vr/        # VR-spezifische Komponenten
│   └── ui/        # Benutzeroberfläche
├── include/       # Öffentliche Header
├── tests/         # Unit-Tests
└── scripts/       # Build- und Installationsskripte
```

### Tests ausführen

```bash
cd build
ctest --output-on-failure
```

## Lizenz

Dieses Projekt ist unter der MIT-Lizenz lizenziert. Siehe [LICENSE](LICENSE) für Details.
