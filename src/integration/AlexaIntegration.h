#ifndef ALEXA_INTEGRATION_H
#define ALEXA_INTEGRATION_H

#include <fauxmoESP.h>
#include "../app/ModeManager.h"
#include "../display/IDisplay.h"

class AlexaIntegration {
public:
    AlexaIntegration(ModeManager* modeManager, IDisplay* display);

    void setup();
    void handle();

    void setOff(bool off) { off_ = off; }
    bool isOff() const { return off_; }
    void setDisplayDrawTime(uint8_t val) { displayDrawTime_ = val; }
    uint8_t getDisplayDrawTime() const { return displayDrawTime_; }

private:
    fauxmoESP fauxmo_;
    ModeManager* modeManager_;
    IDisplay* display_;
    bool off_;
    uint8_t displayDrawTime_;
};

#endif // ALEXA_INTEGRATION_H
