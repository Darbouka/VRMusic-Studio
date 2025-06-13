# Build-Stage
FROM ubuntu:22.04 AS builder

# Installiere Build-Abhängigkeiten
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    python3 \
    python3-pip \
    wget \
    unzip \
    # OpenGL und X11
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libx11-dev \
    libxext-dev \
    libxrender-dev \
    libxfixes-dev \
    libxdamage-dev \
    libxcomposite-dev \
    # Audio
    libasound2-dev \
    libpulse-dev \
    libportaudio2-dev \
    libopenal-dev \
    # System
    libudev-dev \
    libdbus-1-dev \
    libssl-dev \
    libsqlite3-dev \
    # Vulkan
    libvulkan-dev \
    libxcb1-dev \
    libxcb-icccm4-dev \
    libxcb-image0-dev \
    libxcb-keysyms1-dev \
    libxcb-randr0-dev \
    libxcb-render-util0-dev \
    libxcb-shape0-dev \
    libxcb-sync-dev \
    libxcb-xfixes0-dev \
    libxcb-xinerama0-dev \
    libxcb-xkb-dev \
    libxkbcommon-dev \
    libxkbcommon-x11-dev \
    && rm -rf /var/lib/apt/lists/*

# vcpkg installieren und konfigurieren
RUN git clone https://github.com/Microsoft/vcpkg.git /opt/vcpkg \
    && /opt/vcpkg/bootstrap-vcpkg.sh \
    && /opt/vcpkg/vcpkg install \
        spdlog:x64-linux \
        fmt:x64-linux \
        glew:x64-linux \
        glfw3:x64-linux \
        glm:x64-linux \
        portaudio:x64-linux \
        openssl:x64-linux \
        sqlite3:x64-linux \
        vulkan:x64-linux

# Setze Arbeitsverzeichnis
WORKDIR /build

# Kopiere Quellcode
COPY . .

# Konfiguriere und baue
RUN mkdir build && cd build && \
    cmake -G Ninja \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=/usr/local \
          -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake \
          -DCMAKE_OSX_DEPLOYMENT_TARGET=15.5 \
          -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
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
    libvulkan1 \
    && rm -rf /var/lib/apt/lists/*

# Kopiere Build-Artefakte
COPY --from=builder /build/build/vrmusicstudio /usr/local/bin/
COPY --from=builder /build/build/lib/* /usr/local/lib/
COPY --from=builder /opt/vcpkg/installed/x64-linux/lib/* /usr/local/lib/

# Setze Umgebungsvariablen
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# Erstelle nicht-root Benutzer
RUN useradd -m -s /bin/bash vrmusic
USER vrmusic

# Setze Arbeitsverzeichnis
WORKDIR /home/vrmusic

# SteamVR Runtime installieren
RUN wget https://github.com/ValveSoftware/steam-runtime/releases/download/v0.20230912.0/steam-runtime-release_latest.tar.xz \
    && tar xf steam-runtime-release_latest.tar.xz \
    && rm steam-runtime-release_latest.tar.xz

# Starte Anwendung
CMD ["vrmusicstudio"] 