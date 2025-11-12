
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
    int *xVals = nullptr;
    int *yVals = nullptr;

    uint8_t  **oldGrid = nullptr;
    uint8_t  **grid = nullptr;
    uint8_t  **newGrid = nullptr;
    
    void *stars = nullptr;  // void pointer to avoid struct declaration in header
    void *snowParticles = nullptr;  // void pointer for snow particles
    
    int countAliveNeighbors(uint8_t  x, uint8_t  y);
    bool get(byte **grid, uint8_t  x, uint8_t  y);
    void set(byte **grid, uint8_t  x, uint8_t  y, bool value);
    void initializeGOLGrids();
    void freeGOLGrids();
    void initializeCircleVals();
    void freeCircleVals();
    void freeStars();
    void freeSnow();
    void initializeSnow(bool colored);
public:
    NTPClient *timeClient;
    Counter *counter;
    boolean initializedSnow = false;
    boolean initializedGOL = false;
    
    void freeAllResources();  // Public method to free all allocated memory

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