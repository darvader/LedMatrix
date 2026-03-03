#ifndef COLORS_H
#define COLORS_H

#include <Arduino.h>

inline uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Standard colors
constexpr uint16_t myRED     = ((255 & 0xF8) << 8) | ((0 & 0xFC) << 3) | (0 >> 3);
constexpr uint16_t myGREEN   = ((0 & 0xF8) << 8) | ((255 & 0xFC) << 3) | (0 >> 3);
constexpr uint16_t myBLUE    = ((0 & 0xF8) << 8) | ((0 & 0xFC) << 3) | (255 >> 3);
constexpr uint16_t myWHITE   = ((255 & 0xF8) << 8) | ((255 & 0xFC) << 3) | (255 >> 3);
constexpr uint16_t myYELLOW  = ((255 & 0xF8) << 8) | ((255 & 0xFC) << 3) | (0 >> 3);
constexpr uint16_t myOrange  = ((255 & 0xF8) << 8) | ((127 & 0xFC) << 3) | (0 >> 3);
constexpr uint16_t myCYAN    = ((0 & 0xF8) << 8) | ((255 & 0xFC) << 3) | (255 >> 3);
constexpr uint16_t myMAGENTA = ((255 & 0xF8) << 8) | ((0 & 0xFC) << 3) | (255 >> 3);
constexpr uint16_t myBLACK   = 0;
constexpr uint16_t timeoutColor = ((0 & 0xF8) << 8) | ((50 & 0xFC) << 3) | (50 >> 3);

constexpr uint16_t myCOLORS[9] = {myBLACK, myRED, myGREEN, myBLUE, myOrange, myYELLOW, myCYAN, myMAGENTA, myWHITE};

#endif // COLORS_H
