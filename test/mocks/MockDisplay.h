#ifndef MOCK_DISPLAY_H
#define MOCK_DISPLAY_H

#include "../../src/display/IDisplay.h"
#include <vector>
#include <string>
#include <cstdarg>
#include <cstdio>

struct DrawCall {
    enum Type { PIXEL, PIXEL_RGB, LINE, FILL_RECT, FILL_TRI, FILL_SCREEN,
                FAST_VLINE, FILL_CIRCLE, DRAW_RECT, TEXT, CLEAR, SHOW_BUFFER };
    Type type;
    int16_t x, y, x1, y1, x2, y2, w, h, r;
    uint16_t color;
    uint8_t red, green, blue;
    std::string text;
};

class MockDisplay : public IDisplay {
public:
    MockDisplay(int16_t w = 128, int16_t h = 64) : w_(w), h_(h), brightness_(100) {}

    std::vector<DrawCall> calls;

    void clearCalls() { calls.clear(); }

    void drawPixel(int16_t x, int16_t y, uint16_t color) override {
        calls.push_back({DrawCall::PIXEL, x, y, 0, 0, 0, 0, 0, 0, 0, color});
    }
    void drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b) override {
        DrawCall c = {DrawCall::PIXEL_RGB, x, y};
        c.red = r; c.green = g; c.blue = b;
        calls.push_back(c);
    }
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) override {
        calls.push_back({DrawCall::LINE, x0, y0, x1, y1, 0, 0, 0, 0, 0, color});
    }
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override {
        calls.push_back({DrawCall::FILL_RECT, x, y, 0, 0, 0, 0, w, h, 0, color});
    }
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) override {
        calls.push_back({DrawCall::FILL_TRI, x0, y0, x1, y1, x2, y2, 0, 0, 0, color});
    }
    void fillScreen(uint16_t color) override {
        calls.push_back({DrawCall::FILL_SCREEN, 0, 0, 0, 0, 0, 0, 0, 0, 0, color});
    }
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override {
        calls.push_back({DrawCall::FAST_VLINE, x, y, 0, 0, 0, 0, 0, h, 0, color});
    }
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) override {
        calls.push_back({DrawCall::FILL_CIRCLE, x0, y0, 0, 0, 0, 0, 0, 0, r, color});
    }
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override {
        calls.push_back({DrawCall::DRAW_RECT, x, y, 0, 0, 0, 0, w, h, 0, color});
    }
    void setCursor(int16_t x, int16_t y) override { cursorX_ = x; cursorY_ = y; }
    void setTextColor(uint16_t color) override { textColor_ = color; }
    void setTextSize(uint8_t size) override { textSize_ = size; }
    void setTextWrap(bool wrap) override { textWrap_ = wrap; }

    size_t print(const char* text) override {
        DrawCall c = {DrawCall::TEXT};
        c.text = text;
        calls.push_back(c);
        return strlen(text);
    }
    size_t print(int value) override {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d", value);
        return print(buf);
    }
    void printf(const char* fmt, ...) override {
        char buf[128];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        print(buf);
    }
    void setFont(const void* font = nullptr) override {}

    uint16_t color565(uint8_t r, uint8_t g, uint8_t b) override {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }
    uint16_t color444(uint8_t r, uint8_t g, uint8_t b) override {
        return ((r & 0xF) << 8) | ((g & 0xF) << 4) | (b & 0xF);
    }

    int16_t width() const override { return w_; }
    int16_t height() const override { return h_; }

    void showBuffer() override {
        calls.push_back({DrawCall::SHOW_BUFFER});
    }
    void clearScreen() override {
        calls.push_back({DrawCall::CLEAR});
    }

    void setBrightness(uint8_t brightness) override { brightness_ = brightness; }
    uint8_t getBrightness() const { return brightness_; }

private:
    int16_t w_, h_;
    int16_t cursorX_ = 0, cursorY_ = 0;
    uint16_t textColor_ = 0;
    uint8_t textSize_ = 1;
    bool textWrap_ = false;
    uint8_t brightness_;
};

#endif // MOCK_DISPLAY_H
