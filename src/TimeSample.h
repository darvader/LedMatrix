#include <NTPClient.h>
#include <BaseLedMatrix.h>

class TimeSample: public BaseLedMatrix
{
private:
    /* data */
public:
    NTPClient *timeClient;
    boolean initializedSnow = false;

#ifdef ESP8266
    TimeSample(PxMATRIX *display, NTPClient *timeClient);
#endif
#ifdef ESP32
    TimeSample(VirtualMatrixPanel *display, NTPClient *timeClient);
#endif
    ~TimeSample();

    void drawTimeWithBackground();
    void drawTime();
    void timeSample1();
    void timeSample2();
    void timeSample3();
    void timeSample4();
    void timePlasma();
    void timeSnow(bool colored);
};

