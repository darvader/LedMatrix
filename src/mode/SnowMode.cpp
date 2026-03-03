#include "SnowMode.h"
#include "../util/Colors.h"

SnowMode::SnowMode(IDisplay* display, TimeService* timeService, bool colored)
    : display_(display), timeService_(timeService),
      timeOverlay_(display, timeService), colored_(colored),
      initialized_(false), particles_(nullptr), numParticles_(0) {}

void SnowMode::init() {
    initialized_ = false;
}

void SnowMode::update() {
#ifdef ESP8266
    const int size = 1;
#else
    const int size = 4;
#endif
    numParticles_ = 60 * size;
    const int width = display_->width() - 1;
    const int height = display_->height() - 1;

    if (!initialized_) {
        initializeSnow();
    }

    display_->showBuffer();
    display_->clearScreen();

    for (int i = 0; i < numParticles_; i++) {
        snow_t* sn = &particles_[i];
        if (colored_) {
            uint8_t r = sn->r * sn->speed;
            uint8_t g = sn->g * sn->speed;
            uint8_t b = sn->b * sn->speed;
            display_->drawPixelRGB888(sn->x, sn->y, r, g, b);
        } else {
            display_->drawPixelRGB888(sn->x, sn->y, 0xff * sn->speed, 0xff * sn->speed, 0xff * sn->speed);
        }
        sn->y += sn->speed;
        if (sn->y > height) {
            sn->x = random(width);
            sn->y = 0;
            sn->speed = random(3, 10) / 10.0f;
        }
    }

    timeOverlay_.drawTime();
}

void SnowMode::cleanup() {
    if (particles_ != nullptr) {
        delete[] particles_;
        particles_ = nullptr;
        initialized_ = false;
    }
}

void SnowMode::initializeSnow() {
    cleanup();

#ifdef ESP8266
    const int size = 1;
#else
    const int size = 4;
#endif
    numParticles_ = 60 * size;
    const int width = display_->width() - 1;
    const int height = display_->height() - 1;

    particles_ = new snow_t[numParticles_];

    for (int i = 0; i < numParticles_; i++) {
        snow_t* snow = &particles_[i];
        snow->type = SNOW_SMALL;
        snow->x = random(width);
        snow->y = random(height);
        snow->speed = random(1, 10) / 10.0f;
        if (colored_) {
            snow->r = random(255);
            snow->b = random(255);
            snow->g = random(255);
        }
    }

    initialized_ = true;
}
