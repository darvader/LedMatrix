#ifdef ESP32

#include "DisplayESP32.h"

DisplayESP32::DisplayESP32(VirtualMatrixPanel* vDisplay, MatrixPanel_I2S_DMA* dmaDisplay)
    : vDisplay_(vDisplay), dmaDisplay_(dmaDisplay) {}

void DisplayESP32::drawPixel(int16_t x, int16_t y, uint16_t color) {
    vDisplay_->drawPixel(x, y, color);
}

void DisplayESP32::drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b) {
    vDisplay_->drawPixelRGB888(x, y, r, g, b);
}

void DisplayESP32::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    vDisplay_->drawLine(x0, y0, x1, y1, color);
}

void DisplayESP32::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    vDisplay_->fillRect(x, y, w, h, color);
}

void DisplayESP32::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    vDisplay_->fillTriangle(x0, y0, x1, y1, x2, y2, color);
}

void DisplayESP32::fillScreen(uint16_t color) {
    vDisplay_->fillScreen(color);
}

void DisplayESP32::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    vDisplay_->drawFastVLine(x, y, h, color);
}

void DisplayESP32::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    vDisplay_->fillCircle(x0, y0, r, color);
}

void DisplayESP32::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    vDisplay_->drawRect(x, y, w, h, color);
}

void DisplayESP32::setCursor(int16_t x, int16_t y) {
    vDisplay_->setCursor(x, y);
}

void DisplayESP32::setTextColor(uint16_t color) {
    vDisplay_->setTextColor(color);
}

void DisplayESP32::setTextSize(uint8_t size) {
    vDisplay_->setTextSize(size);
}

void DisplayESP32::setTextWrap(bool wrap) {
    vDisplay_->setTextWrap(wrap);
}

size_t DisplayESP32::print(const char* text) {
    return vDisplay_->print(text);
}

size_t DisplayESP32::print(int value) {
    return vDisplay_->print(value);
}

void DisplayESP32::printf(const char* fmt, ...) {
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    vDisplay_->print(buf);
}

void DisplayESP32::setFont(const void* font) {
    if (font) {
        vDisplay_->setFont((const GFXfont*)font);
    } else {
        vDisplay_->setFont();
    }
}

uint16_t DisplayESP32::color565(uint8_t r, uint8_t g, uint8_t b) {
    return vDisplay_->color565(r, g, b);
}

uint16_t DisplayESP32::color444(uint8_t r, uint8_t g, uint8_t b) {
    return vDisplay_->color444(r, g, b);
}

int16_t DisplayESP32::width() const {
    return vDisplay_->width();
}

int16_t DisplayESP32::height() const {
    return vDisplay_->height();
}

void DisplayESP32::showBuffer() {
    vDisplay_->flipDMABuffer();
    if (dmaDisplay_->calculated_refresh_rate > 0) {
        delay(1000 / dmaDisplay_->calculated_refresh_rate);
    } else {
        delay(20);
    }
}

void DisplayESP32::clearScreen() {
    vDisplay_->clearScreen();
}

void DisplayESP32::setBrightness(uint8_t brightness) {
    dmaDisplay_->setBrightness8(brightness);
}

#endif // ESP32
