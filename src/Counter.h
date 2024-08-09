#ifndef _COUNTER
#define _COUNTER

#include <BaseLedMatrix.h>
#include <NTPClient.h>
class Counter: public BaseLedMatrix
{
private:
    /* data */
public:
    NTPClient *timeClient;
    u_int16_t counter = 0;
#ifdef ESP8266
    Counter(PxMATRIX *display, NTPClient *timeClient);
#endif
#ifdef ESP32
    Counter(VirtualMatrixPanel *display, NTPClient *timeClient);
#endif
    ~Counter();

    void start();
    void show();
    void setCounter(u_int16_t counter);
    void inline drawTime();
    void runningDots();
};
#endif