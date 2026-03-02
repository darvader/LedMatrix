#include <BaseLedMatrix.h>

void BaseLedMatrix::clear() {
  #ifdef ESP8266
    display->clearDisplay();
  #endif
  #ifdef ESP32
    display->clearScreen();
  #endif
}

void BaseLedMatrix::showBuffer() {
  #ifdef ESP8266
    display->showBuffer();
  #endif
  #ifdef ESP32
    // Correct double-buffer pattern per HUB75 library examples:
    // flip → delay (wait one refresh cycle) → caller must then call clear() → draw
    display->flipDMABuffer();
    if (dmaDisplay && dmaDisplay->calculated_refresh_rate > 0) {
      delay(1000 / dmaDisplay->calculated_refresh_rate);
    } else {
      delay(20);
    }
  #endif
}

