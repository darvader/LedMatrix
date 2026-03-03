#include "MandelbrotMode.h"

MandelbrotMode::MandelbrotMode(IDisplay* display)
    : display_(display), zoom_(1.0), targetX_(-0.65), targetY_(0.0),
      zoomStartTime_(0), zoomDuration_(60000), targetZoom_(10.0) {}

void MandelbrotMode::init() {
    zoom_ = 1.0;
    zoomStartTime_ = millis();
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
    if ((millis() - zoomStartTime_) > zoomDuration_) zoomStartTime_ = millis();
    if (zoomProgress > 1.0) zoomProgress = 1.0;
    zoom_ = 1.0 + (targetZoom_ - 1.0) * zoomProgress;

    float xmin = targetX_ - 2.5 / zoom_;
    float xmax = targetX_ + 1.5 / zoom_;
    float ymin = targetY_ - 2.0 / zoom_;
    float ymax = targetY_ + 2.0 / zoom_;

    int hxmax = MATRIX_WIDTH, hymax = MATRIX_HEIGHT;
    int maxIteration = 1000;

    float xr = (xmax - xmin) / ((hxmax - 1) * zoom_);
    float yr = (ymax - ymin) / ((hymax - 1) * zoom_);

    for (int hx = 0; hx < hxmax; hx++) {
        for (int hy = 0; hy < hymax; hy++) {
            float cr = xmin + (hx * xr);
            float ci = ymin + (hy * yr);

            int iteration = computeIterations(cr, ci, maxIteration);

            uint16_t color;
            if (iteration >= maxIteration) {
                color = myBLACK;
            } else {
                color = display_->color565(iteration % 256, iteration % 128, iteration % 64);
            }
            display_->drawPixel(hx, hy, color);
        }
    }
}
