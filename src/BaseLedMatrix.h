
#ifndef _BASE_LED_MATRIX
#define _BASE_LED_MATRIX
#ifdef ESP8266
#define PxMATRIX_double_buffer true
#define double_buffer
#include <PxMatrix.h>
#else
#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>
#endif
class BaseLedMatrix {
    protected:
#ifdef ESP8266
        PxMATRIX *display;

#endif
#ifdef ESP32
        VirtualMatrixPanel *display;
#endif
        void clear();
        void showBuffer();
};
#endif