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

# 


