#include "GameOfLifeMode.h"
#include "../util/Colors.h"

GameOfLifeMode::GameOfLifeMode(IDisplay* display, TimeService* timeService)
    : display_(display), timeService_(timeService),
      timeOverlay_(display, timeService),
      oldGrid_(nullptr), grid_(nullptr), newGrid_(nullptr),
      initialized_(false), golType_(-1), startTime_(0) {}

void GameOfLifeMode::init() {
    initialized_ = false;
}

void GameOfLifeMode::cleanup() {
    freeGrids();
}

void GameOfLifeMode::initializeGrids() {
    if (oldGrid_ == nullptr) {
        oldGrid_ = new uint8_t*[GRIDX_BYTE];
        grid_ = new uint8_t*[GRIDX_BYTE];
        newGrid_ = new uint8_t*[GRIDX_BYTE];

        for (int i = 0; i < GRIDX_BYTE; i++) {
            oldGrid_[i] = new uint8_t[GRIDY];
            grid_[i] = new uint8_t[GRIDY];
            newGrid_[i] = new uint8_t[GRIDY];
            for (int j = 0; j < GRIDY; j++) {
                oldGrid_[i][j] = 0;
                grid_[i][j] = 0;
                newGrid_[i][j] = 0;
            }
        }
    }
}

void GameOfLifeMode::freeGrids() {
    if (oldGrid_ != nullptr) {
        for (int i = 0; i < GRIDX_BYTE; i++) {
            delete[] oldGrid_[i];
            delete[] grid_[i];
            delete[] newGrid_[i];
        }
        delete[] oldGrid_;
        delete[] grid_;
        delete[] newGrid_;
        oldGrid_ = nullptr;
        grid_ = nullptr;
        newGrid_ = nullptr;
    }
    initialized_ = false;
}

bool GameOfLifeMode::getCell(uint8_t** grid, uint8_t x, uint8_t y) {
    uint8_t xByte = x / 8;
    uint8_t xBit = x % 8;
    return (grid[xByte][y] & (1 << xBit)) != 0;
}

void GameOfLifeMode::setCell(uint8_t** grid, uint8_t x, uint8_t y, bool value) {
    uint8_t byteIndex = x / 8;
    uint8_t bitIndex = x % 8;
    if (value) {
        grid[byteIndex][y] |= (1 << bitIndex);
    } else {
        grid[byteIndex][y] &= ~(1 << bitIndex);
    }
}

int GameOfLifeMode::countAliveNeighbors(uint8_t** grid, uint8_t x, uint8_t y,
                                         int gridX, int gridY) {
    uint8_t count = 0;
    for (int8_t i = -1; i <= 1; i++) {
        for (int8_t j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            uint8_t nx = (x + i + gridX) % gridX;
            uint8_t ny = (y + j + gridY) % gridY;
            count += getCell(grid, nx, ny) ? 1 : 0;
        }
    }
    return count;
}

void GameOfLifeMode::update() {
    if (!initialized_) {
        initializeGrids();
        for (int y = 0; y < GRIDY; y++) {
            for (int x = 0; x < GRIDX; x++) {
                setCell(grid_, x, y, (random(2) == 0));
            }
        }
        golType_++;
        if (golType_ > 2) golType_ = 0;
        startTime_ = millis();
        initialized_ = true;
    }

    display_->showBuffer();
    display_->clearScreen();

    int changes = 0;
    for (int y = 0; y < GRIDY; y++) {
#ifdef ESP8266
        yield();
#endif
        for (int x = 0; x < GRIDX; x++) {
            int aliveNeighbors = countAliveNeighbors(grid_, x, y, GRIDX, GRIDY);
            if (getCell(grid_, x, y)) {
                switch (golType_) {
                    case 0: setCell(newGrid_, x, y, aliveNeighbors == 2 || aliveNeighbors == 3); break;
                    case 1: setCell(newGrid_, x, y, aliveNeighbors == 2 || aliveNeighbors == 3); break;
                    case 2: setCell(newGrid_, x, y, aliveNeighbors >= 1 || aliveNeighbors <= 6); break;
                    case 3: setCell(newGrid_, x, y, aliveNeighbors == 3); break;
                    case 4: setCell(newGrid_, x, y, aliveNeighbors == 5); break;
                    case 5: setCell(newGrid_, x, y, aliveNeighbors >= 3 && aliveNeighbors <= 6); break;
                    default: setCell(newGrid_, x, y, aliveNeighbors == 2 || aliveNeighbors == 3); break;
                }
            } else {
                switch (golType_) {
                    case 0: setCell(newGrid_, x, y, aliveNeighbors == 3); break;
                    case 1: setCell(newGrid_, x, y, aliveNeighbors == 3 || aliveNeighbors == 6); break;
                    case 2: setCell(newGrid_, x, y, aliveNeighbors == 3); break;
                    case 3: setCell(newGrid_, x, y, aliveNeighbors == 3 || aliveNeighbors == 4); break;
                    case 4: setCell(newGrid_, x, y, aliveNeighbors == 3); break;
                    case 5: setCell(newGrid_, x, y, aliveNeighbors >= 3 && aliveNeighbors <= 8); break;
                    default: setCell(newGrid_, x, y, aliveNeighbors == 2 || aliveNeighbors == 3); break;
                }
            }
            if (getCell(oldGrid_, x, y) != getCell(newGrid_, x, y)) {
                changes++;
            }
        }
    }

#ifdef ESP32
    if ((changes < (GRIDX * GRIDY) / 4000) ||
#else
    if ((changes < (GRIDX * GRIDY) / 1000) ||
#endif
        (millis() - startTime_ > 240000)) {
        initialized_ = false;
    }

    // Swap grids
    for (int y = 0; y < GRIDY; y++) {
#ifdef ESP8266
        yield();
#endif
        for (int x = 0; x < GRIDX_BYTE; x++) {
            oldGrid_[x][y] = grid_[x][y];
            grid_[x][y] = newGrid_[x][y];
        }
    }

    // Draw the grid
    for (int y = 0; y < GRIDY; y++) {
#ifdef ESP8266
        yield();
#endif
        for (int x = 0; x < GRIDX; x++) {
            display_->drawPixel(x, y, getCell(grid_, x, y) ? myCOLORS[countAliveNeighbors(grid_, x, y, GRIDX, GRIDY)] : myBLACK);
        }
    }

    timeOverlay_.drawTimeWithBackground();
}
