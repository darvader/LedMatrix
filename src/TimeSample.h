#include <PxMatrix.h>
#include <NTPClient.h>

class TimeSample
{
private:
    /* data */
public:
    PxMATRIX *display;
    NTPClient *timeClient;

    TimeSample(PxMATRIX *display, NTPClient *timeClient);
    ~TimeSample();

    void drawTimeWithBackground();
    void drawTime();
    void timeSample1();
    void timeSample2();
    void timeSample3();
    void timeSample4();
    void timePlasma();
    void timeSnow();
};

