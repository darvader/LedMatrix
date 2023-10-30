#include <BaseLedMatrix.h>
class Timer: public BaseLedMatrix
{
private:
    int mode = 0;
    double elapsed = 0.0f;
    boolean started = false;
    long startTime = millis();

    /* data */
public:
    uint16_t timer = 110;
#ifdef ESP8266
    Timer(PxMATRIX *display);
#endif
#ifdef ESP32
    Timer(VirtualMatrixPanel *display);
#endif
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

