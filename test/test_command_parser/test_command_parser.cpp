#include <unity.h>
#include "../../src/command/CommandParser.h"

void test_detect() {
    Command cmd = CommandParser::parse("Detect", 6);
    TEST_ASSERT_EQUAL(CommandType::Detect, cmd.type);
}

void test_off() {
    Command cmd = CommandParser::parse("off", 3);
    TEST_ASSERT_EQUAL(CommandType::Off, cmd.type);
}

void test_time1() {
    Command cmd = CommandParser::parse("time1", 5);
    TEST_ASSERT_EQUAL(CommandType::SetMode, cmd.type);
    TEST_ASSERT_EQUAL(2, cmd.intValue);
}

void test_time2() {
    Command cmd = CommandParser::parse("time2", 5);
    TEST_ASSERT_EQUAL(CommandType::SetMode, cmd.type);
    TEST_ASSERT_EQUAL(3, cmd.intValue);
}

void test_time3() {
    Command cmd = CommandParser::parse("time3", 5);
    TEST_ASSERT_EQUAL(CommandType::SetMode, cmd.type);
    TEST_ASSERT_EQUAL(4, cmd.intValue);
}

void test_time4() {
    Command cmd = CommandParser::parse("time4", 5);
    TEST_ASSERT_EQUAL(CommandType::SetMode, cmd.type);
    TEST_ASSERT_EQUAL(5, cmd.intValue);
}

void test_timeSnow() {
    Command cmd = CommandParser::parse("timeSnow", 8);
    TEST_ASSERT_EQUAL(CommandType::SetMode, cmd.type);
    TEST_ASSERT_EQUAL(6, cmd.intValue);
}

void test_timePlasma() {
    Command cmd = CommandParser::parse("timePlasma", 10);
    TEST_ASSERT_EQUAL(CommandType::SetMode, cmd.type);
    TEST_ASSERT_EQUAL(7, cmd.intValue);
}

void test_timeColoredSnow() {
    Command cmd = CommandParser::parse("timeColoredSnow", 15);
    TEST_ASSERT_EQUAL(CommandType::SetMode, cmd.type);
    TEST_ASSERT_EQUAL(8, cmd.intValue);
}

void test_timeGameOfLife() {
    Command cmd = CommandParser::parse("timeGameOfLife", 14);
    TEST_ASSERT_EQUAL(CommandType::SetMode, cmd.type);
    TEST_ASSERT_EQUAL(9, cmd.intValue);
}

void test_timeEllipse() {
    Command cmd = CommandParser::parse("timeEllipse", 11);
    TEST_ASSERT_EQUAL(CommandType::SetMode, cmd.type);
    TEST_ASSERT_EQUAL(11, cmd.intValue);
}

void test_timeStarWars() {
    Command cmd = CommandParser::parse("timeStarWars", 12);
    TEST_ASSERT_EQUAL(CommandType::SetMode, cmd.type);
    TEST_ASSERT_EQUAL(12, cmd.intValue);
}

void test_scoreboard() {
    Command cmd = CommandParser::parse("scoreboard", 10);
    TEST_ASSERT_EQUAL(CommandType::Scoreboard, cmd.type);
}

void test_mandelbrot() {
    Command cmd = CommandParser::parse("mandelbrot", 10);
    TEST_ASSERT_EQUAL(CommandType::Mandelbrot, cmd.type);
}

void test_picture() {
    Command cmd = CommandParser::parse("picture", 7);
    TEST_ASSERT_EQUAL(CommandType::Picture, cmd.type);
}

void test_timerStart() {
    Command cmd = CommandParser::parse("timerStart", 10);
    TEST_ASSERT_EQUAL(CommandType::TimerStart, cmd.type);
}

void test_timerPause() {
    Command cmd = CommandParser::parse("timerPause", 10);
    TEST_ASSERT_EQUAL(CommandType::TimerPause, cmd.type);
}

void test_stopWatch() {
    Command cmd = CommandParser::parse("stopWatch", 9);
    TEST_ASSERT_EQUAL(CommandType::StopWatch, cmd.type);
}

void test_stopWatchStart() {
    Command cmd = CommandParser::parse("stopWatchStart", 14);
    TEST_ASSERT_EQUAL(CommandType::StopWatchStart, cmd.type);
}

void test_stopWatchStop() {
    Command cmd = CommandParser::parse("stopWatchStop", 13);
    TEST_ASSERT_EQUAL(CommandType::StopWatchStop, cmd.type);
}

void test_timeout() {
    Command cmd = CommandParser::parse("timeout", 7);
    TEST_ASSERT_EQUAL(CommandType::Timeout, cmd.type);
}

void test_counterReset() {
    Command cmd = CommandParser::parse("counterReset", 12);
    TEST_ASSERT_EQUAL(CommandType::CounterReset, cmd.type);
}

void test_pointsLeft() {
    char packet[] = "pointsLeft=\x0F";
    Command cmd = CommandParser::parse(packet, 12);
    TEST_ASSERT_EQUAL(CommandType::SetPointsLeft, cmd.type);
    TEST_ASSERT_EQUAL(15, cmd.intValue);
}

void test_pointsRight() {
    char packet[] = "pointsRight=\x10";
    Command cmd = CommandParser::parse(packet, 13);
    TEST_ASSERT_EQUAL(CommandType::SetPointsRight, cmd.type);
    TEST_ASSERT_EQUAL(16, cmd.intValue);
}

void test_setsLeft() {
    char packet[] = "setsLeft=\x02";
    Command cmd = CommandParser::parse(packet, 10);
    TEST_ASSERT_EQUAL(CommandType::SetSetsLeft, cmd.type);
    TEST_ASSERT_EQUAL(2, cmd.intValue);
}

void test_setsRight() {
    char packet[] = "setsRight=\x01";
    Command cmd = CommandParser::parse(packet, 11);
    TEST_ASSERT_EQUAL(CommandType::SetSetsRight, cmd.type);
    TEST_ASSERT_EQUAL(1, cmd.intValue);
}

void test_brightness() {
    char packet[] = "brightness=\x32";
    Command cmd = CommandParser::parse(packet, 12);
    TEST_ASSERT_EQUAL(CommandType::SetBrightness, cmd.type);
    TEST_ASSERT_EQUAL(50, cmd.intValue);
}

void test_unknown() {
    Command cmd = CommandParser::parse("randomgarbage", 13);
    TEST_ASSERT_EQUAL(CommandType::Unknown, cmd.type);
}

void test_updateScore() {
    Command cmd = CommandParser::parse("updateScore=data", 16);
    TEST_ASSERT_EQUAL(CommandType::UpdateScore, cmd.type);
}

void test_scrollText() {
    Command cmd = CommandParser::parse("scrollText=hello", 16);
    TEST_ASSERT_EQUAL(CommandType::ScrollText, cmd.type);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_detect);
    RUN_TEST(test_off);
    RUN_TEST(test_time1);
    RUN_TEST(test_time2);
    RUN_TEST(test_time3);
    RUN_TEST(test_time4);
    RUN_TEST(test_timeSnow);
    RUN_TEST(test_timePlasma);
    RUN_TEST(test_timeColoredSnow);
    RUN_TEST(test_timeGameOfLife);
    RUN_TEST(test_timeEllipse);
    RUN_TEST(test_timeStarWars);
    RUN_TEST(test_scoreboard);
    RUN_TEST(test_mandelbrot);
    RUN_TEST(test_picture);
    RUN_TEST(test_timerStart);
    RUN_TEST(test_timerPause);
    RUN_TEST(test_stopWatch);
    RUN_TEST(test_stopWatchStart);
    RUN_TEST(test_stopWatchStop);
    RUN_TEST(test_timeout);
    RUN_TEST(test_counterReset);
    RUN_TEST(test_pointsLeft);
    RUN_TEST(test_pointsRight);
    RUN_TEST(test_setsLeft);
    RUN_TEST(test_setsRight);
    RUN_TEST(test_brightness);
    RUN_TEST(test_unknown);
    RUN_TEST(test_updateScore);
    RUN_TEST(test_scrollText);
    return UNITY_END();
}
