#ifndef MANDELBROT_MODE_H
#define MANDELBROT_MODE_H

#include "IDisplayMode.h"
#include "../display/IDisplay.h"
#include "../util/Colors.h"
#include "../app/Config.h"

class MandelbrotMode : public IDisplayMode {
public:
    explicit MandelbrotMode(IDisplay* display);

    void init() override;
    void update() override;
    void cleanup() override;
    uint16_t frameDelayMs() const override { return 0; }

    void resetZoom() { zoom_ = 1.0; zoomStartTime_ = millis(); }

    // Testable pure function
    static int computeIterations(float cr, float ci, int maxIterations);

private:
    void buildPalette();

    IDisplay* display_;

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
