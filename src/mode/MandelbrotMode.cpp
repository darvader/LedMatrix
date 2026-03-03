#include "MandelbrotMode.h"
#include <math.h>

const float MandelbrotMode::TARGETS[][2] = {
    { -0.7463f,  0.1102f },  // Seahorse valley
    { -0.1011f,  0.9563f },  // Spiral
    { -1.7497f,  0.0000f },  // Mini-Mandelbrot
    { -0.1700f,  1.0410f },  // Lightning
};

MandelbrotMode::MandelbrotMode(IDisplay* display, TimeService* timeService)
    : display_(display), timeOverlay_(display, timeService), zoom_(1.0),
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

// Float version kept for tests
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

// Cardioid: q*(q + (cr - 0.25)) <= 0.25*ci^2  where q = (cr-0.25)^2 + ci^2
// Period-2 bulb: (cr + 1)^2 + ci^2 <= 1/16
// All in Q7.25 fixed-point.
bool MandelbrotMode::inCardioidOrBulb(int32_t cr, int32_t ci) {
    // Constants in Q7.25
    static constexpr int32_t QUARTER = 1 << (FRAC - 2);       // 0.25
    static constexpr int32_t ONE = 1 << FRAC;                  // 1.0
    static constexpr int32_t SIXTEENTH = 1 << (FRAC - 4);     // 0.0625

    // Cardioid check
    int32_t cr_shifted = cr - QUARTER;  // cr - 0.25
    int64_t ci2 = (int64_t)ci * ci;     // ci^2 (unshifted, Q14.50)
    int64_t q_wide = (int64_t)cr_shifted * cr_shifted + ci2;  // Q14.50
    int32_t q = (int32_t)(q_wide >> FRAC);  // back to Q7.25
    // q * (q + cr_shifted) <= 0.25 * ci^2
    int64_t lhs = (int64_t)q * (q + cr_shifted);  // Q14.50
    int64_t rhs = ci2 >> 2;                         // 0.25 * ci^2 in Q14.50
    if (lhs <= rhs) return true;

    // Period-2 bulb check: (cr+1)^2 + ci^2 <= 1/16
    int32_t cr1 = cr + ONE;
    int64_t bulb = (int64_t)cr1 * cr1 + ci2;  // Q14.50
    int64_t bulb_limit = (int64_t)SIXTEENTH << FRAC;  // 1/16 in Q14.50
    if (bulb <= bulb_limit) return true;

    return false;
}

// Fixed-point Q7.25 inner loop with periodicity detection.
// Range ±64, precision ~3e-8. Safe for zoom up to ~500x.
int MandelbrotMode::computeIterationsFixed(int32_t cr, int32_t ci, int maxIter) {
    // Skip points known to be inside the set
    if (inCardioidOrBulb(cr, ci)) return maxIter;

    int32_t x = 0, y = 0;

    // Periodicity detection (Brent's algorithm variant)
    int32_t ox = 0, oy = 0;     // saved orbit point
    int period = 1;
    int checkAt = 1;             // next iteration to double the period

    // Bailout threshold: 4.0 in Q7.25
    static constexpr int32_t FOUR = 4 << FRAC;

    for (int i = 0; i < maxIter; i++) {
        // x^2 and y^2 via 64-bit multiply, result back in Q7.25
        int64_t x2_64 = (int64_t)x * x;
        int64_t y2_64 = (int64_t)y * y;

        // Bailout check in 64-bit to avoid overflow
        if ((x2_64 + y2_64) >> FRAC > FOUR) return i;

        int32_t x2 = (int32_t)(x2_64 >> FRAC);
        int32_t y2 = (int32_t)(y2_64 >> FRAC);
        int32_t xy = (int32_t)(((int64_t)x * y) >> FRAC);

        x = x2 - y2 + cr;
        y = 2 * xy + ci;

        // Periodicity check: if orbit returns to a saved point, it's in the set
        if (x == ox && y == oy) return maxIter;

        if (--period == 0) {
            ox = x;
            oy = y;
            checkAt <<= 1;       // double the period window
            period = checkAt;
        }
    }
    return maxIter;
}

void MandelbrotMode::update() {
    display_->showBuffer();
    display_->clearScreen();

    float zoomProgress = (millis() - zoomStartTime_) / zoomDuration_;
    if (zoomProgress >= 1.0f) {
        currentTarget_ = (currentTarget_ + 1) % NUM_TARGETS;
        targetX_ = TARGETS[currentTarget_][0];
        targetY_ = TARGETS[currentTarget_][1];
        zoom_ = 1.0;
        zoomStartTime_ = millis();
        zoomProgress = 0.0f;
    }

    zoom_ = powf(targetZoom_, zoomProgress);

    int maxIteration = 50 + (int)(30.0f * log2f(zoom_ > 1.0f ? zoom_ : 1.0f));

    // Pre-convert view bounds to fixed-point
    float xmin = targetX_ - 2.0f / zoom_;
    float xmax = targetX_ + 2.0f / zoom_;
    float ymin = targetY_ - 1.0f / zoom_;
    float ymax = targetY_ + 1.0f / zoom_;

    int hxmax = MATRIX_WIDTH, hymax = MATRIX_HEIGHT;

    // Fixed-point coordinate stepping — no float in the pixel loop
    int32_t fxmin = toFixed(xmin);
    int32_t fymin = toFixed(ymin);
    int32_t fxstep = toFixed((xmax - xmin) / (hxmax - 1));
    int32_t fystep = toFixed((ymax - ymin) / (hymax - 1));

    for (int hx = 0; hx < hxmax; hx++) {
        if (hx % 4 == 0) yield();
        int32_t cr = fxmin + hx * fxstep;
        for (int hy = 0; hy < hymax; hy++) {
            int32_t ci = fymin + hy * fystep;

            int iteration = computeIterationsFixed(cr, ci, maxIteration);

            uint16_t color;
            if (iteration >= maxIteration) {
                color = myBLACK;
            } else {
                color = palette_[iteration % 256];
            }
            display_->drawPixel(hx, hy, color);
        }
    }

    timeOverlay_.drawTime();
}
