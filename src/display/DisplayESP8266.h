#ifndef DISPLAY_ESP8266_H
#define DISPLAY_ESP8266_H

#ifdef ESP8266

#include "IDisplay.h"
#define PxMATRIX_double_buffer true
#define double_buffer
#include <PxMatrix.h>
#include <cstdarg>

class DisplayESP8266 : public IDisplay {
public:
    explicit DisplayESP8266(PxMATRIX* pxDisplay);

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
    PxMATRIX* getPxDisplay() { return pxDisplay_; }

private:
    PxMATRIX* pxDisplay_;
};

#endif // ESP8266
#endif // DISPLAY_ESP8266_H
