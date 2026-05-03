#include "GameOfLifeMode.h"
#include "../util/Colors.h"

namespace {
inline bool cellAt(uint8_t** grid, int x, int y) {
    return (grid[x >> 3][y] & (1 << (x & 7))) != 0;
}

inline uint8_t countBits(uint8_t value) {
    value = value - ((value >> 1) & 0x55);
    value = (value & 0x33) + ((value >> 2) & 0x33);
    return (value + (value >> 4)) & 0x0F;
}

inline uint8_t countNeighbors(uint8_t** grid, int x, int y, int gridX, int gridY) {
    const int left = (x == 0) ? gridX - 1 : x - 1;
    const int right = (x == gridX - 1) ? 0 : x + 1;
    const int up = (y == 0) ? gridY - 1 : y - 1;
    const int down = (y == gridY - 1) ? 0 : y + 1;

    return (cellAt(grid, left, up) ? 1 : 0) +
           (cellAt(grid, x, up) ? 1 : 0) +
           (cellAt(grid, right, up) ? 1 : 0) +
           (cellAt(grid, left, y) ? 1 : 0) +
           (cellAt(grid, right, y) ? 1 : 0) +
           (cellAt(grid, left, down) ? 1 : 0) +
           (cellAt(grid, x, down) ? 1 : 0) +
           (cellAt(grid, right, down) ? 1 : 0);
}

inline bool nextCellState(bool alive, uint8_t aliveNeighbors, int golType) {
    if (alive) {
        switch (golType) {
            case 0:
            case 1: return aliveNeighbors == 2 || aliveNeighbors == 3;
            case 2: return aliveNeighbors >= 1 || aliveNeighbors <= 6;
            case 3: return aliveNeighbors == 3;
            case 4: return aliveNeighbors == 5;
            case 5: return aliveNeighbors >= 3 && aliveNeighbors <= 6;
            default: return aliveNeighbors == 2 || aliveNeighbors == 3;
        }
    }

    switch (golType) {
        case 0: return aliveNeighbors == 3;
        case 1: return aliveNeighbors == 3 || aliveNeighbors == 6;
        case 2: return aliveNeighbors == 3;
        case 3: return aliveNeighbors == 3 || aliveNeighbors == 4;
        case 4: return aliveNeighbors == 3;
        case 5: return aliveNeighbors >= 3 && aliveNeighbors <= 8;
        default: return aliveNeighbors == 2 || aliveNeighbors == 3;
    }
}

inline uint8_t wave8(uint8_t phase) {
    phase &= 0x3F;
    return phase < 32 ? phase * 8 : (63 - phase) * 8;
}

inline uint16_t lifeColor(IDisplay* display, int x, int y, uint8_t aliveNeighbors,
                          uint16_t generation) {
    const uint8_t heat = aliveNeighbors * 24;
    const uint8_t drift = static_cast<uint8_t>(generation + x * 3 + y * 5);
    const uint8_t red = 48 + wave8(drift + heat);
    const uint8_t green = 40 + wave8(drift + 21 + heat / 2);
    const uint8_t blue = 56 + wave8(drift + 42 + (8 - aliveNeighbors) * 12);

    return display->color565(red, green, blue);
}
}

GameOfLifeMode::GameOfLifeMode(IDisplay* display, TimeService* timeService)
    : display_(display), timeService_(timeService),
      timeOverlay_(display, timeService),
      oldGrid_(nullptr), grid_(nullptr), newGrid_(nullptr),
      initialized_(false), golType_(-1), generation_(0), startTime_(0) {}

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
    return countNeighbors(grid, x, y, gridX, gridY);
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
        generation_ = 0;
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
        for (int xByte = 0; xByte < GRIDX_BYTE; xByte++) {
            const uint8_t currentByte = grid_[xByte][y];
            uint8_t nextByte = 0;

            for (int bit = 0; bit < 8; bit++) {
                const int x = (xByte << 3) + bit;
                const uint8_t aliveNeighbors = countNeighbors(grid_, x, y, GRIDX, GRIDY);

                if (nextCellState((currentByte & (1 << bit)) != 0, aliveNeighbors, golType_)) {
                    nextByte |= (1 << bit);
                }
            }

            newGrid_[xByte][y] = nextByte;
            changes += countBits(oldGrid_[xByte][y] ^ nextByte);
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
        for (int xByte = 0; xByte < GRIDX_BYTE; xByte++) {
            const uint8_t currentByte = grid_[xByte][y];
            for (int bit = 0; bit < 8; bit++) {
                const int x = (xByte << 3) + bit;
                const uint16_t color = (currentByte & (1 << bit))
                    ? lifeColor(display_, x, y, countNeighbors(grid_, x, y, GRIDX, GRIDY), generation_)
                    : myBLACK;
                display_->drawPixel(x, y, color);
            }
        }
    }

    generation_++;
    timeOverlay_.drawTimeWithBackground();
}
