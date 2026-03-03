#include <unity.h>
#include <cstdint>

// Replicate the pure logic functions from GameOfLifeMode for native testing
// These are bit-packed grid operations that don't depend on Arduino or display

static bool getCell(uint8_t** grid, uint8_t x, uint8_t y) {
    uint8_t xByte = x / 8;
    uint8_t xBit = x % 8;
    return (grid[xByte][y] & (1 << xBit)) != 0;
}

static void setCell(uint8_t** grid, uint8_t x, uint8_t y, bool value) {
    uint8_t byteIndex = x / 8;
    uint8_t bitIndex = x % 8;
    if (value) {
        grid[byteIndex][y] |= (1 << bitIndex);
    } else {
        grid[byteIndex][y] &= ~(1 << bitIndex);
    }
}

static int countAliveNeighbors(uint8_t** grid, uint8_t x, uint8_t y,
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

// Small test grid: 16 wide (2 bytes) x 4 high
static const int TEST_GRIDX = 16;
static const int TEST_GRIDX_BYTE = 2;
static const int TEST_GRIDY = 4;

static uint8_t** createGrid() {
    uint8_t** g = new uint8_t*[TEST_GRIDX_BYTE];
    for (int i = 0; i < TEST_GRIDX_BYTE; i++) {
        g[i] = new uint8_t[TEST_GRIDY];
        for (int j = 0; j < TEST_GRIDY; j++) g[i][j] = 0;
    }
    return g;
}

static void freeGrid(uint8_t** g) {
    for (int i = 0; i < TEST_GRIDX_BYTE; i++) delete[] g[i];
    delete[] g;
}

void test_set_and_get_cell() {
    auto** grid = createGrid();

    setCell(grid, 0, 0, true);
    TEST_ASSERT_TRUE(getCell(grid, 0, 0));
    TEST_ASSERT_FALSE(getCell(grid, 1, 0));

    setCell(grid, 7, 2, true);
    TEST_ASSERT_TRUE(getCell(grid, 7, 2));

    setCell(grid, 15, 3, true);
    TEST_ASSERT_TRUE(getCell(grid, 15, 3));

    setCell(grid, 0, 0, false);
    TEST_ASSERT_FALSE(getCell(grid, 0, 0));

    freeGrid(grid);
}

void test_count_neighbors_no_neighbors() {
    auto** grid = createGrid();
    setCell(grid, 5, 2, true);
    int count = countAliveNeighbors(grid, 5, 2, TEST_GRIDX, TEST_GRIDY);
    TEST_ASSERT_EQUAL(0, count);
    freeGrid(grid);
}

void test_count_neighbors_all_surrounded() {
    auto** grid = createGrid();
    setCell(grid, 4, 1, true);
    setCell(grid, 5, 1, true);
    setCell(grid, 6, 1, true);
    setCell(grid, 4, 2, true);
    setCell(grid, 6, 2, true);
    setCell(grid, 4, 3, true);
    setCell(grid, 5, 3, true);
    setCell(grid, 6, 3, true);

    int count = countAliveNeighbors(grid, 5, 2, TEST_GRIDX, TEST_GRIDY);
    TEST_ASSERT_EQUAL(8, count);
    freeGrid(grid);
}

void test_count_neighbors_wrapping() {
    auto** grid = createGrid();
    setCell(grid, 15, 3, true);
    int count = countAliveNeighbors(grid, 0, 0, TEST_GRIDX, TEST_GRIDY);
    TEST_ASSERT_EQUAL(1, count);
    freeGrid(grid);
}

void test_gol_birth_rule() {
    auto** grid = createGrid();
    setCell(grid, 4, 1, true);
    setCell(grid, 5, 1, true);
    setCell(grid, 6, 1, true);

    int count = countAliveNeighbors(grid, 5, 2, TEST_GRIDX, TEST_GRIDY);
    TEST_ASSERT_EQUAL(3, count);

    freeGrid(grid);
}

void test_gol_survival_rule() {
    auto** grid = createGrid();
    setCell(grid, 5, 2, true);
    setCell(grid, 4, 1, true);
    setCell(grid, 6, 1, true);

    int count = countAliveNeighbors(grid, 5, 2, TEST_GRIDX, TEST_GRIDY);
    TEST_ASSERT_EQUAL(2, count);

    freeGrid(grid);
}

void test_gol_death_rule() {
    auto** grid = createGrid();
    setCell(grid, 5, 2, true);
    setCell(grid, 4, 1, true);

    int count = countAliveNeighbors(grid, 5, 2, TEST_GRIDX, TEST_GRIDY);
    TEST_ASSERT_EQUAL(1, count);

    freeGrid(grid);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_set_and_get_cell);
    RUN_TEST(test_count_neighbors_no_neighbors);
    RUN_TEST(test_count_neighbors_all_surrounded);
    RUN_TEST(test_count_neighbors_wrapping);
    RUN_TEST(test_gol_birth_rule);
    RUN_TEST(test_gol_survival_rule);
    RUN_TEST(test_gol_death_rule);
    return UNITY_END();
}
