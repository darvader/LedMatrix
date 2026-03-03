#ifndef DISPLAY_FACTORY_H
#define DISPLAY_FACTORY_H

#include "IDisplay.h"

#ifdef ESP32
class MatrixPanel_I2S_DMA;
class VirtualMatrixPanel;
#endif

struct DisplayResources {
    IDisplay* display;
#ifdef ESP32
    MatrixPanel_I2S_DMA* dmaDisplay;
    VirtualMatrixPanel* virtualDisplay;
#endif
#ifdef ESP8266
    void* pxDisplay; // PxMATRIX*
#endif
};

class DisplayFactory {
public:
    static DisplayResources create();
};

#endif // DISPLAY_FACTORY_H
