#include <NTPClient.h>
#include <BaseLedMatrix.h>

#define GRIDX 128
#define GRIDX_BYTE 16
#define GRIDY 64

class TimeSample: public BaseLedMatrix
{
private:
    /* data */
    uint8_t  oldGrid[GRIDX_BYTE][GRIDY];
    uint8_t  grid[GRIDX_BYTE][GRIDY];
    uint8_t  newGrid[GRIDX_BYTE][GRIDY];
    int countAliveNeighbors(uint8_t  x, uint8_t  y);
    bool get(byte grid[GRIDX_BYTE][GRIDY], uint8_t  x, uint8_t  y);
    void set(byte grid[GRIDX_BYTE][GRIDY], uint8_t  x, uint8_t  y, bool value);
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

