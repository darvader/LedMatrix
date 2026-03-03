#include "StarWarsMode.h"

StarWarsMode::StarWarsMode(IDisplay* display, TimeService* timeService)
    : display_(display), timeOverlay_(display, timeService),
      stars_(nullptr), initialized_(false) {}

void StarWarsMode::init() {
    initialized_ = false;
}

void StarWarsMode::cleanup() {
    if (stars_ != nullptr) {
        delete[] stars_;
        stars_ = nullptr;
        initialized_ = false;
    }
}

void StarWarsMode::update() {
    int width = MATRIX_WIDTH;
    int height = MATRIX_HEIGHT;
    float centerX = width / 2.0;
    float centerY = height / 2.0;

    if (!initialized_) {
        if (stars_ == nullptr) {
            stars_ = new star_t[NUM_STARS];
        }
        for (int i = 0; i < NUM_STARS; i++) {
            stars_[i].x = random(-width, width);
            stars_[i].y = random(-height, height);
            stars_[i].z = random(1, width);
            stars_[i].vz = random(5, 15) / 10.0;
            stars_[i].brightness = random(128, 255);
        }
        initialized_ = true;
    }

    if (stars_ == nullptr) return;

    display_->showBuffer();
    display_->clearScreen();

    for (int i = 0; i < NUM_STARS; i++) {
#ifdef ESP8266
        if (i % 20 == 0) yield();
#endif
        stars_[i].z -= stars_[i].vz;

        if (stars_[i].z <= 0) {
            stars_[i].x = random(-width, width);
            stars_[i].y = random(-height, height);
            stars_[i].z = width;
            stars_[i].vz = random(5, 15) / 10.0;
            stars_[i].brightness = random(128, 255);
        }

        float k = width / stars_[i].z;
        int sx = (int)(stars_[i].x * k + centerX);
        int sy = (int)(stars_[i].y * k + centerY);

        float prevZ = stars_[i].z + stars_[i].vz;
        float prevK = width / prevZ;
        int px = (int)(stars_[i].x * prevK + centerX);
        int py = (int)(stars_[i].y * prevK + centerY);

        if (sx >= 0 && sx < width && sy >= 0 && sy < height) {
            uint8_t brightness = (uint8_t)(stars_[i].brightness * (1.0 - stars_[i].z / width));
            uint8_t r = brightness;
            uint8_t g = brightness;
            uint8_t b = min(255, brightness + 30);

            uint16_t color = display_->color565(r, g, b);

            if (stars_[i].vz > 0.8 && px >= 0 && px < width && py >= 0 && py < height) {
                display_->drawLine(px, py, sx, sy, color);
            } else {
                display_->drawPixel(sx, sy, color);
                if (stars_[i].z < width / 4) {
                    if (sx + 1 < width) display_->drawPixel(sx + 1, sy, color);
                    if (sy + 1 < height) display_->drawPixel(sx, sy + 1, color);
                }
            }
        }
    }

    timeOverlay_.drawTimeWithBackground();
}
