# LedMatrix
This project runs a HUB75 RGB LED matrix panel with either an ESP8266 or ESP32. Its main purpose is to display a **scoreboard** for Volleyball and other, showing scores, sets, timeouts, and scrolling text. It can also be controlled by the Android app [Scoreboard](https://github.com/darvader/Scoreboard) or [Smart Home](https://github.com/darvader/SmartHome).

Additionally, it can display the current **time with various animations** when not showing the scoreboard.


# License
[MIT license](LICENSE.txt)

# build
You need to use PlatformIO to build and upload to the ESP

# WiFi Setup
The LED matrix features an automatic WiFi setup process for easy configuration:

1. **On first boot** (or if WiFi connection fails), the device enters setup mode:
   - Creates an open Access Point named "LedMatrix-Setup"
   - The LED matrix displays scrolling text: "Connect to LedMatrix-Setup and go to 192.168.4.1 to setup WiFi"

2. **Connect to the setup network**:
   - Use your phone or computer to connect to the "LedMatrix-Setup" WiFi network
   - Open a web browser and navigate to `http://192.168.4.1`

3. **Configure WiFi**:
   - The web page will show a dropdown list of available WiFi networks
   - Select your network from the list, or enter a custom SSID manually
   - Enter your WiFi password
   - Click "Save"

4. **Automatic connection**:
   - The device will save your credentials and reboot
   - It will then connect to your configured WiFi network
   - The LED matrix will display normal content instead of setup text

If the device fails to connect to WiFi after configuration, it will automatically return to setup mode.

# Features

## Scoreboard Mode
- Displays current score (points and sets for both teams)
- Shows which team is serving
- Timeout indicator
- Scrolling text for team names or messages
- Controlled via UDP commands from Android app

## Time Display with Animations
The device can display the current time with various animated backgrounds:

- **Time Sample 1-4**: Different time display styles
- **Snow Animation**: Falling snow effect (white or colored)
- **Plasma**: Animated plasma effect
- **Game of Life**: Conway's Game of Life simulation
- **Ellipse**: Animated ellipses
- **Star Wars**: Star Wars-style scrolling text with time

## Other Features
- **Timer/Stopwatch**: Countdown timer and stopwatch functionality
- **Counter**: Numeric counter display
- **Mandelbrot**: Fractal visualization
- **Picture Display**: Custom RGB image display via UDP
- **Brightness Control**: Adjustable display brightness
- **Alexa Integration**: Voice control via FauxmoESP (Amazon Echo compatibility)

# Used Libraries
[//]: # (---)

## Hardware Assembly: 4x HUB75 Matrices with ESP32

To build a large LED display, you can connect four HUB75 RGB LED matrix panels together and control them with an ESP32. Here’s how to assemble and wire the hardware:

### 1. Physical Arrangement
- **Panels can be arranged in a 2x2 grid (recommended for square displays) or a 1x4 strip (for long displays).**
- Each panel has an input (IN) and output (OUT) HUB75 connector.

### 2. Daisy-Chaining Panels
1. Connect the ESP32 to the **IN** connector of the first panel using jumper wires or a HUB75 adapter board.
2. Use the provided ribbon cable to connect the **OUT** of the first panel to the **IN** of the second panel.
3. Repeat for the third and fourth panels, chaining OUT to IN.
4. The data flows from the ESP32 through all four panels in sequence.

### 3. ESP32 to HUB75 Pin Connections
Wire the ESP32 GPIO pins to the HUB75 connector of the first panel as follows (default for ESP32-HUB75-MatrixPanel-DMA):

| HUB75 Pin | ESP32 GPIO | Signal Description |
|----------|------------|-------------------|
| R1       | GPIO 25    | Red data (upper)  |
| G1       | GPIO 26    | Green data (upper)|
| B1       | GPIO 27    | Blue data (upper) |
| R2       | GPIO 14    | Red data (lower)  |
| G2       | GPIO 12    | Green data (lower)|
| B2       | GPIO 13    | Blue data (lower) |
| A        | GPIO 23    | Row select A      |
| B        | GPIO 19    | Row select B      |
| C        | GPIO 5     | Row select C      |
| D        | GPIO 17    | Row select D      |
| E        | GPIO 18    | Row select E (if used) |
| LAT      | GPIO 4     | Latch             |
| OE       | GPIO 15    | Output Enable     |
| CLK      | GPIO 16    | Clock             |
| GND      | GND        | Ground            |

> **Note:** Pin numbers may vary depending on your configuration. Check your `platformio.ini` and code for the actual mapping.

### 4. Power Supply
- You can temporarily power a single panel from USB or the ESP32, but you must limit the display content and avoid lighting all pixels at once. For reliable operation and full brightness, always use a dedicated 5V power supply.
- Use a dedicated 5V, high-current power supply (e.g., 5V 10A or higher for 4 panels).
- Connect 5V and GND from the power supply directly to the panels’ power terminals.
- Connect ESP32 GND to the panel GND to ensure a common ground.

### 5. Example Wiring Diagram (Text)

```
ESP32         Panel 1 IN      Panel 1 OUT   Panel 2 IN ... Panel 4 OUT
   |  --------> [ HUB75 ] ----> [ HUB75 ] -> [ HUB75 ] ...
   |--GND----------------------GND (all grounds connected)
   |--5V-----------------------5V (panels only, not ESP32)
```

### 6. Additional Tips
- Double-check all connections before powering up.
- Panels can draw significant current; use thick wires for power.
- For more details, see the [ESP32-HUB75-MatrixPanel-DMA documentation](https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA).

---
The project uses the following libraries:

## Display Libraries
- **[PxMatrix LED MATRIX library](https://github.com/2dom/PxMatrix)**: For ESP8266 HUB75 matrix control
- **[ESP32-HUB75-MatrixPanel-DMA](https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA)**: For ESP32 HUB75 matrix control with DMA

## Networking & Communication
- **[AsyncTCP](https://github.com/me-no-dev/AsyncTCP)**: Asynchronous TCP library for ESP32
- **[NTPClient](https://github.com/arduino-libraries/NTPClient)**: NTP client for time synchronization
- **[FauxmoESP](https://github.com/vintlabs/FauxmoESP)**: Amazon Alexa (Echo) integration

## LED & Graphics
- **[Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)**: NeoPixel LED control
- **[FastLED](https://github.com/FastLED/FastLED)**: Advanced LED control library
- **[Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)**: Graphics library for displays

## Time & Utilities
- **[Timezone](https://github.com/JChristensen/Timezone)**: Timezone handling library

# Releases & Flashing

## Creating a Release
See the [release/README.md](release/README.md) for detailed instructions on building and packaging firmware releases.

For quick builds, use the provided scripts:
- **Windows**: `build_release.bat`
- **Linux/Mac**: `chmod +x build_release.sh && ./build_release.sh`

## Flashing to ESP32 (NodeMCU-32S)
1. Connect your ESP32 board via USB
2. Use PlatformIO: `pio run --target upload --environment nodemcu-32s`
3. Or use esptool.py: `esptool.py --chip esp32 --port COM3 --baud 921600 write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x10000 firmware.bin`

## Flashing to ESP8266 (NodeMCU v2)
1. Connect your ESP8266 board via USB
2. Put ESP8266 in flash mode (GPIO0 to GND)
3. Use PlatformIO: `pio run --target upload --environment nodemcuv2`
4. Or use esptool.py: `esptool.py --chip esp8266 --port COM3 --baud 921600 write_flash --flash_mode dio --flash_size 4MB 0x00000 firmware.bin`

For detailed flashing instructions and troubleshooting, see [release/README.md](release/README.md).

# 


