#include <unity.h>
#include "../../src/util/Colors.h"

void test_color565_red() {
    uint16_t c = color565(255, 0, 0);
    // Red: 11111 000000 00000 = 0xF800
    TEST_ASSERT_EQUAL_HEX16(0xF800, c);
}

void test_color565_green() {
    uint16_t c = color565(0, 255, 0);
    // Green: 00000 111111 00000 = 0x07E0
    TEST_ASSERT_EQUAL_HEX16(0x07E0, c);
}

void test_color565_blue() {
    uint16_t c = color565(0, 0, 255);
    // Blue: 00000 000000 11111 = 0x001F
    TEST_ASSERT_EQUAL_HEX16(0x001F, c);
}

void test_color565_white() {
    uint16_t c = color565(255, 255, 255);
    TEST_ASSERT_EQUAL_HEX16(0xFFFF, c);
}

void test_color565_black() {
    uint16_t c = color565(0, 0, 0);
    TEST_ASSERT_EQUAL_HEX16(0x0000, c);
}

void test_color565_yellow() {
    uint16_t c = color565(255, 255, 0);
    // R=31, G=63, B=0 -> 0xFFE0
    TEST_ASSERT_EQUAL_HEX16(0xFFE0, c);
}

void test_predefined_colors() {
    TEST_ASSERT_EQUAL_HEX16(color565(255, 0, 0), myRED);
    TEST_ASSERT_EQUAL_HEX16(color565(0, 255, 0), myGREEN);
    TEST_ASSERT_EQUAL_HEX16(color565(0, 0, 255), myBLUE);
    TEST_ASSERT_EQUAL_HEX16(color565(255, 255, 255), myWHITE);
    TEST_ASSERT_EQUAL_HEX16(0, myBLACK);
}

void test_myCOLORS_array() {
    TEST_ASSERT_EQUAL_HEX16(myBLACK, myCOLORS[0]);
    TEST_ASSERT_EQUAL_HEX16(myRED, myCOLORS[1]);
    TEST_ASSERT_EQUAL_HEX16(myGREEN, myCOLORS[2]);
    TEST_ASSERT_EQUAL_HEX16(myBLUE, myCOLORS[3]);
    TEST_ASSERT_EQUAL_HEX16(myWHITE, myCOLORS[8]);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_color565_red);
    RUN_TEST(test_color565_green);
    RUN_TEST(test_color565_blue);
    RUN_TEST(test_color565_white);
    RUN_TEST(test_color565_black);
    RUN_TEST(test_color565_yellow);
    RUN_TEST(test_predefined_colors);
    RUN_TEST(test_myCOLORS_array);
    return UNITY_END();
}
