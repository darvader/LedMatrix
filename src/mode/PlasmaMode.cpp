#include "PlasmaMode.h"

static CRGBPalette16 plasmaPalettes[] = {
    HeatColors_p, LavaColors_p, RainbowColors_p, RainbowStripeColors_p, CloudColors_p
};

PlasmaMode::PlasmaMode(IDisplay* display, TimeService* timeService)
    : display_(display), timeService_(timeService),
      timeOverlay_(display, timeService),
      timeCounter_(0), cycles_(0), fps_(0), fpsTimer_(0),
      currentPalette_(plasmaPalettes[0]) {}

void PlasmaMode::init() {
    timeCounter_ = 0;
    cycles_ = 0;
    fps_ = 0;
    fpsTimer_ = millis();
    currentPalette_ = plasmaPalettes[0];
}

void PlasmaMode::update() {
    int paneWidth = display_->width();
    int paneHeight = display_->height();

    display_->showBuffer();
    display_->clearScreen();

    for (int x = 0; x < paneWidth; x++) {
#ifdef ESP8266
        if (x % 4 == 0) yield();
#endif
        for (int y = 0; y < paneHeight; y++) {
            int16_t v = 0;
            uint8_t wibble = sin8(timeCounter_);
            v += sin16(x * wibble * 3 + timeCounter_);
            v += cos16(y * (128 - wibble) + timeCounter_);
            v += sin16(y * x * cos8(-timeCounter_) / 8);

            CRGB currentColor = ColorFromPalette(currentPalette_, (v >> 8) + 127);
            display_->drawPixelRGB888(x, y, currentColor.r, currentColor.g, currentColor.b);
        }
    }

    ++timeCounter_;
    ++cycles_;
    ++fps_;

    if (cycles_ >= 1024) {
        timeCounter_ = 0;
        cycles_ = 0;
        currentPalette_ = plasmaPalettes[random(0, sizeof(plasmaPalettes) / sizeof(plasmaPalettes[0]))];
    }

    if (fpsTimer_ + 5000 < millis()) {
        Serial.printf_P(PSTR("Effect fps: %d\n"), fps_ / 5);
        fpsTimer_ = millis();
        fps_ = 0;
    }
    timeOverlay_.drawTimeWithBackground();
}

void PlasmaMode::cleanup() {}
