#ifndef _COUNTER
#define _COUNTER

#include <BaseLedMatrix.h>
class Counter: public BaseLedMatrix
{
private:
    /* data */
public:
    u_int16_t counter = 0;
#ifdef ESP8266
    Counter(PxMATRIX *display);
#endif
#ifdef ESP32
    Counter(VirtualMatrixPanel *display);
#endif
    ~Counter();

    void start();
    void show();
    void setCounter(u_int16_t counter);
};
#endif