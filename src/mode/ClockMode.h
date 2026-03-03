#ifndef CLOCK_MODE_H
#define CLOCK_MODE_H

#include "IDisplayMode.h"
#include "../display/IDisplay.h"
#include "../time/TimeService.h"
#include "../util/TimeOverlay.h"
#include "../util/Colors.h"
#include "../app/Config.h"

struct line_t {
    uint16_t color;
    int16_t x1, x2, y1, y2;
    double arc;
    double degree;
};

class ClockMode : public IDisplayMode {
public:
    ClockMode(IDisplay* display, TimeService* timeService, int variant);

    void init() override;
    void update() override;
    void cleanup() override;
    uint16_t frameDelayMs() const override;

private:
    void updateVariant1();
    void updateVariant2();
    void updateVariant3();
    void updateVariant4();

    void initCircleVals();
    void freeCircleVals();

    IDisplay* display_;
    TimeService* timeService_;
    TimeOverlay timeOverlay_;
    int variant_;

    // Variant 1 state
    int x1_, y1_, stepX_, stepY_;

    // Variant 2 state
    static const int V2_SIZE = 10;
    line_t v2Lines_[V2_SIZE];
    bool v2Initialized_;

    // Variant 3 state
    static const int V3_SIZE = 20;
    line_t v3Lines_[V3_SIZE];
    bool v3Initialized_;

    // Variant 4 state
    static const int V4_SIZE = 60;
    line_t v4Lines_[V4_SIZE];
    bool v4Initialized_;

    // Shared circle lookup tables
    int* xVals_;
    int* yVals_;
};

#endif // CLOCK_MODE_H
