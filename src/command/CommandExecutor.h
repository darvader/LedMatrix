#ifndef COMMAND_EXECUTOR_H
#define COMMAND_EXECUTOR_H

#include "Command.h"

class ModeManager;
class ScoreboardMode;
class TimerMode;
class CounterMode;
class MandelbrotMode;
class SnowMode;
class GameOfLifeMode;
class IDisplay;

class CommandExecutor {
public:
    CommandExecutor(ModeManager* modeManager, ScoreboardMode* scoreboard,
                    TimerMode* timer, CounterMode* counter,
                    MandelbrotMode* mandelbrot,
                    SnowMode* whiteSnow, SnowMode* coloredSnow,
                    GameOfLifeMode* gameOfLife,
                    IDisplay* display);

    void execute(const Command& cmd);

    // Accessors for state needed by Application
    uint8_t getDisplayDrawTime() const { return displayDrawTime_; }
    void setDisplayDrawTime(uint8_t val) { displayDrawTime_ = val; }
    bool isOff() const { return off_; }

private:
    ModeManager* modeManager_;
    ScoreboardMode* scoreboard_;
    TimerMode* timer_;
    CounterMode* counter_;
    MandelbrotMode* mandelbrot_;
    SnowMode* whiteSnow_;
    SnowMode* coloredSnow_;
    GameOfLifeMode* gameOfLife_;
    IDisplay* display_;

    uint8_t displayDrawTime_;
    bool off_;
};

#endif // COMMAND_EXECUTOR_H
