#!/bin/bash

# Prüfe, ob Docker installiert ist
if ! command -v docker &> /dev/null; then
    echo "Docker ist nicht installiert. Bitte installiere Docker zuerst."
    exit 1
fi

# Baue das Docker-Image
echo "Baue Docker-Image..."
docker build -t vrmusicstudio-dev .

# Starte den Container
echo "Starte Entwicklungsumgebung..."
docker run -it --rm \
    -v "$(pwd):/app" \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -e DISPLAY="$DISPLAY" \
    --device /dev/dri \
    --device /dev/snd \
    vrmusicstudio-dev

# Prüfe den Exit-Status
if [ $? -eq 0 ]; then
    echo "Entwicklungsumgebung wurde erfolgreich beendet."
else
    echo "Fehler beim Ausführen der Entwicklungsumgebung."
    exit 1
fi 