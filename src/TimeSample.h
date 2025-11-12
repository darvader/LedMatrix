
#ifndef _TIME_SAMPLE
#define _TIME_SAMPE

#include <NTPClient.h>
#include <BaseLedMatrix.h>
#include <Counter.h>

#ifdef ESP32
    #define GRIDX 128
    #define GRIDX_BYTE 16
    #define GRIDY 64
#else
    #define GRIDX 64
    #define GRIDX_BYTE 8
    #define GRIDY 32
#endif


class TimeSample: public BaseLedMatrix
{
private:
    /* data */
    int xVals[360];
    int yVals[360];

    uint8_t  oldGrid[GRIDX_BYTE][GRIDY];
    uint8_t  grid[GRIDX_BYTE][GRIDY];
    uint8_t  newGrid[GRIDX_BYTE][GRIDY];
    int countAliveNeighbors(uint8_t  x, uint8_t  y);
    bool get(byte grid[GRIDX_BYTE][GRIDY], uint8_t  x, uint8_t  y);
    void set(byte grid[GRIDX_BYTE][GRIDY], uint8_t  x, uint8_t  y, bool value);
public:
    NTPClient *timeClient;
    Counter *counter;
    boolean initializedSnow = false;
    boolean initializedGOL = false;

#ifdef ESP8266
    TimeSample(PxMATRIX *display, NTPClient *timeClient, Counter *counter);
#endif
#ifdef ESP32
    TimeSample(VirtualMatrixPanel *display, NTPClient *timeClient, Counter *counter);
#endif
    ~TimeSample();

    void drawTimeWithBackground();
    void drawTime();
    void timeSample1();
    void timeSample2();
    void timeSample3();
    void timeSample4();
    void timePlasma();
    void timeEllipse();
    void timeGameOfLife();
    void timeSnow(bool colored);
    void timeStarWars();
};
#endif