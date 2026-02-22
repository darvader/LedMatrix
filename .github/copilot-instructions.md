# GitHub Copilot Instructions for LED Matrix Project

## Project Overview
This is an ESP32/ESP8266-based LED matrix display project that shows sports scores, time with animations, and other visual content. The project uses HUB75 LED panels and supports both ESP32 and ESP8266 microcontrollers.

## Key Architectural Patterns

### 1. Cross-Platform Compatibility (ESP32/ESP8266)

**Pattern**: Use conditional compilation with `#ifdef ESP32`/`#else` for platform-specific code.

**Examples**:
- **Display Types**: ESP32 uses `VirtualMatrixPanel*`, ESP8266 uses `PxMATRIX*`
- **Web Servers**: ESP32 uses `WebServer`, ESP8266 uses `ESP8266WebServer`
- **WiFi Libraries**: ESP32 uses `<WiFi.h>`, ESP8266 uses `<ESP8266WiFi.h>`
- **Display Methods**: ESP32 uses `clearScreen()`/`flipDMABuffer()`, ESP8266 uses `clear()`/`showBuffer()`

**Implementation**:
```cpp
// In headers
class ManualWifiSetup {
public:
    ManualWifiSetup(
#ifdef ESP8266
        PxMATRIX* display
#else
        VirtualMatrixPanel* display
#endif
    );
private:
#ifdef ESP8266
    PxMATRIX* display;
    ESP8266WebServer server;
#else
    VirtualMatrixPanel* display;
    WebServer server;
#endif
};
```

### 2. WiFi Setup System

**Pattern**: Implement captive portal with AP mode for initial WiFi configuration.

**Components**:
- **AP Mode**: Create open access point named "LedMatrix-Setup"
- **Web Interface**: Serve HTML form at 192.168.4.1 for SSID/password input
- **Network Scanning**: Scan and display available WiFi networks in dropdown
- **EEPROM Storage**: Persist credentials in non-volatile memory
- **Fallback Input**: Allow manual SSID entry for hidden networks

**Key Features**:
- Automatic setup mode detection on first boot or connection failure
- Visual feedback via scrolling text on LED matrix
- Graceful fallback to setup mode if connection fails

### 3. Display Abstraction Layer

**Pattern**: Use inheritance and polymorphism for display operations.

**Structure**:
- `BaseLedMatrix`: Abstract base class with platform-agnostic methods
- Platform-specific implementations with conditional compilation
- Consistent API across ESP32/ESP8266 despite different underlying libraries

**Benefits**:
- Code reusability across platforms
- Centralized display operations
- Easy addition of new display types

### 4. EEPROM Management

**Pattern**: Centralized EEPROM initialization and structured data storage.

**Best Practices**:
- Initialize EEPROM early in `setup()` before any read/write operations
- Use dedicated address ranges for different data types
- Implement null-terminated string storage for credentials
- Commit changes after writing

**Example**:
```cpp
void setupEEPROM() {
  EEPROM.begin(200);  // Initialize with sufficient size
}

void saveCredentials(String ssid, String password) {
  // Write SSID
  for (int i = 0; i < maxSSIDLen; i++) {
    EEPROM.write(eepromSSIDAddr + i, i < ssid.length() ? ssid[i] : 0);
  }
  // Write password
  for (int i = 0; i < maxPassLen; i++) {
    EEPROM.write(eepromPassAddr + i, i < password.length() ? password[i] : 0);
  }
  EEPROM.commit();  // Important: Commit changes to flash
}
```

### 5. Library Dependency Management

**Pattern**: Separate library lists for different PlatformIO environments.

**ESP32 Libraries**:
```
lib_deps =
    AsyncTCP
    NTPClient
    Adafruit NeoPixel
    FastLED
    Timezone
    Adafruit GFX Library
    ESP32-HUB75-MatrixPanel-DMA
    FauxmoESP
```

**ESP8266 Libraries**:
```
lib_deps =
    PxMatrix LED MATRIX library
    FauxmoESP
    Adafruit GFX Library
    NTPClient
    Timezone
    Adafruit NeoPixel
    FastLED
```

**Key Considerations**:
- ESP8266 requires explicit inclusion of all dependencies
- Some libraries have platform-specific versions (e.g., ESPAsyncTCP vs AsyncTCP)
- Test compilation on both platforms when adding new libraries

### 6. Release and Build Management

**Pattern**: Automated build scripts with platform-specific outputs.

**Structure**:
```
release/
├── ESP32/firmware.bin
├── ESP8266/firmware.bin
└── README.md
```

**Build Scripts**:
- `build_release.bat`: Windows batch script
- `build_release.sh`: Unix shell script
- Both handle cleaning, building both platforms, and copying firmware

**Flashing Methods**:
- **PlatformIO**: `pio run --target upload --environment <env>`
- **esptool.py**: Direct flash with proper addresses
- **GUI Tools**: ESP32 Download Tool, NodeMCU Flasher

### 7. Animation and Display Modes

**Pattern**: State-based mode system with UDP control interface.

**Mode Structure**:
- Mode 1: Scoreboard display
- Modes 2-12: Time display with different animations
- Mode 30: Timer/stopwatch
- Mode 40: Counter
- Mode 60: Mandelbrot fractal

**Control Methods**:
- UDP commands for mode switching
- Alexa integration via FauxmoESP
- Web interface for some functions

## Development Guidelines

### Code Organization
- Keep platform-specific code in conditional compilation blocks
- Use consistent naming conventions
- Document platform differences in comments
- Test changes on both ESP32 and ESP8266

### Adding New Features
1. **Check Platform Compatibility**: Ensure new code works on both ESP32/ESP8266
2. **Update Libraries**: Add any new dependencies to both platform configurations
3. **Test Builds**: Run `build_release.bat` to verify compilation
4. **Update Documentation**: Add new features to README.md and this instruction file

### Debugging Tips
- Use serial output for debugging (both platforms support Serial)
- Check EEPROM initialization order
- Verify library versions are compatible
- Test WiFi functionality on actual hardware

### Common Issues and Solutions

**WiFi Setup Not Working**:
- Ensure EEPROM is initialized before WiFi setup
- Check that credentials are properly null-terminated
- Verify AP mode works (LED matrix should show scrolling text)

**Display Issues**:
- Confirm correct display type for platform
- Check pin configurations in setupDisplay()
- Verify display buffer operations use correct methods

**Build Failures**:
- Check library dependencies for target platform
- Ensure conditional compilation covers all platform differences
- Clean build directory and rebuild

## File Structure Reference

```
├── src/
│   ├── LedMatrix.cpp          # Main application logic
│   ├── ManualWifiSetup.cpp    # WiFi configuration system
│   ├── BaseLedMatrix.h/cpp    # Display abstraction
│   ├── Scoreboard.h/cpp       # Scoreboard display logic
│   └── [Other display modes]
├── lib/                       # Local libraries (if any)
├── release/                   # Built firmware binaries
│   ├── ESP32/firmware.bin
│   ├── ESP8266/firmware.bin
│   └── README.md             # Flashing instructions
├── build_release.bat         # Windows build script
├── build_release.sh          # Unix build script
├── platformio.ini           # PlatformIO configuration
└── README.md                # Project documentation
```

## PlatformIO Configuration Notes

**Environment-Specific Settings**:
- ESP32: `nodemcu-32s` with DMA display driver
- ESP8266: `nodemcuv2` with PxMatrix driver
- Different upload speeds and board configurations

**Build Flags**:
- ESP32: Higher bandwidth WiFi settings
- ESP8266: Specific flash layout configurations

This instruction file should be updated whenever new patterns or significant changes are implemented in the project.