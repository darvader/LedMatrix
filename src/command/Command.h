#ifndef COMMAND_H
#define COMMAND_H

#include <Arduino.h>

enum class CommandType {
    Detect,
    Off,
    SetMode,
    SetBrightness,
    SetPointsLeft,
    SetPointsRight,
    SetSetsLeft,
    SetSetsRight,
    SetTeamLeftServes,
    UpdateScore,
    ScrollText,
    SetTimer,
    TimerStart,
    TimerPause,
    StopWatch,
    StopWatchStart,
    StopWatchStop,
    Timeout,
    SetCounter,
    CounterReset,
    Picture,
    Mandelbrot,
    Scoreboard,
    Unknown
};

struct Command {
    CommandType type;
    int intValue;        // mode number, timer value, counter value, brightness, etc.
    bool boolValue;      // teamLeftServes, etc.
    const char* rawPacket;
    int packetLen;
};

#endif // COMMAND_H
