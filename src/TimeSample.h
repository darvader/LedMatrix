#include <NTPClient.h>
#include <BaseLedMatrix.h>

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
#ifdef ESP32
    uint8_t  oldGrid[GRIDX_BYTE][GRIDY];
    uint8_t  grid[GRIDX_BYTE][GRIDY];
    uint8_t  newGrid[GRIDX_BYTE][GRIDY];
    int countAliveNeighbors(uint8_t  x, uint8_t  y);
    bool get(byte grid[GRIDX_BYTE][GRIDY], uint8_t  x, uint8_t  y);
    void set(byte grid[GRIDX_BYTE][GRIDY], uint8_t  x, uint8_t  y, bool value);
#endif
public:
    NTPClient *timeClient;
    boolean initializedSnow = false;
    boolean initializedGOL = false;

#ifdef ESP8266
    TimeSample(PxMATRIX *display, NTPClient *timeClient);
#endif
#ifdef ESP32
    TimeSample(VirtualMatrixPanel *display, NTPClient *timeClient);
    void timeGameOfLife();
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

