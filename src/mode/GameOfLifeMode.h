#ifndef GAME_OF_LIFE_MODE_H
#define GAME_OF_LIFE_MODE_H

#include "IDisplayMode.h"
#include "../display/IDisplay.h"
#include "../time/TimeService.h"
#include "../util/TimeOverlay.h"
#include "../app/Config.h"

class GameOfLifeMode : public IDisplayMode {
public:
    GameOfLifeMode(IDisplay* display, TimeService* timeService);

    void init() override;
    void update() override;
    void cleanup() override;
    uint16_t frameDelayMs() const override { return 30; }

    void resetInitialized() { initialized_ = false; }

    // Testable pure functions
    static int countAliveNeighbors(uint8_t** grid, uint8_t x, uint8_t y,
                                    int gridX, int gridY);
    static bool getCell(uint8_t** grid, uint8_t x, uint8_t y);
    static void setCell(uint8_t** grid, uint8_t x, uint8_t y, bool value);

private:
    void initializeGrids();
    void freeGrids();

    IDisplay* display_;
    TimeService* timeService_;
    TimeOverlay timeOverlay_;

    uint8_t** oldGrid_;
    uint8_t** grid_;
    uint8_t** newGrid_;

    bool initialized_;
    int golType_;
    unsigned long startTime_;
};

#endif // GAME_OF_LIFE_MODE_H
