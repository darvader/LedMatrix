#include <PxMatrix.h>

class Timer
{
private:
    int mode = 0;
    float elapsed = 0.0f;
    boolean started = false;
    long startTime = millis();
    PxMATRIX *display;

    /* data */
public:
    uint16_t timer = 110;

    Timer(PxMATRIX *display);
    ~Timer();

    void start();
    void pause();
    void setTimer(uint16_t timer);
    void stopWatch();
    void stopWatchStart();
    void stopWatchStop();
    void show();
    void showTimer();
    void showStopWatch();
};

