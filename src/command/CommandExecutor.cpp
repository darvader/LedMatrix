#include "CommandExecutor.h"
#include "../app/ModeManager.h"
#include "../mode/ScoreboardMode.h"
#include "../mode/TimerMode.h"
#include "../mode/CounterMode.h"
#include "../mode/MandelbrotMode.h"
#include "../mode/SnowMode.h"
#include "../mode/GameOfLifeMode.h"
#include "../display/IDisplay.h"
#include "../app/Config.h"
#include <EEPROM.h>

CommandExecutor::CommandExecutor(ModeManager* modeManager, ScoreboardMode* scoreboard,
                                 TimerMode* timer, CounterMode* counter,
                                 MandelbrotMode* mandelbrot,
                                 SnowMode* whiteSnow, SnowMode* coloredSnow,
                                 GameOfLifeMode* gameOfLife,
                                 IDisplay* display)
    : modeManager_(modeManager), scoreboard_(scoreboard),
      timer_(timer), counter_(counter), mandelbrot_(mandelbrot),
      whiteSnow_(whiteSnow), coloredSnow_(coloredSnow),
      gameOfLife_(gameOfLife), display_(display),
      displayDrawTime_(DEFAULT_DISPLAY_DRAW_TIME), off_(false) {}

void CommandExecutor::execute(const Command& cmd) {
    switch (cmd.type) {
        case CommandType::Off:
            off_ = true;
            display_->showBuffer();
            display_->clearScreen();
            return;

        case CommandType::SetPointsLeft:
            off_ = false;
            scoreboard_->setPointsLeft(cmd.intValue);
            modeManager_->setMode(1);
            return;

        case CommandType::SetPointsRight:
            off_ = false;
            scoreboard_->setPointsRight(cmd.intValue);
            modeManager_->setMode(1);
            return;

        case CommandType::SetTeamLeftServes:
            off_ = false;
            scoreboard_->setTeamLeftServes(cmd.boolValue);
            modeManager_->setMode(1);
            return;

        case CommandType::SetSetsLeft:
            off_ = false;
            scoreboard_->setSetsLeft(cmd.intValue);
            modeManager_->setMode(1);
            return;

        case CommandType::SetSetsRight:
            off_ = false;
            scoreboard_->setSetsRight(cmd.intValue);
            modeManager_->setMode(1);
            return;

        case CommandType::UpdateScore:
            off_ = false;
            scoreboard_->updateScore(cmd.rawPacket);
            modeManager_->setMode(1);
            return;

        case CommandType::ScrollText:
            off_ = false;
            scoreboard_->updateScrollingText(cmd.rawPacket);
            modeManager_->setMode(1);
            return;

        case CommandType::SetBrightness:
            displayDrawTime_ = cmd.intValue;
            display_->setBrightness(displayDrawTime_ * 2.55);
            return;

        case CommandType::SetTimer:
            off_ = false;
            timer_->setTimer(cmd.intValue);
            modeManager_->setMode(30);
            return;

        case CommandType::TimerStart:
            off_ = false;
            timer_->start();
            return;

        case CommandType::TimerPause:
            off_ = false;
            timer_->pause();
            return;

        case CommandType::StopWatch:
            off_ = false;
            timer_->stopWatch();
            modeManager_->setMode(30);
            return;

        case CommandType::StopWatchStart:
            off_ = false;
            timer_->stopWatchStart();
            return;

        case CommandType::StopWatchStop:
            off_ = false;
            timer_->stopWatchStop();
            return;

        case CommandType::Timeout:
            off_ = false;
            scoreboard_->startTimeout();
            return;

        case CommandType::SetMode:
            off_ = false;
            // Reset mode-specific state for snow/GOL
            if (cmd.intValue == 6) whiteSnow_->resetInitialized();
            if (cmd.intValue == 8) coloredSnow_->resetInitialized();
            if (cmd.intValue == 9) gameOfLife_->resetInitialized();
            modeManager_->setMode(cmd.intValue);
            return;

        case CommandType::Scoreboard:
            off_ = false;
            modeManager_->setMode(1);
            return;

        case CommandType::Mandelbrot:
            off_ = false;
            mandelbrot_->resetZoom();
            modeManager_->setMode(60);
            return;

        case CommandType::SetCounter: {
            off_ = false;
            uint16_t counterValue = cmd.intValue;
            counter_->setCounter(counterValue);
            modeManager_->setMode(40);
            EEPROM.write(EEPROM_MODE_ADDR, 40);
#ifdef ESP32
            EEPROM.writeUInt(EEPROM_COUNTER_ADDR, counterValue);
#else
            EEPROM.write(EEPROM_COUNTER_ADDR, counterValue & 0xFF);
            EEPROM.write(EEPROM_COUNTER_ADDR + 1, (counterValue >> 8) & 0xFF);
#endif
            EEPROM.commit();
            return;
        }

        case CommandType::CounterReset:
            off_ = false;
            counter_->resetCounter();
            modeManager_->setMode(40);
            return;

        case CommandType::Picture:
            off_ = false;
            modeManager_->setMode(20);
            return;

        case CommandType::Detect:
        case CommandType::Unknown:
            off_ = false;
            return;
    }
}
