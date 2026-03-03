#ifndef COUNTER_MODE_H
#define COUNTER_MODE_H

#include "IDisplayMode.h"
#include "../display/IDisplay.h"
#include "../time/TimeService.h"
#include "../util/EllipseRenderer.h"
#include "../util/Colors.h"
#include "../app/Config.h"

class CounterMode : public IDisplayMode {
public:
    CounterMode(IDisplay* display, TimeService* timeService, EllipseRenderer* ellipseRenderer);

    void init() override;
    void update() override;
    void cleanup() override;
    uint16_t frameDelayMs() const override { return 20; }

    void setCounter(uint16_t value) { counter_ = value; }
    uint16_t getCounter() const { return counter_; }
    void resetCounter() { counter_ = 0; }

private:
    void drawTime();

    IDisplay* display_;
    TimeService* timeService_;
    EllipseRenderer* ellipseRenderer_;
    uint16_t counter_;
};

#endif // COUNTER_MODE_H
