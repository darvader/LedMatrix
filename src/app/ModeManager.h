#ifndef MODE_MANAGER_H
#define MODE_MANAGER_H

#include "../mode/IDisplayMode.h"
#include <Arduino.h>

class ModeManager {
public:
    ModeManager();

    void registerMode(int id, IDisplayMode* mode);
    void setMode(int id);
    int getMode() const { return currentModeId_; }
    void update();
    IDisplayMode* getCurrentMode() const { return currentMode_; }
    uint16_t getFrameDelay() const;

    // Callback for mode changes
    using ModeChangeCallback = void (*)(int newMode);
    void setModeChangeCallback(ModeChangeCallback cb) { modeChangeCallback_ = cb; }

private:
    static const int MAX_MODES = 64;
    IDisplayMode* modes_[MAX_MODES];
    IDisplayMode* currentMode_;
    int currentModeId_;
    ModeChangeCallback modeChangeCallback_;
};

#endif // MODE_MANAGER_H
