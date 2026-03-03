#ifndef MANDELBROT_MODE_H
#define MANDELBROT_MODE_H

#include "IDisplayMode.h"
#include "../display/IDisplay.h"
#include "../util/Colors.h"
#include "../util/TimeOverlay.h"
#include "../app/Config.h"
#include <stdint.h>

class TimeService;

class MandelbrotMode : public IDisplayMode {
public:
    MandelbrotMode(IDisplay* display, TimeService* timeService);

    void init() override;
    void update() override;
    void cleanup() override;
    uint16_t frameDelayMs() const override { return 1; }

    void resetZoom() { zoom_ = 1.0; zoomStartTime_ = millis(); }

    // Testable pure function (float, kept for tests)
    static int computeIterations(float cr, float ci, int maxIterations);

    // Fixed-point Q7.25 inner loop — the fast path
    static constexpr int FRAC = 25;
    static int computeIterationsFixed(int32_t cr, int32_t ci, int maxIter);

private:
    void buildPalette();
    static inline int32_t toFixed(float f) { return (int32_t)(f * (1 << FRAC)); }
    static inline int32_t fmul(int32_t a, int32_t b) {
        return (int32_t)(((int64_t)a * b) >> FRAC);
    }
    static bool inCardioidOrBulb(int32_t cr, int32_t ci);

    IDisplay* display_;
    TimeOverlay timeOverlay_;

    float zoom_;
    float targetX_;
    float targetY_;
    unsigned long zoomStartTime_;
    float zoomDuration_;
    float targetZoom_;
    int currentTarget_;

    uint16_t palette_[256];

    static constexpr int NUM_TARGETS = 4;
    static const float TARGETS[][2];
};

#endif // MANDELBROT_MODE_H
