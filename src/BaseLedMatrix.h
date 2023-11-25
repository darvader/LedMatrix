
#include <PxMatrix.h>
#ifndef _BASE_LED_MATRIX
#define _BASE_LED_MATRIX
class BaseLedMatrix {
    protected:
#ifdef ESP8266
        PxMATRIX *display;

#endif
#ifdef ESP32
#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>
        VirtualMatrixPanel *display;
#endif
        void clear();
        void showBuffer();
};
#endif