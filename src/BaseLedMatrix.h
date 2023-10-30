
#ifndef _BASE_LED_MATRIX
#define _BASE_LED_MATRIX
#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>
class BaseLedMatrix {
    protected:
#ifdef ESP8266
#include <PxMatrix.h>
        PxMATRIX *display;

#endif
#ifdef ESP32
        VirtualMatrixPanel *display;
#endif
        void clear();
        void showBuffer();
};
#endif