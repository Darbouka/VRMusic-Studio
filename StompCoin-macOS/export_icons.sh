#!/bin/bash

# Erstelle das Icon-Verzeichnis
mkdir -p Sources/Assets.xcassets/AppIcon.appiconset

# Größen für die Icons
sizes=(16 32 128 256 512)

# Generiere Icons in verschiedenen Größen
for size in "${sizes[@]}"; do
    # 1x
    sips -z $size $size IconGenerator.png --out "Sources/Assets.xcassets/AppIcon.appiconset/icon_${size}x${size}.png"
    # 2x
    sips -z $((size*2)) $((size*2)) IconGenerator.png --out "Sources/Assets.xcassets/AppIcon.appiconset/icon_${size}x${size}@2x.png"
done

# Füge Icon zur Info.plist hinzu
echo '<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleIconFile</key>
    <string>AppIcon</string>
</dict>
</plist>' > Sources/Info.plist 