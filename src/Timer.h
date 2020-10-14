#include <PxMatrix.h>

class Timer
{
private:
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
    void show();
};
