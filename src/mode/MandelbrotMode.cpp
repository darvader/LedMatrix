#include "MandelbrotMode.h"
#include <math.h>

const float MandelbrotMode::TARGETS[][2] = {
    { -0.7463f,  0.1102f },  // Seahorse valley
    { -0.1011f,  0.9563f },  // Spiral
    { -1.7497f,  0.0000f },  // Mini-Mandelbrot
    { -0.1700f,  1.0410f },  // Lightning
};

MandelbrotMode::MandelbrotMode(IDisplay* display)
    : display_(display), zoom_(1.0),
      targetX_(TARGETS[0][0]), targetY_(TARGETS[0][1]),
      zoomStartTime_(0), zoomDuration_(60000), targetZoom_(500.0),
      currentTarget_(0) {}

void MandelbrotMode::buildPalette() {
    for (int i = 0; i < 256; i++) {
        float h = fmod(i * 5.0f, 360.0f);
        float s = 0.85f;
        float v = (i < 32) ? (i / 32.0f) : 1.0f;

        float c = v * s;
        float x = c * (1.0f - fabsf(fmod(h / 60.0f, 2.0f) - 1.0f));
        float m = v - c;

        float r, g, b;
        if (h < 60)       { r = c; g = x; b = 0; }
        else if (h < 120) { r = x; g = c; b = 0; }
        else if (h < 180) { r = 0; g = c; b = x; }
        else if (h < 240) { r = 0; g = x; b = c; }
        else if (h < 300) { r = x; g = 0; b = c; }
        else              { r = c; g = 0; b = x; }

        uint8_t r8 = (uint8_t)((r + m) * 255);
        uint8_t g8 = (uint8_t)((g + m) * 255);
        uint8_t b8 = (uint8_t)((b + m) * 255);
        palette_[i] = display_->color565(r8, g8, b8);
    }
}

void MandelbrotMode::init() {
    zoom_ = 1.0;
    zoomStartTime_ = millis();
    buildPalette();
}

void MandelbrotMode::cleanup() {}

int MandelbrotMode::computeIterations(float cr, float ci, int maxIterations) {
    float x = 0.0, y = 0.0, xx;
    int iteration = 0;
    while ((x * x + y * y <= 4) && (iteration < maxIterations)) {
        xx = x * x - y * y + cr;
        y = 2.0 * x * y + ci;
        x = xx;
        iteration++;
    }
    return iteration;
}

void MandelbrotMode::update() {
    display_->showBuffer();
    display_->clearScreen();

    float zoomProgress = (millis() - zoomStartTime_) / zoomDuration_;
    if (zoomProgress >= 1.0f) {
        // Cycle to next target
        currentTarget_ = (currentTarget_ + 1) % NUM_TARGETS;
        targetX_ = TARGETS[currentTarget_][0];
        targetY_ = TARGETS[currentTarget_][1];
        zoom_ = 1.0;
        zoomStartTime_ = millis();
        zoomProgress = 0.0f;
    }

    // Exponential zoom: pow(targetZoom, progress)
    zoom_ = powf(targetZoom_, zoomProgress);

    // Adaptive max iterations
    int maxIteration = 50 + (int)(30.0f * log2f(zoom_ > 1.0f ? zoom_ : 1.0f));

    float xmin = targetX_ - 2.0f / zoom_;
    float xmax = targetX_ + 2.0f / zoom_;
    float ymin = targetY_ - 1.0f / zoom_;
    float ymax = targetY_ + 1.0f / zoom_;

    int hxmax = MATRIX_WIDTH, hymax = MATRIX_HEIGHT;

    float xr = (xmax - xmin) / (hxmax - 1);
    float yr = (ymax - ymin) / (hymax - 1);

    for (int hx = 0; hx < hxmax; hx++) {
        for (int hy = 0; hy < hymax; hy++) {
            float cr = xmin + (hx * xr);
            float ci = ymin + (hy * yr);

            int iteration = computeIterations(cr, ci, maxIteration);

            uint16_t color;
            if (iteration >= maxIteration) {
                color = myBLACK;
            } else {
                color = palette_[iteration % 256];
            }
            display_->drawPixel(hx, hy, color);
        }
    }
}
