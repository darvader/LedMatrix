#ifndef _COUNTER
#define _COUNTER

#include <BaseLedMatrix.h>
#include <NTPClient.h>
class Counter: public BaseLedMatrix
{

struct runner_t {
  float speed;
  float distance;
  float position;
  float radius;
  uint8_t r, g, b;
};

private:
    void *runners = nullptr;
    bool initializedRunners = false;
    void freeRunners();
    void initializeRunners();
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
    void drawRunners();
    void createRunner(runner_t *runner);
    void freeAllResources();
};
#endif