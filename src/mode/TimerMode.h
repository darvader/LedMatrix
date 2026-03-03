#ifndef TIMER_MODE_H
#define TIMER_MODE_H

#include "IDisplayMode.h"
#include "../display/IDisplay.h"
#include "../util/Colors.h"
#include "../app/Config.h"

class TimerMode : public IDisplayMode {
public:
    explicit TimerMode(IDisplay* display);

    void init() override;
    void update() override;
    void cleanup() override;
    uint16_t frameDelayMs() const override { return 20; }

    // Timer controls
    void setTimer(uint16_t seconds);
    void start();
    void pause();

    // Stopwatch controls
    void stopWatch();
    void stopWatchStart();
    void stopWatchStop();

private:
    enum SubMode { TIMER_MODE, STOPWATCH_MODE };

    void showTimer();
    void showStopWatch();

    IDisplay* display_;
    SubMode subMode_;
    double elapsed_;
    bool started_;
    unsigned long startTime_;
    uint16_t timerDuration_;
    unsigned long blinkingStart_;
};

#endif // TIMER_MODE_H
