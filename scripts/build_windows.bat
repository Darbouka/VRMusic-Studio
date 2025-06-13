@echo off
setlocal enabledelayedexpansion

echo Building VRMusic Studio for Windows...

:: Prüfe Visual Studio Installation
where cl >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Visual Studio nicht gefunden. Bitte installieren Sie Visual Studio 2022 oder neuer.
    exit /b 1
)

:: Prüfe vcpkg Installation
if not defined VCPKG_ROOT (
    echo VCPKG_ROOT ist nicht gesetzt. Bitte setzen Sie die Umgebungsvariable VCPKG_ROOT.
    exit /b 1
)

:: Erstelle Build-Verzeichnis
if not exist build mkdir build
cd build

:: Konfiguriere CMake
cmake .. -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" ^
    -DCMAKE_BUILD_TYPE=Release

if %ERRORLEVEL% neq 0 (
    echo CMake Konfiguration fehlgeschlagen.
    exit /b 1
)

:: Baue das Projekt
cmake --build . --config Release

if %ERRORLEVEL% neq 0 (
    echo Build fehlgeschlagen.
    exit /b 1
)

echo Build erfolgreich abgeschlossen!
echo Die ausführbare Datei befindet sich in: %CD%\Release\VRMusicStudio.exe

cd .. 