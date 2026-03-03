#ifndef DISPLAY_ESP32_H
#define DISPLAY_ESP32_H

#ifdef ESP32

#include "IDisplay.h"
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>
#include <cstdarg>

class DisplayESP32 : public IDisplay {
public:
    DisplayESP32(VirtualMatrixPanel* vDisplay, MatrixPanel_I2S_DMA* dmaDisplay);

    void drawPixel(int16_t x, int16_t y, uint16_t color) override;
    void drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b) override;
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) override;
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) override;
    void fillScreen(uint16_t color) override;
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) override;
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;

    void setCursor(int16_t x, int16_t y) override;
    void setTextColor(uint16_t color) override;
    void setTextSize(uint8_t size) override;
    void setTextWrap(bool wrap) override;
    size_t print(const char* text) override;
    size_t print(int value) override;
    void printf(const char* fmt, ...) override;
    void setFont(const void* font = nullptr) override;

    uint16_t color565(uint8_t r, uint8_t g, uint8_t b) override;
    uint16_t color444(uint8_t r, uint8_t g, uint8_t b) override;

    int16_t width() const override;
    int16_t height() const override;

    void showBuffer() override;
    void clearScreen() override;

    void setBrightness(uint8_t brightness) override;

    // Access raw display for platform-specific init
    VirtualMatrixPanel* getVirtualDisplay() { return vDisplay_; }
    MatrixPanel_I2S_DMA* getDMADisplay() { return dmaDisplay_; }

private:
    VirtualMatrixPanel* vDisplay_;
    MatrixPanel_I2S_DMA* dmaDisplay_;
};

#endif // ESP32
#endif // DISPLAY_ESP32_H
