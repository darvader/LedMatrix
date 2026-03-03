#ifndef IDISPLAY_H
#define IDISPLAY_H

#include <Arduino.h>

class IDisplay {
public:
    virtual ~IDisplay() = default;

    // Drawing primitives
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;
    virtual void drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b) = 0;
    virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) = 0;
    virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) = 0;
    virtual void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) = 0;
    virtual void fillScreen(uint16_t color) = 0;
    virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) = 0;
    virtual void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) = 0;
    virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) = 0;

    // Text
    virtual void setCursor(int16_t x, int16_t y) = 0;
    virtual void setTextColor(uint16_t color) = 0;
    virtual void setTextSize(uint8_t size) = 0;
    virtual void setTextWrap(bool wrap) = 0;
    virtual size_t print(const char* text) = 0;
    virtual size_t print(int value) = 0;
    virtual void printf(const char* fmt, ...) = 0;
    virtual void setFont(const void* font = nullptr) = 0;

    // Color conversion
    virtual uint16_t color565(uint8_t r, uint8_t g, uint8_t b) = 0;
    virtual uint16_t color444(uint8_t r, uint8_t g, uint8_t b) = 0;

    // Display dimensions
    virtual int16_t width() const = 0;
    virtual int16_t height() const = 0;

    // Buffer management
    virtual void showBuffer() = 0;
    virtual void clearScreen() = 0;

    // Brightness
    virtual void setBrightness(uint8_t brightness) = 0;
};

#endif // IDISPLAY_H
