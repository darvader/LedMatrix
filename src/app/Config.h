#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#ifdef ESP8266
    constexpr int16_t MATRIX_WIDTH = 64;
    constexpr int16_t MATRIX_HEIGHT = 32;
    constexpr uint8_t TEXT_SIZE_LARGE = 2;
    constexpr uint8_t TEXT_SIZE_SMALL = 1;
#else
    constexpr int16_t MATRIX_WIDTH = 128;
    constexpr int16_t MATRIX_HEIGHT = 64;
    constexpr uint8_t TEXT_SIZE_LARGE = 4;
    constexpr uint8_t TEXT_SIZE_SMALL = 1;
#endif

// For backward compatibility with existing code that uses #define
#ifndef matrix_width
    #define matrix_width MATRIX_WIDTH
#endif
#ifndef matrix_height
    #define matrix_height MATRIX_HEIGHT
#endif

// Grid dimensions for Game of Life (bit-packed)
constexpr int16_t GRIDX = MATRIX_WIDTH;
constexpr int16_t GRIDX_BYTE = MATRIX_WIDTH / 8;
constexpr int16_t GRIDY = MATRIX_HEIGHT;

// UDP port
constexpr uint16_t UDP_PORT = 4210;
constexpr uint16_t UDP_REPLY_PORT = 4445;

// EEPROM addresses
constexpr int EEPROM_MODE_ADDR = 0;
constexpr int EEPROM_COUNTER_ADDR = 1;
constexpr int EEPROM_SIZE = 720;

// Default display draw time
constexpr uint8_t DEFAULT_DISPLAY_DRAW_TIME = 40;

// Home Assistant state update interval (ms)
constexpr unsigned long HA_STATE_UPDATE_INTERVAL = 30000;

#endif // CONFIG_H
