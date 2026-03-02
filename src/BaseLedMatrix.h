
#ifndef _BASE_LED_MATRIX
#define _BASE_LED_MATRIX
#ifdef ESP8266
#define PxMATRIX_double_buffer true
#define double_buffer
#include <PxMatrix.h>
#else
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>
#endif
class BaseLedMatrix {
    protected:
#ifdef ESP8266
        PxMATRIX *display;
#endif
#ifdef ESP32
        VirtualMatrixPanel *display;
        MatrixPanel_I2S_DMA *dmaDisplay;
#endif
        void clear();
        void showBuffer();

    public:
#ifdef ESP32
        void setDMADisplay(MatrixPanel_I2S_DMA *dmaDisplay) { this->dmaDisplay = dmaDisplay; }
#endif
};
#endif