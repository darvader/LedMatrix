#ifndef TIME_OVERLAY_H
#define TIME_OVERLAY_H

#include "../display/IDisplay.h"
#include "../time/TimeService.h"
#include "../util/Colors.h"

class TimeOverlay {
public:
    TimeOverlay(IDisplay* display, TimeService* timeService)
        : display_(display), timeService_(timeService) {}

    void drawTime();
    void drawTimeWithBackground();

private:
    IDisplay* display_;
    TimeService* timeService_;
};

#endif // TIME_OVERLAY_H
