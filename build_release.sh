#!/bin/bash
# LED Matrix Release Build Script
# This script builds firmware for both ESP8266 and ESP32 and copies to release folder

echo "Building LED Matrix firmware..."

# Clean previous builds
echo "Cleaning previous builds..."
pio run --target clean

# Build ESP32
echo "Building for ESP32..."
if ! pio run --environment nodemcu-32s; then
    echo "ESP32 build failed!"
    exit 1
fi

# Build ESP8266
echo "Building for ESP8266..."
if ! pio run --environment nodemcuv2; then
    echo "ESP8266 build failed!"
    exit 1
fi

# Copy firmware files
echo "Copying firmware files..."
cp .pio/build/nodemcu-32s/firmware.bin release/ESP32/
cp .pio/build/nodemcuv2/firmware.bin release/ESP8266/

# Create version info
VERSION=$(date +%Y%m%d_%H%M%S)

echo "Firmware built successfully!"
echo "ESP32 firmware: release/ESP32/firmware.bin"
echo "ESP8266 firmware: release/ESP8266/firmware.bin"
echo "Version: $VERSION"

echo ""
echo "To create a release archive:"
echo "mkdir release_$VERSION"
echo "cp -r release/* release_$VERSION/"
echo "tar -czf ledmatrix_$VERSION.tar.gz release_$VERSION/"