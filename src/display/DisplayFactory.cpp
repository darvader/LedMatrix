#include "DisplayFactory.h"

#ifdef ESP32
#include "DisplayESP32.h"

DisplayResources DisplayFactory::create() {
    // Pin definitions for ESP32 HUB75 - use struct directly to avoid macro conflicts
    HUB75_I2S_CFG::i2s_pins pins = {
        27,  // R1
        10,  // G1
        25,  // B1
        22,  // R2
        13,  // G2
        21,  // B2
        17,  // A
        5,   // B
        16,  // C
        23,  // D
        -1,  // E
        19,  // LAT
        32,  // OE
        15   // CLK
    };

    HUB75_I2S_CFG mxconfig(64, 32, 4, pins, HUB75_I2S_CFG::FM6126A);
    mxconfig.double_buff = true;
    mxconfig.clkphase = false;

    auto* dma = new MatrixPanel_I2S_DMA(mxconfig);
    if (!dma->begin()) {
        Serial.println(F("****** !KABOOM! I2S memory allocation failed ***********"));
    }

    auto* vDisplay = new VirtualMatrixPanel(*dma, 2, 2, 64, 32, CHAIN_BOTTOM_RIGHT_UP);
    dma->setBrightness8(100);

    DisplayResources res;
    res.dmaDisplay = dma;
    res.virtualDisplay = vDisplay;
    res.display = new DisplayESP32(vDisplay, dma);
    return res;
}

#endif // ESP32

#ifdef ESP8266
#include "DisplayESP8266.h"

DisplayResources DisplayFactory::create() {
    auto* px = new PxMATRIX(64, 32, 16, 2, 5, 4, 15, 12);
    px->begin(16);
    px->setFastUpdate(true);
    px->setDriverChip(FM6124);

    DisplayResources res;
    res.pxDisplay = px;
    res.display = new DisplayESP8266(px);
    return res;
}

#endif // ESP8266
