#include "GameOfLifeMode.h"
#include "../util/Colors.h"

namespace {
const int8_t GLIDER[][2] = {
    {1, 0}, {2, 1}, {0, 2}, {1, 2}, {2, 2}
};

const int8_t LWSS[][2] = {
    {1, 0}, {4, 0}, {0, 1}, {0, 2}, {4, 2},
    {0, 3}, {1, 3}, {2, 3}, {3, 3}
};

const int8_t BLINKER[][2] = {
    {0, 0}, {1, 0}, {2, 0}
};

const int8_t TOAD[][2] = {
    {1, 0}, {2, 0}, {3, 0}, {0, 1}, {1, 1}, {2, 1}
};

const int8_t BEACON[][2] = {
    {0, 0}, {1, 0}, {0, 1}, {1, 1},
    {2, 2}, {3, 2}, {2, 3}, {3, 3}
};

inline bool cellAt(uint8_t** grid, int x, int y) {
    return (grid[x >> 3][y] & (1 << (x & 7))) != 0;
}

inline int wrapCoordinate(int value, int limit) {
    while (value < 0) value += limit;
    while (value >= limit) value -= limit;
    return value;
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

inline uint8_t clampColor(uint16_t value) {
    return value > 255 ? 255 : static_cast<uint8_t>(value);
}

inline uint8_t scaleColor(uint8_t value, uint8_t scale) {
    return static_cast<uint8_t>((static_cast<uint16_t>(value) * scale) / 255);
}

inline uint16_t lifeColor(IDisplay* display, int x, int y, uint8_t aliveNeighbors,
                          uint16_t generation, uint8_t age, bool born,
                          uint8_t paletteTheme) {
    const uint8_t ageGlow = age > 18 ? 18 : age;
    const uint8_t heat = aliveNeighbors * 20;
    const uint8_t drift = static_cast<uint8_t>(generation + x * 3 + y * 5);
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

    switch (paletteTheme) {
        case 0: // Neon
            red = clampColor(24 + wave8(drift + heat + ageGlow * 3));
            green = clampColor(72 + wave8(drift + 17 + ageGlow * 5));
            blue = clampColor(96 + wave8(drift + 43 + (8 - aliveNeighbors) * 10));
            break;
        case 1: // Fire
            red = clampColor(120 + ageGlow * 7);
            green = clampColor(28 + aliveNeighbors * 18 + wave8(drift + ageGlow * 2) / 3);
            blue = born ? 96 : 12 + wave8(drift + 41) / 6;
            break;
        case 2: // Ocean
            red = born ? 72 : 8 + wave8(drift + 23) / 5;
            green = clampColor(64 + aliveNeighbors * 13 + ageGlow * 4);
            blue = clampColor(116 + wave8(drift + heat + ageGlow * 2) / 2);
            break;
        default: // Rainbow
            red = clampColor(44 + wave8(drift + ageGlow * 4));
            green = clampColor(44 + wave8(drift + 21 + aliveNeighbors * 11));
            blue = clampColor(44 + wave8(drift + 42 + generation / 2));
            break;
    }

    if (born) {
        red = clampColor(red + 48);
        green = clampColor(green + 64);
        blue = clampColor(blue + 72);
    } else {
        red = clampColor(red + ageGlow * 3);
        green = clampColor(green + ageGlow * 2);
    }

    return display->color565(red, green, blue);
}

inline uint16_t fadeColor(IDisplay* display, uint8_t fade, uint8_t paletteTheme) {
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

    switch (paletteTheme) {
        case 0:
            red = 30;
            green = 80;
            blue = 120;
            break;
        case 1:
            red = 120;
            green = 34;
            blue = 6;
            break;
        case 2:
            red = 8;
            green = 58;
            blue = 116;
            break;
        default:
            red = 70;
            green = 45;
            blue = 110;
            break;
    }

    red = scaleColor(red, fade);
    green = scaleColor(green, fade);
    blue = scaleColor(blue, fade);
    return display->color565(red, green, blue);
}
}

GameOfLifeMode::GameOfLifeMode(IDisplay* display, TimeService* timeService)
    : display_(display), timeService_(timeService),
      timeOverlay_(display, timeService),
      oldGrid_(nullptr), grid_(nullptr), newGrid_(nullptr),
      ageGrid_(nullptr), fadeGrid_(nullptr),
      initialized_(false), golType_(-1), paletteTheme_(0), generation_(0), startTime_(0) {}

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
        ageGrid_ = new uint8_t[GRIDX * GRIDY];
        fadeGrid_ = new uint8_t[GRIDX * GRIDY];

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
        for (int i = 0; i < GRIDX * GRIDY; i++) {
            ageGrid_[i] = 0;
            fadeGrid_[i] = 0;
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
        delete[] ageGrid_;
        delete[] fadeGrid_;
        oldGrid_ = nullptr;
        grid_ = nullptr;
        newGrid_ = nullptr;
        ageGrid_ = nullptr;
        fadeGrid_ = nullptr;
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

void GameOfLifeMode::clearWorld() {
    for (int y = 0; y < GRIDY; y++) {
        for (int xByte = 0; xByte < GRIDX_BYTE; xByte++) {
            oldGrid_[xByte][y] = 0;
            grid_[xByte][y] = 0;
            newGrid_[xByte][y] = 0;
        }
    }

    for (int i = 0; i < GRIDX * GRIDY; i++) {
        ageGrid_[i] = 0;
        fadeGrid_[i] = 0;
    }
}

void GameOfLifeMode::initializeVisualStateFromGrid() {
    for (int y = 0; y < GRIDY; y++) {
        for (int x = 0; x < GRIDX; x++) {
            const int index = y * GRIDX + x;
            ageGrid_[index] = getCell(grid_, x, y) ? 1 : 0;
            fadeGrid_[index] = 0;
        }
    }
}

void GameOfLifeMode::placePattern(const int8_t pattern[][2], uint8_t count, int originX,
                                  int originY, uint8_t transform) {
    const bool mirror = (transform & 0x04) != 0;
    const uint8_t rotation = transform & 0x03;

    for (uint8_t i = 0; i < count; i++) {
        int px = pattern[i][0];
        int py = pattern[i][1];
        if (mirror) px = -px;

        int tx = px;
        int ty = py;
        switch (rotation) {
            case 1:
                tx = -py;
                ty = px;
                break;
            case 2:
                tx = -px;
                ty = -py;
                break;
            case 3:
                tx = py;
                ty = -px;
                break;
            default:
                break;
        }

        setCell(grid_, wrapCoordinate(originX + tx, GRIDX),
                wrapCoordinate(originY + ty, GRIDY), true);
    }
}

void GameOfLifeMode::placePulsar(int originX, int originY, uint8_t transform) {
    const int8_t bars[] = {2, 3, 4, 8, 9, 10};
    for (uint8_t i = 0; i < 6; i++) {
        const int8_t p = bars[i];
        const int8_t topBottom[][2] = {
            {p, 0}, {p, 5}, {p, 7}, {p, 12},
            {0, p}, {5, p}, {7, p}, {12, p}
        };
        placePattern(topBottom, 8, originX, originY, transform);
    }
}

void GameOfLifeMode::addRandomPocket(int centerX, int centerY, uint8_t radiusX,
                                     uint8_t radiusY, uint8_t densityPercent) {
    for (int dy = -radiusY; dy <= radiusY; dy++) {
        for (int dx = -radiusX; dx <= radiusX; dx++) {
            if (random(100) < densityPercent) {
                setCell(grid_, wrapCoordinate(centerX + dx, GRIDX),
                        wrapCoordinate(centerY + dy, GRIDY), true);
            }
        }
    }
}

void GameOfLifeMode::seedGliderClusters() {
    const uint8_t gliders = (GRIDX >= 128) ? 18 : 9;
    for (uint8_t i = 0; i < gliders; i++) {
        placePattern(GLIDER, 5, random(GRIDX), random(GRIDY), random(8));
    }

    const uint8_t ships = (GRIDX >= 128) ? 6 : 2;
    for (uint8_t i = 0; i < ships; i++) {
        placePattern(LWSS, 9, random(GRIDX), random(GRIDY), random(8));
    }

    for (uint8_t i = 0; i < 4; i++) {
        addRandomPocket(random(GRIDX), random(GRIDY), 3, 2, 18);
    }
}

void GameOfLifeMode::seedOscillatorField() {
    const uint8_t oscillatorCount = (GRIDX >= 128) ? 20 : 10;
    for (uint8_t i = 0; i < oscillatorCount; i++) {
        const int x = random(GRIDX);
        const int y = random(GRIDY);
        const uint8_t transform = random(8);
        switch (random(3)) {
            case 0:
                placePattern(BLINKER, 3, x, y, transform);
                break;
            case 1:
                placePattern(TOAD, 6, x, y, transform);
                break;
            default:
                placePattern(BEACON, 8, x, y, transform);
                break;
        }
    }

    if (GRIDX >= 96 && GRIDY >= 48) {
        placePulsar(GRIDX / 2 - 6, GRIDY / 2 - 6, random(8));
        if (random(2) == 0) {
            placePulsar(GRIDX / 4 - 6, GRIDY / 2 - 6, random(8));
        }
    }

    for (uint8_t i = 0; i < 3; i++) {
        addRandomPocket(random(GRIDX), random(GRIDY), 4, 3, 12);
    }
}

void GameOfLifeMode::seedSymmetricChaos() {
    const bool mirrorY = random(2) == 0;
    const uint8_t density = (GRIDX >= 128) ? 13 : 15;
    const int halfWidth = GRIDX / 2;
    const int maxY = mirrorY ? GRIDY / 2 : GRIDY;

    for (int y = 0; y < maxY; y++) {
        for (int x = 0; x < halfWidth; x++) {
            if (random(100) < density) {
                setCell(grid_, x, y, true);
                setCell(grid_, GRIDX - 1 - x, y, true);
                if (mirrorY) {
                    setCell(grid_, x, GRIDY - 1 - y, true);
                    setCell(grid_, GRIDX - 1 - x, GRIDY - 1 - y, true);
                }
            }
        }
    }

    placePattern(GLIDER, 5, GRIDX / 2 - 8, GRIDY / 2, 0);
    placePattern(GLIDER, 5, GRIDX / 2 + 8, GRIDY / 2, 6);
}

void GameOfLifeMode::seedSparseNebula() {
    const uint8_t pockets = (GRIDX >= 128) ? 14 : 7;
    for (uint8_t i = 0; i < pockets; i++) {
        addRandomPocket(random(GRIDX), random(GRIDY), random(3, 8), random(2, 6),
                        random(16, 30));
    }

    const uint8_t gliders = (GRIDX >= 128) ? 8 : 4;
    for (uint8_t i = 0; i < gliders; i++) {
        placePattern(GLIDER, 5, random(GRIDX), random(GRIDY), random(8));
    }
}

void GameOfLifeMode::seedCollisionCluster() {
    const int centerX = GRIDX / 2;
    const int centerY = GRIDY / 2;
    const int spreadX = GRIDX / 5;
    const int spreadY = GRIDY / 5;

    placePattern(LWSS, 9, centerX - spreadX, centerY - spreadY, 0);
    placePattern(LWSS, 9, centerX + spreadX, centerY + spreadY, 6);
    placePattern(GLIDER, 5, centerX - spreadX, centerY + spreadY, 1);
    placePattern(GLIDER, 5, centerX + spreadX, centerY - spreadY, 5);

    const uint8_t extraMovers = (GRIDX >= 128) ? 8 : 4;
    for (uint8_t i = 0; i < extraMovers; i++) {
        const int side = random(4);
        const int x = (side < 2) ? random(GRIDX) : (side == 2 ? 4 : GRIDX - 5);
        const int y = (side >= 2) ? random(GRIDY) : (side == 0 ? 4 : GRIDY - 5);
        if (random(2) == 0) {
            placePattern(GLIDER, 5, x, y, random(8));
        } else {
            placePattern(LWSS, 9, x, y, random(8));
        }
    }

    addRandomPocket(centerX, centerY, GRIDX >= 128 ? 12 : 8, GRIDY >= 64 ? 8 : 5, 24);
}

void GameOfLifeMode::seedNewWorld() {
    clearWorld();

    golType_++;
    if (golType_ > 2) golType_ = 0;
    paletteTheme_ = static_cast<uint8_t>(random(4));

    switch (paletteTheme_) {
        case 0:
            if (random(2) == 0) seedOscillatorField();
            else seedCollisionCluster();
            break;
        case 1:
            seedCollisionCluster();
            break;
        case 2:
            if (random(2) == 0) seedSparseNebula();
            else seedGliderClusters();
            break;
        default:
            if (random(2) == 0) seedSymmetricChaos();
            else seedGliderClusters();
            break;
    }

    initializeVisualStateFromGrid();
}

void GameOfLifeMode::update() {
    if (!initialized_) {
        initializeGrids();
        seedNewWorld();
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
            const uint8_t currentByte = grid_[x][y];
            const uint8_t nextByte = newGrid_[x][y];
            oldGrid_[x][y] = currentByte;
            grid_[x][y] = nextByte;

            for (int bit = 0; bit < 8; bit++) {
                const int pixelX = (x << 3) + bit;
                const int index = y * GRIDX + pixelX;
                const bool wasAlive = (currentByte & (1 << bit)) != 0;
                const bool isAlive = (nextByte & (1 << bit)) != 0;

                if (isAlive) {
                    ageGrid_[index] = wasAlive && ageGrid_[index] < 255 ? ageGrid_[index] + 1 : 1;
                    fadeGrid_[index] = 0;
                } else {
                    ageGrid_[index] = 0;
                    if (wasAlive) {
                        fadeGrid_[index] = 160;
                    } else if (fadeGrid_[index] > 28) {
                        fadeGrid_[index] -= 28;
                    } else {
                        fadeGrid_[index] = 0;
                    }
                }
            }
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
                const int index = y * GRIDX + x;
                const bool alive = (currentByte & (1 << bit)) != 0;
                const bool born = alive && ((oldGrid_[xByte][y] & (1 << bit)) == 0);
                const uint16_t color = alive
                    ? lifeColor(display_, x, y, countNeighbors(grid_, x, y, GRIDX, GRIDY),
                                generation_, ageGrid_[index], born, paletteTheme_)
                    : (fadeGrid_[index] > 0 ? fadeColor(display_, fadeGrid_[index], paletteTheme_) : myBLACK);
                display_->drawPixel(x, y, color);
            }
        }
    }

    generation_++;
    timeOverlay_.drawTimeWithBackground();
}
