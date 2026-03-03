#ifndef ELLIPSE_RENDERER_H
#define ELLIPSE_RENDERER_H

#include "../display/IDisplay.h"
#include <Arduino.h>

struct runner_t {
    float speed;
    float distance;
    float position;
    float radius;
    uint8_t r, g, b;
};

class EllipseRenderer {
public:
    explicit EllipseRenderer(IDisplay* display);
    ~EllipseRenderer();

    void init();
    void cleanup();
    void drawRunners();

private:
    void createRunner(runner_t* runner);

    IDisplay* display_;
    runner_t* runners_;
    bool initialized_;
    static const int NUM_RUNNERS = 400;
};

#endif // ELLIPSE_RENDERER_H
