#ifdef ESP8266

#include "DisplayESP8266.h"

DisplayESP8266::DisplayESP8266(PxMATRIX* pxDisplay)
    : pxDisplay_(pxDisplay) {}

void DisplayESP8266::drawPixel(int16_t x, int16_t y, uint16_t color) {
    pxDisplay_->drawPixel(x, y, color);
}

void DisplayESP8266::drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b) {
    pxDisplay_->drawPixelRGB888(x, y, r, g, b);
}

void DisplayESP8266::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    pxDisplay_->drawLine(x0, y0, x1, y1, color);
}

void DisplayESP8266::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    pxDisplay_->fillRect(x, y, w, h, color);
}

void DisplayESP8266::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    pxDisplay_->fillTriangle(x0, y0, x1, y1, x2, y2, color);
}

void DisplayESP8266::fillScreen(uint16_t color) {
    pxDisplay_->fillScreen(color);
}

void DisplayESP8266::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    pxDisplay_->drawFastVLine(x, y, h, color);
}

void DisplayESP8266::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    pxDisplay_->fillCircle(x0, y0, r, color);
}

void DisplayESP8266::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    pxDisplay_->drawRect(x, y, w, h, color);
}

void DisplayESP8266::setCursor(int16_t x, int16_t y) {
    pxDisplay_->setCursor(x, y);
}

void DisplayESP8266::setTextColor(uint16_t color) {
    pxDisplay_->setTextColor(color);
}

void DisplayESP8266::setTextSize(uint8_t size) {
    pxDisplay_->setTextSize(size);
}

void DisplayESP8266::setTextWrap(bool wrap) {
    pxDisplay_->setTextWrap(wrap);
}

size_t DisplayESP8266::print(const char* text) {
    return pxDisplay_->print(text);
}

size_t DisplayESP8266::print(int value) {
    return pxDisplay_->print(value);
}

void DisplayESP8266::printf(const char* fmt, ...) {
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    pxDisplay_->print(buf);
}

void DisplayESP8266::setFont(const void* font) {
    if (font) {
        pxDisplay_->setFont((const GFXfont*)font);
    } else {
        pxDisplay_->setFont();
    }
}

uint16_t DisplayESP8266::color565(uint8_t r, uint8_t g, uint8_t b) {
    return pxDisplay_->color565(r, g, b);
}

uint16_t DisplayESP8266::color444(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF) << 8) | ((g & 0xF) << 4) | (b & 0xF);
}

int16_t DisplayESP8266::width() const {
    return pxDisplay_->width();
}

int16_t DisplayESP8266::height() const {
    return pxDisplay_->height();
}

void DisplayESP8266::showBuffer() {
    pxDisplay_->showBuffer();
}

void DisplayESP8266::clearScreen() {
    pxDisplay_->clearDisplay();
}

void DisplayESP8266::setBrightness(uint8_t brightness) {
    // PxMATRIX doesn't have a direct brightness API
    // brightness is controlled via display_draw_time externally
}

#endif // ESP8266
