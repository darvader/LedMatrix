#include <NTPClient.h>
#include <BaseLedMatrix.h>

#define GRIDX 128
#define GRIDY 64

class TimeSample: public BaseLedMatrix
{
private:
    /* data */
    bool grid[GRIDX][GRIDY];
    bool newGrid[GRIDX][GRIDY];
    int countAliveNeighbors(int x, int y);
public:
    NTPClient *timeClient;
    boolean initializedSnow = false;
    boolean initializedGOL = false;

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
    void timeGameOfLife();
    void timeSnow(bool colored);
};

