#include "CommandParser.h"
#include <cstring>

Command CommandParser::parse(const char* packet, int len) {
    Command cmd;
    cmd.type = CommandType::Unknown;
    cmd.intValue = 0;
    cmd.boolValue = false;
    cmd.rawPacket = packet;
    cmd.packetLen = len;

    if (std::strcmp(packet, "Detect") == 0) {
        cmd.type = CommandType::Detect;
    } else if (strstr(packet, "off") != NULL) {
        cmd.type = CommandType::Off;
    } else if (strstr(packet, "pointsLeft=") != NULL) {
        cmd.type = CommandType::SetPointsLeft;
        cmd.intValue = packet[11];
    } else if (strstr(packet, "pointsRight=") != NULL) {
        cmd.type = CommandType::SetPointsRight;
        cmd.intValue = packet[12];
    } else if (strstr(packet, "teamLeftServes=") != NULL) {
        cmd.type = CommandType::SetTeamLeftServes;
        cmd.boolValue = packet[15];
    } else if (strstr(packet, "setsLeft=") != NULL) {
        cmd.type = CommandType::SetSetsLeft;
        cmd.intValue = packet[9];
    } else if (strstr(packet, "setsRight=") != NULL) {
        cmd.type = CommandType::SetSetsRight;
        cmd.intValue = packet[10];
    } else if (strstr(packet, "updateScore=") != NULL) {
        cmd.type = CommandType::UpdateScore;
    } else if (strstr(packet, "scrollText=") != NULL) {
        cmd.type = CommandType::ScrollText;
    } else if (strstr(packet, "brightness=") != NULL) {
        cmd.type = CommandType::SetBrightness;
        cmd.intValue = packet[11];
    } else if (strstr(packet, "timer=") != NULL) {
        cmd.type = CommandType::SetTimer;
        cmd.intValue = (packet[6] << 8) | packet[7];
    } else if (std::strcmp(packet, "timerStart") == 0) {
        cmd.type = CommandType::TimerStart;
    } else if (std::strcmp(packet, "timerPause") == 0) {
        cmd.type = CommandType::TimerPause;
    } else if (std::strcmp(packet, "stopWatch") == 0) {
        cmd.type = CommandType::StopWatch;
    } else if (std::strcmp(packet, "stopWatchStart") == 0) {
        cmd.type = CommandType::StopWatchStart;
    } else if (std::strcmp(packet, "stopWatchStop") == 0) {
        cmd.type = CommandType::StopWatchStop;
    } else if (std::strcmp(packet, "timeout") == 0) {
        cmd.type = CommandType::Timeout;
    } else if (std::strcmp(packet, "time1") == 0) {
        cmd.type = CommandType::SetMode;
        cmd.intValue = 2;
    } else if (std::strcmp(packet, "time2") == 0) {
        cmd.type = CommandType::SetMode;
        cmd.intValue = 3;
    } else if (std::strcmp(packet, "time3") == 0) {
        cmd.type = CommandType::SetMode;
        cmd.intValue = 4;
    } else if (std::strcmp(packet, "time4") == 0) {
        cmd.type = CommandType::SetMode;
        cmd.intValue = 5;
    } else if (std::strcmp(packet, "timeSnow") == 0) {
        cmd.type = CommandType::SetMode;
        cmd.intValue = 6;
    } else if (std::strcmp(packet, "timePlasma") == 0) {
        cmd.type = CommandType::SetMode;
        cmd.intValue = 7;
    } else if (std::strcmp(packet, "timeColoredSnow") == 0) {
        cmd.type = CommandType::SetMode;
        cmd.intValue = 8;
    } else if (std::strcmp(packet, "timeGameOfLife") == 0) {
        cmd.type = CommandType::SetMode;
        cmd.intValue = 9;
    } else if (std::strcmp(packet, "timeEllipse") == 0) {
        cmd.type = CommandType::SetMode;
        cmd.intValue = 11;
    } else if (std::strcmp(packet, "timeStarWars") == 0) {
        cmd.type = CommandType::SetMode;
        cmd.intValue = 12;
    } else if (std::strcmp(packet, "scoreboard") == 0) {
        cmd.type = CommandType::Scoreboard;
    } else if (std::strcmp(packet, "mandelbrot") == 0) {
        cmd.type = CommandType::Mandelbrot;
    } else if (std::strcmp(packet, "picture") == 0) {
        cmd.type = CommandType::Picture;
    } else if (strstr(packet, "counter=") != NULL) {
        cmd.type = CommandType::SetCounter;
        cmd.intValue = (packet[8] << 8) | packet[9];
    } else if (strstr(packet, "counterReset") != NULL) {
        cmd.type = CommandType::CounterReset;
    }

    return cmd;
}
