#ifndef PLASMA_MODE_H
#define PLASMA_MODE_H

#include "IDisplayMode.h"
#include "../display/IDisplay.h"
#include "../time/TimeService.h"
#include "../util/TimeOverlay.h"
#include <FastLED.h>

class PlasmaMode : public IDisplayMode {
public:
    PlasmaMode(IDisplay* display, TimeService* timeService);

    void init() override;
    void update() override;
    void cleanup() override;
    uint16_t frameDelayMs() const override { return 20; }

private:
    IDisplay* display_;
    TimeService* timeService_;
    TimeOverlay timeOverlay_;

    uint16_t timeCounter_;
    uint16_t cycles_;
    uint16_t fps_;
    unsigned long fpsTimer_;
    CRGBPalette16 currentPalette_;
};

#endif // PLASMA_MODE_H
