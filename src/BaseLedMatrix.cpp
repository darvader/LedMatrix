#include <BaseLedMatrix.h>

void BaseLedMatrix::clear() {
  #ifdef ESP8266
    Serial.printf("The address of display is %p\n", (void*)display);
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
    display->flipDMABuffer();
  #endif
}

