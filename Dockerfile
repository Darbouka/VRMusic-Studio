# Build-Stage
FROM ubuntu:22.04 AS builder

# Installiere Build-Abhängigkeiten
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    libglfw3-dev \
    libportaudio2-dev \
    libopenal-dev \
    libsqlite3-dev \
    libssl-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

# Setze Arbeitsverzeichnis
WORKDIR /build

# Kopiere Quellcode
COPY . .

# Konfiguriere und baue
RUN mkdir build && cd build && \
    cmake -G Ninja \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=/usr/local \
          .. && \
    ninja

# Runtime-Stage
FROM ubuntu:22.04

# Installiere Runtime-Abhängigkeiten
RUN apt-get update && apt-get install -y \
    libglfw3 \
    libportaudio2 \
    libopenal1 \
    libsqlite3-0 \
    libssl3 \
    && rm -rf /var/lib/apt/lists/*

# Kopiere Build-Artefakte
COPY --from=builder /build/build/vrmusicstudio /usr/local/bin/
COPY --from=builder /build/build/lib/* /usr/local/lib/

# Setze Umgebungsvariablen
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# Erstelle nicht-root Benutzer
RUN useradd -m -s /bin/bash vrmusic
USER vrmusic

# Setze Arbeitsverzeichnis
WORKDIR /home/vrmusic

# Starte Anwendung
CMD ["vrmusicstudio"]

# System-Pakete installieren
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    ninja-build \
    python3 \
    python3-pip \
    wget \
    && rm -rf /var/lib/apt/lists/*

# vcpkg installieren
RUN git clone https://github.com/Microsoft/vcpkg.git /opt/vcpkg \
    && /opt/vcpkg/bootstrap-vcpkg.sh

# Arbeitsverzeichnis erstellen
WORKDIR /app

# Projektdateien kopieren
COPY . .

# Abhängigkeiten installieren
RUN /opt/vcpkg/vcpkg install \
    spdlog:x64-linux \
    fmt:x64-linux \
    glew:x64-linux \
    glfw3:x64-linux \
    glm:x64-linux

# Build-Verzeichnis erstellen und konfigurieren
RUN mkdir build && cd build \
    && cmake .. -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build-Befehl
CMD ["cmake", "--build", "build"] 