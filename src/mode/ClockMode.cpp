#include "ClockMode.h"
#include <Adafruit_NeoPixel.h>

ClockMode::ClockMode(IDisplay* display, TimeService* timeService, int variant)
    : display_(display), timeService_(timeService),
      timeOverlay_(display, timeService), variant_(variant),
      x1_(0), y1_(0), stepX_(1), stepY_(1),
      v2Initialized_(false), v3Initialized_(false), v4Initialized_(false),
      xVals_(nullptr), yVals_(nullptr) {}

void ClockMode::init() {
    x1_ = 0;
    y1_ = 0;
    stepX_ = 1;
    stepY_ = 1;
    v2Initialized_ = false;
    v3Initialized_ = false;
    v4Initialized_ = false;
    if (variant_ >= 3) {
        initCircleVals();
    }
}

void ClockMode::update() {
    switch (variant_) {
        case 1: updateVariant1(); break;
        case 2: updateVariant2(); break;
        case 3: updateVariant3(); break;
        case 4: updateVariant4(); break;
    }
}

void ClockMode::cleanup() {
    freeCircleVals();
    v2Initialized_ = false;
    v3Initialized_ = false;
    v4Initialized_ = false;
}

uint16_t ClockMode::frameDelayMs() const {
    return variant_ == 1 ? 30 : 20;
}

void ClockMode::initCircleVals() {
    if (xVals_ == nullptr) {
        xVals_ = new int[360];
        yVals_ = new int[360];
    }
}

void ClockMode::freeCircleVals() {
    if (xVals_ != nullptr) {
        delete[] xVals_;
        xVals_ = nullptr;
    }
    if (yVals_ != nullptr) {
        delete[] yVals_;
        yVals_ = nullptr;
    }
}

void ClockMode::updateVariant1() {
    int w = display_->width() - 1;
    int h = display_->height() - 1;

    if (x1_ == w) stepX_ = -1;
    if (x1_ == 0) stepX_ = 1;
    if (y1_ == h) stepY_ = -1;
    if (y1_ == 0) stepY_ = 1;

    display_->showBuffer();
    display_->clearScreen();

    display_->drawLine(x1_, 0, w - x1_, h, myRED);
    display_->drawLine(w - x1_, 0, x1_, h, myBLUE);
    display_->drawLine(0, y1_, w, h - y1_, myGREEN);
    display_->drawLine(0, h - y1_, w, y1_, myYELLOW);

    timeOverlay_.drawTimeWithBackground();

    x1_ += stepX_;
    y1_ += stepY_;
}

void ClockMode::updateVariant2() {
    const byte width = MATRIX_WIDTH / 2;
    const byte height = MATRIX_HEIGHT / 2;
    byte radius = width + 5;

    if (!v2Initialized_) {
        for (int i = 0; i < V2_SIZE; i++) {
            v2Lines_[i].x1 = sin(i * PI / V2_SIZE) * radius + width;
            v2Lines_[i].y1 = cos(i * PI / V2_SIZE) * radius + height;
            v2Lines_[i].x2 = sin(i * PI / V2_SIZE + PI) * radius + width;
            v2Lines_[i].y2 = cos(i * PI / V2_SIZE + PI) * radius + height;
            v2Lines_[i].color = myCOLORS[i % 7 + 1];
            v2Lines_[i].arc = i * PI / V2_SIZE;
        }
        v2Initialized_ = true;
    }

    display_->showBuffer();
    display_->clearScreen();

    for (int i = 0; i < V2_SIZE; i++) {
        display_->drawLine(v2Lines_[i].x1, v2Lines_[i].y1, v2Lines_[i].x2, v2Lines_[i].y2, v2Lines_[i].color);
        v2Lines_[i].arc -= PI * 0.2 / 180;
        if (v2Lines_[i].arc < 2 * PI) v2Lines_[i].arc += 2 * PI;

        double arc = v2Lines_[i].arc;
        v2Lines_[i].x1 = sin(arc) * radius + width;
        v2Lines_[i].y1 = cos(arc) * radius + height;
        v2Lines_[i].x2 = sin(arc + PI) * radius + width;
        v2Lines_[i].y2 = cos(arc + PI) * radius + height;
    }
    timeOverlay_.drawTimeWithBackground();
}

void ClockMode::updateVariant3() {
    byte radius = MATRIX_WIDTH / 2 + 5;
    const int step = 360 / V3_SIZE;

    if (!v3Initialized_) {
        initCircleVals();
        for (int i = 0; i < 360; i++) {
            xVals_[i] = sin(i / 180.0 * PI) * radius + MATRIX_WIDTH / 2;
            yVals_[i] = cos(i / 180.0 * PI) * radius + MATRIX_HEIGHT / 2;
        }
        for (int i = 0; i < V3_SIZE; i++) {
            v3Lines_[i].x1 = xVals_[i * step];
            v3Lines_[i].y1 = yVals_[i * step];
            int step2 = (i + 1) * step;
            if (step2 >= 360) step2 -= 360;
            v3Lines_[i].x2 = xVals_[(i + 1) * step];
            v3Lines_[i].y2 = yVals_[(i + 1) * step];
            v3Lines_[i].color = myCOLORS[i % 7];
            v3Lines_[i].degree = i * step;
        }
        v3Initialized_ = true;
    }

    display_->showBuffer();
    display_->clearScreen();

    for (int i = 0; i < V3_SIZE; i++) {
        v3Lines_[i].degree -= 0.2;
        if (v3Lines_[i].degree < 0) v3Lines_[i].degree += 360;

        display_->fillTriangle(MATRIX_WIDTH / 2, MATRIX_HEIGHT / 2,
                               v3Lines_[i].x1, v3Lines_[i].y1,
                               v3Lines_[i].x2, v3Lines_[i].y2,
                               v3Lines_[i].color);

        int degree = v3Lines_[i].degree;
        v3Lines_[i].x1 = xVals_[degree];
        v3Lines_[i].y1 = yVals_[degree];
        int step2 = degree + step;
        if (step2 >= 360) step2 -= 360;
        v3Lines_[i].x2 = xVals_[step2];
        v3Lines_[i].y2 = yVals_[step2];
    }
    timeOverlay_.drawTimeWithBackground();
}

void ClockMode::updateVariant4() {
    timeService_->update();
    int w = display_->width();
    int h = display_->height();
    byte radius = h + 10;
    const int step = 360 / V4_SIZE;

    if (!v4Initialized_) {
        initCircleVals();
        for (int i = 0; i < 360; i++) {
            xVals_[i] = sin(i / 180.0 * PI) * radius + w / 2;
            yVals_[i] = cos(i / 180.0 * PI) * radius + h / 2;
        }
        for (int i = 0; i < V4_SIZE; i++) {
            v4Lines_[i].x1 = xVals_[i * step];
            v4Lines_[i].y1 = yVals_[i * step];
            int step2 = (i + 1) * step;
            if (step2 >= 360) step2 -= 360;
            v4Lines_[i].x2 = xVals_[(i + 1) * step];
            v4Lines_[i].y2 = yVals_[(i + 1) * step];
            uint32_t c32 = Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::ColorHSV(65536L / V4_SIZE * i));
            v4Lines_[i].color = display_->color565((c32 & 0xff0000) >> 16, (c32 & 0xff00) >> 8, c32 & 0xff);
            v4Lines_[i].degree = i * step;
        }
        v4Initialized_ = true;
    }

    display_->showBuffer();
    display_->clearScreen();

    for (int i = 0; i < V4_SIZE; i++) {
        v4Lines_[i].degree -= 2;
        if (v4Lines_[i].degree < 0) v4Lines_[i].degree += 360;

        display_->fillTriangle(w / 2, h / 2,
                               v4Lines_[i].x1, v4Lines_[i].y1,
                               v4Lines_[i].x2, v4Lines_[i].y2,
                               v4Lines_[i].color);

        int degree = v4Lines_[i].degree;
        v4Lines_[i].x1 = xVals_[degree];
        v4Lines_[i].y1 = yVals_[degree];
        int step2 = degree + step;
        if (step2 >= 360) step2 -= 360;
        v4Lines_[i].x2 = xVals_[step2];
        v4Lines_[i].y2 = yVals_[step2];
    }
    timeOverlay_.drawTimeWithBackground();
}
