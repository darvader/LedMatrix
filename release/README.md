# Release Instructions

This folder contains pre-built firmware binaries for the LED Matrix project.

## Creating a Release

### Prerequisites
- [PlatformIO](https://platformio.org/) installed
- Git repository cloned

### Automated Build (Recommended)

Use the provided build scripts:

**Windows:**
```bash
build_release.bat
```

**Linux/Mac:**
```bash
chmod +x build_release.sh
./build_release.sh
```

### Manual Build Process

1. **Clean the project** (optional):
   ```bash
   pio run --target clean
   ```

2. **Build for ESP32**:
   ```bash
   pio run --environment nodemcu-32s
   ```

3. **Build for ESP8266**:
   ```bash
   pio run --environment nodemcuv2
   ```

4. **Copy firmware files**:
   - ESP32: Copy `.pio/build/nodemcu-32s/firmware.bin` to `release/ESP32/`
   - ESP8266: Copy `.pio/build/nodemcuv2/firmware.bin` to `release/ESP8266/`

5. **Create release archive**:
   ```bash
   # Create timestamped release
   VERSION=$(date +%Y%m%d_%H%M%S)
   mkdir release_$VERSION
   cp -r release/* release_$VERSION/
   tar -czf ledmatrix_$VERSION.tar.gz release_$VERSION/
   ```

## Flashing Instructions

### ESP32 (NodeMCU-32S)

#### Method 1: PlatformIO (Recommended)
```bash
# Connect ESP32 via USB
pio run --target upload --environment nodemcu-32s
```

#### Method 2: esptool.py
```bash
# Install esptool if not already installed
pip install esptool

# Flash the firmware
esptool.py --chip esp32 --port COM3 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader.bin 0x10000 firmware.bin 0x8000 partitions.bin
```

#### Method 3: ESP32 Download Tool
1. Download [ESP32 Download Tool](https://www.espressif.com/en/support/download/other-tools)
2. Select ESP32
3. Set flash address for firmware.bin to 0x10000
4. Select firmware.bin from release/ESP32/
5. Click START

### ESP8266 (NodeMCU v2)

#### Method 1: PlatformIO (Recommended)
```bash
# Connect ESP8266 via USB
pio run --target upload --environment nodemcuv2
```

#### Method 2: esptool.py
```bash
# Flash the firmware
esptool.py --chip esp8266 --port COM3 --baud 921600 write_flash --flash_mode dio --flash_size 4MB 0x00000 firmware.bin
```

#### Method 3: NodeMCU Flasher
1. Download [NodeMCU PyFlasher](https://github.com/marcelstoer/nodemcu-pyflasher)
2. Select COM port
3. Select firmware.bin from release/ESP8266/
4. Set baud rate to 921600
5. Click Flash

## Troubleshooting

### ESP32 Issues
- Ensure the board is in bootloader mode (press BOOT button while powering on)
- Try different USB ports or cables
- Check if the board is properly recognized: `pio device list`

### ESP8266 Issues
- Ensure GPIO0 is connected to GND during flashing
- Try lower baud rates if flashing fails
- Check serial output for error messages

### Common Issues
- **Port not found**: Install USB-to-Serial drivers (CP210x, CH340, etc.)
- **Permission denied**: Run terminal as administrator or add user to dialout group
- **Timeout**: Try different baud rates or check connections

## File Structure
```
release/
├── ESP8266/
│   └── firmware.bin          # ESP8266 firmware
├── ESP32/
│   └── firmware.bin          # ESP32 firmware
└── README.md                 # This file
```