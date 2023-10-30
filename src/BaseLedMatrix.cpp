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
}

